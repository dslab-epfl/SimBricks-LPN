#include <iostream>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <cstring>
#include <cstdlib>
#include <chrono>  // For time measurement

// Memory settings
static void *alloc_base = nullptr;
static uint64_t alloc_phys_base = 1ULL * 1024 * 1024 * 1024;  // Start at 1GB
static size_t alloc_size = 512 * 1024 * 1024;  // 512MB reserved by memmap
static size_t alloc_off = 0;  // Offset for writing into the region

// Function to initialize the memory mapping
static void alloc_init()
{
    if (alloc_base)
        return;

    std::cerr << "Initializing allocator" << std::endl;

    // Open /dev/mem for accessing physical memory
    
    int fd = open("/dev/mem", O_RDWR | O_SYNC);
    // int fd = open("/dev/mem", O_RDWR );
    void *mem = mmap(NULL, alloc_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, alloc_phys_base);
    
    // // Map 512MB of memory starting at 1GB physical address
    // int fd = open("hello", O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
    // int fd = open("hello", O_RDWR | O_SYNC);
    
    if (fd < 0) {
        std::cerr << "Error opening /dev/mem" << std::endl;
        abort();
    }
     // Optionally resize the file to match the size to be mapped
    // if (ftruncate(fd, alloc_size) == -1) {
    //     std::cerr << "Error resizing the file" << std::endl;
    //     close(fd);
    //     abort();
    // }

    // // Map 512MB of the file into memory (note: no physical address, just file offset)
    // void *mem = mmap(NULL, alloc_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

    if (mem == MAP_FAILED) {
        std::cerr << "mmap failed" << std::endl;
        abort();
    }
    alloc_base = mem;

    std::cerr << "Allocator initialized" << std::endl;
}

// Function to write 200MB of data into the mapped memory and measure time
static void write_memory()
{
    if (!alloc_base) {
        std::cerr << "Allocator not initialized" << std::endl;
        return;
    }

    void* dest_mem = malloc(alloc_size);
    // Writing 200MB into the reserved memory region
    size_t write_size = 2 * 1024;  // 20KB
    // ratio to 200MB is 30M / 200K = 100*4ms =  
    std::cerr << "Writing " << write_size << " to reserved memory..." << std::endl;

    auto start_time = std::chrono::high_resolution_clock::now();  // Start time
    memcpy(alloc_base, dest_mem, write_size);
    // memcpy(dest_mem, alloc_base, write_size);

    auto end_time = std::chrono::high_resolution_clock::now();  // End time
    auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end_time - start_time).count();  // Time in microseconds

    std::cerr << "Writing completed in " << duration << " nanoseconds." << std::endl;
}

// Main function to initialize the memory and write data
int main()
{
    alloc_init();  // Initialize the memory mapping
    write_memory();  // Write 200MB to the memory region

    return 0;
}

