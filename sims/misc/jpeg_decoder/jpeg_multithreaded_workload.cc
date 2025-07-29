#include <fcntl.h>
#include <linux/vfio.h>
#include <sys/mman.h>
#include <sys/time.h>
#include <unistd.h>

#include <cassert>
#include <cmath>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <ios>
#include <iostream>
#include <sstream>
#include <string>

#include "include/jpeg_decoder_regs.hh"
#include "include/vfio.h"

/* Very simple continuous memory allocator. We assume that there's a physically
 * continuous region of memory of size 512 MiB, located at dma_addr_start,
 * marked as reserved with the Linux kernel. */

static uint64_t cma_phys_start = 0;
static uint8_t *cma_base = nullptr;
static uint8_t *cma_current = nullptr;

void cma_init(uint64_t dma_addr_start) {
  std::cout << "simbricks-pci: initializing allocator\n";
  cma_phys_start = dma_addr_start;
  int fd = open("/dev/mem", O_RDWR);
  if (fd < 0) {
    std::cerr << "opening devmem failed" << std::endl;
    std::terminate();
  }

  cma_current = cma_base = static_cast<uint8_t *>(
      mmap(nullptr, 512 * 1024 * 1024, PROT_READ | PROT_WRITE, MAP_SHARED, fd,
           dma_addr_start));
  if (cma_base == MAP_FAILED) {
    std::cerr << "mmap devmem failed" << std::endl;
    std::terminate();
  }

  std::cout << "simbricks-pci: allocator initialized\n";
}

uint32_t roundTo(uint32_t value, uint32_t roundTo) {
  return (value + (roundTo - 1)) & ~(roundTo - 1);
}

uint8_t *cma_alloc(uint32_t size) {
  uint8_t *alloc = cma_current;
  // make sure next allocation is aligned to 64 bit
  cma_current += roundTo(size, 8);
  return alloc;
}

uint64_t cma_phys_addr(uint8_t *buf) {
  return cma_phys_start + (buf - cma_base);
}

/* Define workload. Taken from
 * NEX/test/jpeg_decoder_test/multiple_device_decode.c and modified. */
double get_real_time() {
  struct timespec now;
  timespec_get(&now, TIME_UTC);
  return now.tv_sec + now.tv_nsec / 1000000000.0;
}

uint64_t get_microseconds() {
  struct timeval tv;
  gettimeofday(&tv, NULL);
  return tv.tv_sec * 1000000 + tv.tv_usec;
}

void load_image(uint8_t *image_data, uint32_t *image_size,
                const char *filename) {
  FILE *f = fopen(filename, "rb");
  if (f) {
    long size;

    // Get size
    fseek(f, 0, SEEK_END);
    size = ftell(f);
    rewind(f);

    // Read file data in
    *image_size = fread(image_data, 1, size, f);

    printf("Successfully loaded image with size: %d\n", *image_size);
    fclose(f);
  } else {
    printf("Could not load image %s:\n", filename);
    abort();
  }
}

// Define a task that holds the image path and output buffer pointer.
typedef struct {
  const char *image_path;
  void *output_buffer;
} task_t;

// Global task queue variables.
static task_t *tasks = NULL;
static int num_tasks = 0;
static int current_task_index = 0;
pthread_mutex_t queue_mutex = PTHREAD_MUTEX_INITIALIZER;

static volatile JpegDecoderRegs **jpeg_ctrls;
static uint8_t **input_bufs;
static uint8_t **output_bufs;

void decode_jpeg_on_device(int dev_id, unsigned size) {
  printf("Decoding JPEG\n");
  // Simulating Register MMIO address
  volatile JpegDecoderRegs *regs = jpeg_ctrls[dev_id];

  printf("Addr: %lu\n", cma_phys_addr(input_bufs[dev_id]));
  // Write src address
  regs->src = cma_phys_addr(input_bufs[dev_id]);

  printf("Dst: %lu\n", cma_phys_addr(output_bufs[dev_id]));
  // // Write destination address
  regs->dst = cma_phys_addr(output_bufs[dev_id]);

  printf("Size: %u\n", size | CTRL_REG_START_BIT);
  // // Start decoding
  regs->ctrl = size | CTRL_REG_START_BIT;
}

