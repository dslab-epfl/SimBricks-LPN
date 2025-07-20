make -C .. clean
make -C .. -j

# make -C /home/jiacma/simbricks-lpn/sims/misc/protoacc/simbricks

# python3 run.py --verbose --force --filter="protoacc_benchmark-gem5_o3-bench1" --force pyexps/test_protoacc.py --repo /home/jiacma/simbricks-lpn/

python3 run.py --verbose --force --filter="protoacc_benchmark_test_mem-gem5_o3-bench1" --force pyexps/test_protoacc_mem.py --repo /home/jiacma/simbricks-lpn/
