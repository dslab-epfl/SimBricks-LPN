make -C .. clean
make -C .. -j

user_home="$HOME"
make -C $(user_home)/SimBricks-LPN/sims/misc/protoacc/simbricks

# python3 run.py --verbose --force --filter="protoacc_benchmark-gem5_o3-bench1" --force pyexps/test_protoacc.py --repo $(user_home)/SimBricks-LPN/
python3 run.py --verbose --force --filter="protoacc_benchmark_test_mem-gem5_o3-bench1" --force pyexps/test_protoacc_mem.py --repo $(user_home)/SimBricks-LPN/
