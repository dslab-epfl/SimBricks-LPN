user_home="$HOME"

# make -C .. sims/misc/jpeg_decoder/jpeg_decoder_verilator 
# python3 run.py --verbose --force --filter="jpeg_decoder-gem5_o3-rtl" --force pyexps/jpeg_decoder.py --repo $(user_home)/SimBricks-LPN/ & 
# python3 run.py --verbose --force --filter="jpeg_decoder-gem5_o3-lpn" --force pyexps/jpeg_decoder.py --repo $(user_home)/SimBricks-LPN/ &

python3 run.py --verbose --force --filter="jpeg_decoder_multithreaded_post-gem5_o3-rtl-1" --force pyexps/jpeg_decoder_multithreaded.py --repo $(user_home)/SimBricks-LPN/ & 
python3 run.py --verbose --force --filter="jpeg_decoder_multithreaded_post-gem5_o3-rtl-2" --force pyexps/jpeg_decoder_multithreaded.py --repo $(user_home)/SimBricks-LPN/ & 
python3 run.py --verbose --force --filter="jpeg_decoder_multithreaded_post-gem5_o3-rtl-4" --force pyexps/jpeg_decoder_multithreaded.py --repo $(user_home)/SimBricks-LPN/ & 
python3 run.py --verbose --force --filter="jpeg_decoder_multithreaded_post-gem5_o3-rtl-8" --force pyexps/jpeg_decoder_multithreaded.py --repo $(user_home)/SimBricks-LPN/ & 

wait 

python3 run.py --verbose --force --filter="jpeg_decoder_multithreaded_post-gem5_o3-lpn-1" --force pyexps/jpeg_decoder_multithreaded.py --repo $(user_home)/SimBricks-LPN/ &
python3 run.py --verbose --force --filter="jpeg_decoder_multithreaded_post-gem5_o3-lpn-2" --force pyexps/jpeg_decoder_multithreaded.py --repo $(user_home)/SimBricks-LPN/ &
python3 run.py --verbose --force --filter="jpeg_decoder_multithreaded_post-gem5_o3-lpn-4" --force pyexps/jpeg_decoder_multithreaded.py --repo $(user_home)/SimBricks-LPN/ & 
python3 run.py --verbose --force --filter="jpeg_decoder_multithreaded_post-gem5_o3-lpn-8" --force pyexps/jpeg_decoder_multithreaded.py --repo $(user_home)/SimBricks-LPN/ &