int decode_done_on_device(int dev_id) {
  volatile JpegDecoderRegs *regs = jpeg_ctrls[dev_id];
  return !regs->isBusy;
}

// Two-dimensional matrix kernel filtering using a 3x3 average filter.
void matrix_kernel_filter_2d(uint8_t *buffer, uint32_t width, uint32_t height) {
  // Allocate a temporary buffer to hold a copy of the original data.
  uint8_t *temp = buffer;

  // Process the image while avoiding the border pixels.
  for (uint32_t row = 1; row < height - 1; row++) {
      for (uint32_t col = 1; col < width - 1; col++) {
          uint32_t sum = 0;
          // Sum all values in the 3x3 neighborhood.
          for (int i = -1; i <= 1; i++) {
              for (int j = -1; j <= 1; j++) {
                  sum += temp[(row + i) * width + (col + j)];
              }
          }
          // Assign the average to the current pixel.
          buffer[row * width + col] = sum / 9;
      }
  }
}


// Worker thread function.
// Each thread uses its device id (passed via arg) to launch decoding tasks.
void *worker_thread(void *arg) {
  int dev_id = *(int *)arg;
  free(arg);  // Free the allocated memory for device id.

  while (1) {
    int task_idx = -1;
    pthread_mutex_lock(&queue_mutex);
    if (current_task_index < num_tasks) {
      task_idx = current_task_index++;
    }
    pthread_mutex_unlock(&queue_mutex);

    if (task_idx == -1) {
      break;  // No more tasks.
    }

    // Process the task.
    const char *path = tasks[task_idx].image_path;

    uint8_t *image_data = input_bufs[dev_id];
    uint32_t image_size = 0;

    // // Load the image.
    load_image(image_data, &image_size, path);

    // // Launch decoding on the specific device.
    printf("Thread (device %d) decoding image \"%s\"\n", dev_id, path);

    // int cnt = 1000;
    // while(cnt-->0){
    //     usleep(100);
    // }
    uint64_t start = get_microseconds();
    decode_jpeg_on_device(dev_id, image_size);
    while (!decode_done_on_device(dev_id)) {
      usleep(400);
      // printf("Dev %d time now %ld\n", dev_id, get_microseconds());
    }
    uint64_t end = get_microseconds();
    
    // free(image_data);
    printf("Thread (device %d) finished decoding image \"%s\", duration %lu \n", dev_id, path, end-start);
    printf("value of width and height %d, %d\n", image_size/100, image_size/100);
    matrix_kernel_filter_2d(output_bufs[dev_id], std::min(2048, (int)image_size/100), std::min(2048, (int)image_size/100)); // Assuming a fixed width and height for simplicity.
    uint64_t end_post_processing = get_microseconds();
    printf("Thread (device %d) finished post_processing, duration %lu \n", dev_id, end_post_processing - end);
  }
  return NULL;
}

// Exported function to decode multiple JPEG images concurrently.
// input_paths: an array of image file paths.
// num_images: number of images/tasks.
// num_threads: number of worker threads (and device ids) to use.
int decode_images_multithreaded(const char **input_paths, int num_images,
                                int num_threads) {
  // Allocate and initialize the task queue.
  num_tasks = num_images;
  current_task_index = 0;
  tasks = (task_t *)malloc(sizeof(task_t) * num_tasks);
  if (!tasks) {
    fprintf(stderr, "ERROR: Failed to allocate task queue\n");
    return -1;
  }
  for (int i = 0; i < num_tasks; i++) {
    tasks[i].image_path = input_paths[i];
    tasks[i].output_buffer = output_bufs[i];
  }

  // Create worker threads.
  pthread_t *threads = (pthread_t *)malloc(sizeof(pthread_t) * num_threads);
  if (!threads) {
    fprintf(stderr, "ERROR: Failed to allocate threads\n");
    free(tasks);
    return -1;
  }

  for (int i = 0; i < num_threads; i++) {
    int *dev_id = (int *)malloc(sizeof(int));
    *dev_id = i;  // Each thread gets a device id equal to its index.
    if (pthread_create(&threads[i], NULL, worker_thread, dev_id) != 0) {
      fprintf(stderr, "ERROR: Failed to create thread %d\n", i);
      free(threads);
      free(tasks);
      return -1;
    }
  }

  // Wait for all threads to finish.
  for (int i = 0; i < num_threads; i++) {
    pthread_join(threads[i], NULL);
  }

  free(threads);
  free(tasks);

  return 0;
}

