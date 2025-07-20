# make -C .. clean
# make -C .. -j

#make -C /home/jiacma/simbricks-lpn/sims/external/vta/simbricks 

# make -C .. sims/misc/jpeg_decoder/jpeg_decoder_verilator
# sudo -E python3 run.py --verbose --filter="*-rtl" --force pyexps/jpeg_decoder_lpn.py --repo /home/jiacma/npc/simbricks-lpn/
# sudo -E python3 run.py --verbose --filter="*-rtl" --force pyexps/jpeg_decoder.py --repo /home/jiacma/npc/simbricks-lpn/
# make -C ..
# make -C ../sims/external/vta/simbricks/ clean
# make -C ../sims/external/vta/simbricks/

#python3 run.py --verbose --force --filter="vtatest-gt-rtl" --force pyexps/vtatest.py --repo /home/jiacma/npc/simbricks-lpn/
#python3 run.py --verbose --force --filter="vtatest-gt-lpn" --force pyexps/vtatest.py --repo /home/jiacma/npc/simbricks-lpn/



#python3 run.py --verbose --force --filter="detect_simple_lpn_cpu-vta-gem5_o3-2000" --force pyexps/detect_simple.py --repo /home/jiacma/simbricks-lpn/

python3 run.py --verbose --force --filter="detect_t-vta-qemu_icount-lpn-160" --force pyexps/detect_simple.py --repo /home/jiacma/simbricks-lpn/
# python3 run.py --verbose --force --filter="detect-vta-gem5_o3-rtl-200" --force pyexps/detect_simple.py --repo /home/jiacma/simbricks-lpn/

#python3 run.py --verbose --force --filter="detect_simple-vta-qk-2000" --force pyexps/detect_simple.py --repo /home/jiacma/simbricks-lpn/

#sudo -E python3 run.py --verbose --filter="*-gem5_timing-lpn" --force pyexps/jpeg_decoder.py --repo /home/jiacma/npc/simbricks-lpn/
# python run.py --verbose --force --filter="*-gem5_kvm-lpn" pyexps/jpeg_decoder.py
# python3 ../results/jpeg_decoder.py out/jpeg_decoder-rtl-1.json
