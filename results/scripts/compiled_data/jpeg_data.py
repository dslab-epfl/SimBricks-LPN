# JPEG extracted performance data

# Filename to label mapping
filename_to_label_map = {
    'jpeg_decoder_multithreaded_post-gem5_o3-lpn-1-1.json': 'JPEG-Gem5-1devices-multi-lpn',
    'jpeg_decoder-gem5_o3-rtl-1.json': 'JPEG-Gem5-single-rtl',
    'jpeg_decoder_multithreaded_post-gem5_o3-rtl-1-1.json': 'JPEG-Gem5-1devices-multi-rtl',
    'jpeg_decoder_multithreaded_post-gem5_o3-lpn-2-1.json': 'JPEG-Gem5-2devices-multi-lpn',
    'jpeg_decoder_multithreaded_post-gem5_o3-lpn-8-1.json': 'JPEG-Gem5-8devices-multi-lpn',
    'jpeg_decoder_multithreaded_post-gem5_o3-rtl-4-1.json': 'JPEG-Gem5-4devices-multi-rtl',
    'jpeg_decoder_multithreaded_post-gem5_o3-lpn-4-1.json': 'JPEG-Gem5-4devices-multi-lpn',
    'jpeg_decoder_multithreaded_post-gem5_o3-rtl-2-1.json': 'JPEG-Gem5-2devices-multi-rtl',
    'jpeg_decoder-gem5_o3-lpn-1.json': 'JPEG-Gem5-single-lpn',
    'jpeg_decoder_multithreaded_post-gem5_o3-rtl-8-1.json': 'JPEG-Gem5-8devices-multi-rtl',
}

# Combined data as dictionary
performance_data = {
    'JPEG-Gem5-1devices-multi-lpn': {
        'latency': 1956703000.0,
        'real_time': 10168.912014722824,
        'filename': 'jpeg_decoder_multithreaded_post-gem5_o3-lpn-1-1.json'
    },
    'JPEG-Gem5-single-rtl': {
        'latency': 1323170288,
        'real_time': 10546.482554197311,
        'filename': 'jpeg_decoder-gem5_o3-rtl-1.json'
    },
    'JPEG-Gem5-1devices-multi-rtl': {
        'latency': 1961702000.0,
        'real_time': 11733.990276098251,
        'filename': 'jpeg_decoder_multithreaded_post-gem5_o3-rtl-1-1.json'
    },
    'JPEG-Gem5-2devices-multi-lpn': {
        'latency': 1066838000.0,
        'real_time': 10694.511618852615,
        'filename': 'jpeg_decoder_multithreaded_post-gem5_o3-lpn-2-1.json'
    },
    'JPEG-Gem5-8devices-multi-lpn': {
        'latency': 554915000.0,
        'real_time': 11717.623435735703,
        'filename': 'jpeg_decoder_multithreaded_post-gem5_o3-lpn-8-1.json'
    },
    'JPEG-Gem5-4devices-multi-rtl': {
        'latency': 713892000.0,
        'real_time': 11263.763670921326,
        'filename': 'jpeg_decoder_multithreaded_post-gem5_o3-rtl-4-1.json'
    },
    'JPEG-Gem5-4devices-multi-lpn': {
        'latency': 719891000.0,
        'real_time': 11367.581559181213,
        'filename': 'jpeg_decoder_multithreaded_post-gem5_o3-lpn-4-1.json'
    },
    'JPEG-Gem5-2devices-multi-rtl': {
        'latency': 1056839000.0,
        'real_time': 10984.035303354263,
        'filename': 'jpeg_decoder_multithreaded_post-gem5_o3-rtl-2-1.json'
    },
    'JPEG-Gem5-single-lpn': {
        'latency': 1324512253,
        'real_time': 10478.445447206497,
        'filename': 'jpeg_decoder-gem5_o3-lpn-1.json'
    },
    'JPEG-Gem5-8devices-multi-rtl': {
        'latency': 551916000.0,
        'real_time': 12065.184331417084,
        'filename': 'jpeg_decoder_multithreaded_post-gem5_o3-rtl-8-1.json'
    },
}
