#!/usr/bin/env python3

import os
import json
import glob
from pathlib import Path

def extract_data_from_json(filepath):
    """
    Extract JPEG performance data from a classification JSON file.
    Returns tuple of (total_inference_duration, warmup_duration, pure_inference_duration, time_taken, real_time).
    """
    try:
        with open(filepath, 'r') as f:
            data = json.load(f)
        
        total_inference_duration = None
        warmup_duration = None
        pure_inference_duration = None
        time_taken = None
        real_time = None
        
        # Extract start_time and end_time to calculate real_time
        start_time = data.get('start_time')
        end_time = data.get('end_time')
        if start_time is not None and end_time is not None:
            real_time = end_time - start_time
        
        # Convert the entire JSON to string to search for patterns
        json_str = json.dumps(data)
        
        import re
        
        duration_per_img = re.findall(r'duration: ([\d.]+) ns', json_str)
        total_duration = 0
        if duration_per_img:
            for ele in duration_per_img:
                total_duration += int(ele)
        
        total_time = re.findall(r'Total time: ([\d.]+) us', json_str)
        if total_time:
            # to ns
            total_duration = float(total_time[-1])*1000.0

        return total_duration, real_time

    except (json.JSONDecodeError, FileNotFoundError) as e:
        print(f"Error reading {filepath}: {e}")
        return None, None, None, None, None

def filename_to_label(filename):
    """
    Convert JPEG classification filename to meaningful label.
    """
    base_name = "JPEG-Gem5-"
    model_name = "single"


    acc_name = ""
    if "rtl" in filename:
        acc_name = "rtl"
    elif "lpn" in filename:
        acc_name = "dsim"

    if "multithreaded" in filename:
        model_name = "multi"
        # grep the number at the end 
        # grep 4 here for example: classify_multi-resnet18_v1-JPEG-go3-lpn-4-1.json
        base_name += filename.split('-')[-2] + "devices" + '-'

    base_name += model_name + '-' + acc_name
    return base_name

def main():
    script_dir = Path(__file__).parent
    
    # Look for JPEG classification results
    search_dirs = [
        script_dir/'..',
    ]
    
    all_results = []
    filename_to_label_map = {}  # Store filename to label mapping
    
    for search_dir in search_dirs:
        if not search_dir.exists():
            continue
            
        print(f"Searching in {search_dir}")
        # Find all *jpeg*-1.json files
        json_files = list(search_dir.glob('*jpeg*-1.json'))
        
        if json_files:
            print(f"Found {len(json_files)} JPEG JSON files in {search_dir}")
        
        for json_file in json_files:
            print(f"Processing {json_file.name}...")
            
            try:
                time_taken, real_time = extract_data_from_json(json_file)
                
                label = filename_to_label(json_file.name)
                
                # Store filename to label mapping
                filename_to_label_map[json_file.name] = label
                
                result = {
                    'filename': json_file.name,
                    'label': label,
                    'latency': time_taken,
                    'real_time': real_time,
                    'source_dir': str(search_dir)
                }
                
                all_results.append(result)
                
                print(f"  Label: {label}")
                print(f"  Time taken: {time_taken}ns" if time_taken else "  Time taken: None")
                print(f"  Real time: {real_time:.2f}s" if real_time else "  Real time: None")
                
            except Exception as e:
                print(f"  Error processing {json_file.name}: {e}")
    
    if not all_results:
        print("No classify_*-1.json files found in any search directory")
        return
    

    # Also create a Python data file for easy import
    data_file = script_dir / 'compiled_data/jpeg_data.py'
    with open(data_file, 'w') as f:
        f.write("# JPEG extracted performance data\n\n")
        
        f.write("# Filename to label mapping\n")
        f.write("filename_to_label_map = {\n")
        for filename, label in filename_to_label_map.items():
            f.write(f"    '{filename}': '{label}',\n")
        f.write("}\n\n")
        
        f.write("# Combined data as dictionary\n")
        f.write("performance_data = {\n")
        for result in all_results:
            f.write(f"    '{result['label']}': {{\n")
            f.write(f"        'latency': {result['latency']},\n")
            f.write(f"        'real_time': {result['real_time']},\n")
            f.write(f"        'filename': '{result['filename']}'\n")
            f.write(f"    }},\n")
        f.write("}\n")
    
    print(f"Python data file: {data_file}")
    print(f"Processed {len(all_results)} files total")
    

if __name__ == "__main__":
    main()