int main(int argc, char *argv[]) {
  if (argc != 4) {
    std::cerr << "usage: jpeg_multithreaded_workload_driver PCI-ID-START "
                 "NUM-JPEG-DECODERS DMA-ADDR-START\n";
    return EXIT_FAILURE;
  }

  uint16_t pci_id_start = std::stoi(argv[1], nullptr, 0);
  uint16_t num_decoders = std::stoi(argv[2], nullptr, 0);
  uint64_t dma_addr_start = std::stoull(argv[3], nullptr, 0);

  cma_init(dma_addr_start);

  int *vfio_fds = new int[num_decoders];
  jpeg_ctrls = new volatile JpegDecoderRegs *[num_decoders];
  input_bufs = new uint8_t *[num_decoders];
  output_bufs = new uint8_t *[num_decoders];
  for (int i = 0; i < num_decoders; ++i) {
    std::stringstream pci_id;
    pci_id << "0000:00:0" << std::hex << pci_id_start++ << ".0";
    vfio_fds[i] = vfio_init(pci_id.str().c_str(), i);
    if (vfio_fds[i] < 0) {
      std::cerr << "vfio init for decoder " << i << " failed" << std::endl;
      return 1;
    }

    // required for DMA
    if (vfio_busmaster_enable(vfio_fds[i])) {
      std::cerr << "vfio busmaster enable for device " << i << " failed"
                << std::endl;
      return 1;
    }

    // map control registers
    void *bar0;
    size_t reg_len;
    if (vfio_map_region(vfio_fds[i], 0, &bar0, &reg_len)) {
      std::cerr << "vfio_map_region for bar 0 for device " << i << " failed"
                << std::endl;
      return 1;
    }
    jpeg_ctrls[i] = static_cast<volatile JpegDecoderRegs *>(bar0);

    // allocate in- and output buffers
    input_bufs[i] = cma_alloc(1 << 24);  // JPEG decoder only has 24 bits on
                                         // input buffer length register
    // output_bufs[i] = cma_alloc(
    //     4032 * 3024 * 2);  // largest image we decode is 4032 x 3024 pixels
    
    output_bufs[i] = cma_alloc(
        4096 * 4096 * 3);  // largest image we decode is 4032 x 3024 pixels
  }

  std::cout << "Initialization of all decoders done, starting workload\n";

  int img_idx_list[36] = {1,  13, 14, 15, 16, 20, 21, 22, 23, 24, 25, 26,
                          27, 29, 32, 33, 34, 36, 37, 39, 40, 41, 42, 43,
                          44, 45, 46, 47, 48, 49, 5,  51, 52, 6,  8,  9};
  const char *images[36];
  for (unsigned i = 0; i < sizeof(img_idx_list) / sizeof(img_idx_list[0]); i++) {
    // for (unsigned i = 0; i < 1; i++) {
      char path[200];
    sprintf(path, "/tmp/guest/%d.jpg", img_idx_list[i]);
    images[i] = strdup(path);
  }

  double tic = get_real_time();  // Real time
  uint64_t start = get_microseconds();
  printf("start time %ld\n", start);
  int ret = decode_images_multithreaded(images, sizeof(images) / sizeof(images[0]), num_decoders);
  if (ret != 0) {
    printf("Decoding failed\n");
  } else {
    printf("Decoding succeeded\n");
  }
  uint64_t end = get_microseconds();
  double toc = get_real_time();

  printf("end time %ld\n", end);
  printf("Total time: %lu us\n", end - start);
  printf("Real time: %f\n", toc - tic);

  return 0;
}