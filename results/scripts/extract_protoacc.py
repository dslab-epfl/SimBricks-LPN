#!/usr/bin/env python3

import os
import json
import glob
from pathlib import Path

def extract_data_from_json(filepath):
    """
    Extract PERF 0 and PERF 1 data from a protoacc JSON file.
    Returns tuple of (perf0_number, perf1_number, real_time).
    """
    try:
        with open(filepath, 'r') as f:
            data = json.load(f)
        
        perf0_number = None
        perf1_number = None
        real_time = None
        
        # Extract start_time and end_time to calculate real_time
        start_time = data.get('start_time')
        end_time = data.get('end_time')
        if start_time is not None and end_time is not None:
            real_time = end_time - start_time
        
        # Convert the entire JSON to string to search for patterns
        json_str = json.dumps(data)
        
        import re
        
        # Extract PERF 0 ns new <num> from the raw string - find all and take the last one
        perf0_matches = re.findall(r'PERF 0, ns new (\d+)', json_str)
        if perf0_matches:
            perf0_number = int(perf0_matches[-1])  # Take the last match
        
        # Extract PERF 1 <num> from the raw string - find all and take the last one
        perf1_matches = re.findall(r'PERF 1, ns new (\d+)', json_str)
        if perf1_matches:
            perf1_number = int(perf1_matches[-1])  # Take the last match
        
        assert len(perf0_matches) > 0, "No PERF 0 matches found"

        return perf0_number, perf1_number, real_time/len(perf0_matches)
        
    except (json.JSONDecodeError, FileNotFoundError) as e:
        print(f"Error reading {filepath}: {e}")
        return None, None, None

def filename_to_label(filename):
    """
    Convert protoacc filename to bench_name + rtl/lpn label.
    """
    # Remove file extension and protoacc_ prefix
    base_name = "Protoacc-Gem5-"

    acc_name = "rtl" if "rtl" in filename else "dsim"
    if "bench" in filename:
        # Extract the bench number
        bench_name = filename.split('-')[-2]
        base_name += f"{bench_name}-{acc_name}"
    
    return base_name

def main():
    script_dir = Path(__file__).parent
    
    # Look for protoacc results - prioritize local directory, then common locations
    search_dirs = [
        script_dir/'..',
    ]
    
    all_results = []
    filename_to_label_map = {}  # Store filename to label mapping
    
    for search_dir in search_dirs:
        if not search_dir.exists():
            continue
            
        print(f"Searching in {search_dir}")
        
        # Find all protoacc_*-1.json files
        json_files = list(search_dir.glob('protoacc_*-1.json'))
        
        if json_files:
            print(f"Found {len(json_files)} protoacc JSON files in {search_dir}")
        
        for json_file in json_files:
            print(f"Processing {json_file.name}...")
            
            try:
                perf0_number, perf1_number, real_time = extract_data_from_json(json_file)
                
                label = filename_to_label(json_file.name)
                
                # Store filename to label mapping
                filename_to_label_map[json_file.name] = label
                
                latency_data = perf0_number
                if "bench2" in label or "bench5" in label:
                    # gem5 is inaccurate for perf0 portion
                    latency_data = perf1_number
                result = {
                    'filename': json_file.name,
                    'label': label,
                    "latency": latency_data,
                    'real_time': real_time,
                    'source_dir': str(search_dir)
                }
                
                all_results.append(result)
            
            except Exception as e:
                print(f"  Error processing {json_file.name}: {e}")
    
    if not all_results:
        print("No protoacc_*-1.json files found in any search directory")
        return

    # Also create a Python data file for easy import
    data_file = script_dir / 'compiled_data/protoacc_data.py'
    with open(data_file, 'w') as f:
        f.write("# ProtoAcc extracted performance data\n\n")
        
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