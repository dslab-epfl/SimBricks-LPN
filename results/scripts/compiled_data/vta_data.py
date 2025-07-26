# VTA extracted performance data

# Filename to label mapping
filename_to_label_map = {
    'classify-resnet18_v1-vta-gem5_o3-rtl-4-2000-1.json': 'VTA-resnet18_v1-rtl',
    'classify_multi-resnet18_v1-vta-go3-lpn-8-1.json': 'VTA-8devices-resnet18_v1-lpn',
    'classify-resnet34_v1-vta-gem5_o3-rtl-4-2000-1.json': 'VTA-resnet34_v1-rtl',
    'classify_multi-resnet18_v1-vta-go3-rtl-8-1.json': 'VTA-8devices-resnet18_v1-rtl',
    'classify-resnet18_v1-vta-gem5_o3-lpn-4-2000-1.json': 'VTA-resnet18_v1-lpn',
    'classify-resnet50_v1-vta-gem5_o3-lpn-4-2000-1.json': 'VTA-resnet50_v1-lpn',
    'classify_multi-resnet18_v1-vta-go3-lpn-4-1.json': 'VTA-4devices-resnet18_v1-lpn',
    'classify-resnet34_v1-vta-gem5_o3-lpn-4-2000-1.json': 'VTA-resnet34_v1-lpn',
    'classify-resnet50_v1-vta-gem5_o3-rtl-4-2000-1.json': 'VTA-resnet50_v1-rtl',
    'classify_multi-resnet18_v1-vta-go3-rtl-4-1.json': 'VTA-4devices-resnet18_v1-rtl',
}

# Combined data as dictionary
performance_data = {
    'VTA-resnet18_v1-rtl': {
        'latency': 46992856,
        'real_time': 1166.2677212556202,
        'filename': 'classify-resnet18_v1-vta-gem5_o3-rtl-4-2000-1.json'
    },
    'VTA-8devices-resnet18_v1-lpn': {
        'latency': 150977048.0,
        'real_time': 29177.564428567886,
        'filename': 'classify_multi-resnet18_v1-vta-go3-lpn-8-1.json'
    },
    'VTA-resnet34_v1-rtl': {
        'latency': 66989816,
        'real_time': 1293.3421546618144,
        'filename': 'classify-resnet34_v1-vta-gem5_o3-rtl-4-2000-1.json'
    },
    'VTA-8devices-resnet18_v1-rtl': {
        'latency': 151976896.0,
        'real_time': 30388.91276884079,
        'filename': 'classify_multi-resnet18_v1-vta-go3-rtl-8-1.json'
    },
    'VTA-resnet18_v1-lpn': {
        'latency': 47992704,
        'real_time': 1087.5437366962433,
        'filename': 'classify-resnet18_v1-vta-gem5_o3-lpn-4-2000-1.json'
    },
    'VTA-resnet50_v1-lpn': {
        'latency': 744886760,
        'real_time': 4498.452273448308,
        'filename': 'classify-resnet50_v1-vta-gem5_o3-lpn-4-2000-1.json'
    },
    'VTA-4devices-resnet18_v1-lpn': {
        'latency': 240963368.0,
        'real_time': 23485.412983179092,
        'filename': 'classify_multi-resnet18_v1-vta-go3-lpn-4-1.json'
    },
    'VTA-resnet34_v1-lpn': {
        'latency': 69989360,
        'real_time': 1156.619638999303,
        'filename': 'classify-resnet34_v1-vta-gem5_o3-lpn-4-2000-1.json'
    },
    'VTA-resnet50_v1-rtl': {
        'latency': 711891776,
        'real_time': 6703.545670350392,
        'filename': 'classify-resnet50_v1-vta-gem5_o3-rtl-4-2000-1.json'
    },
    'VTA-4devices-resnet18_v1-rtl': {
        'latency': 248962152.0,
        'real_time': 23386.223619699478,
        'filename': 'classify_multi-resnet18_v1-vta-go3-rtl-4-1.json'
    },
}
