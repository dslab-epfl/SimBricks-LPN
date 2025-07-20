# make -C .. clean
# make -C .. -j
user_home="$HOME"
# python3 run.py --verbose --force --filter="classify_multi-resnet18_v1-vta-go3-lpn-1" --force pyexps/classify_multi.py --repo $(user_home)/SimBricks-LPN/ &
python3 run.py --verbose --force --filter="classify_multi_exit-resnet18_v1-vta-go3-lpn-8" --force pyexps/classify_multi.py --repo $(user_home)/SimBricks-LPN/ &
python3 run.py --verbose --force --filter="classify_multi_exit-resnet18_v1-vta-go3-rtl-8" --force pyexps/classify_multi.py --repo $(user_home)/SimBricks-LPN/ &
# python3 run.py --verbose --force --filter="classify_multi-resnet18_v1-vta-go3-lpn-8" --force pyexps/classify_multi.py --repo $(user_home)/SimBricks-LPN/ &

# make -C $(user_home)/SimBricks-LPN/sims/external/vta/simbricks 

# python3 run.py --verbose --force --filter="detect_dec-vta-gem5_o3-lpn-2000" --force pyexps/detect_simple.py --repo $(user_home)/SimBricks-LPN/ & 
# python3 run.py --verbose --force --filter="vtatest_dec-gt-lpn" --force pyexps/vtatest.py --repo $(user_home)/SimBricks-LPN/ &

# python3 run.py --verbose --force --filter="detect_dec-vta-gem5_o3-rtl-2000" --force pyexps/detect_simple.py --repo $(user_home)/SimBricks-LPN/ & 
# python3 run.py --verbose --force --filter="classify_dec-resnet18_v1-vta-gem5_o3-rtl-4-2000" --force pyexps/classify_simple.py --repo $(user_home)/SimBricks-LPN/ & 
# python3 run.py --verbose --force --filter="classify_dec-resnet34_v1-vta-gem5_o3-rtl-4-2000" --force pyexps/classify_simple.py --repo $(user_home)/SimBricks-LPN/ & 
# python3 run.py --verbose --force --filter="classify_dec-resnet50_v1-vta-gem5_o3-rtl-4-2000" --force pyexps/classify_simple.py --repo $(user_home)/SimBricks-LPN/ &
# python3 run.py --verbose --force --filter="vtatest_dec-gt-rtl" --force pyexps/vtatest.py --repo $(user_home)/SimBricks-LPN/ &
