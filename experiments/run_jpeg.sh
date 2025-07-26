
make -C .. sims/misc/jpeg_decoder/jpeg_decoder_verilator 

tmux new-session -d -s "jpeg_rtl" "python3 run.py --verbose --force --filter='jpeg_decoder-gem5_o3-rtl' --force pyexps/jpeg_decoder.py --repo $HOME/SimBricks-LPN/" 
tmux new-session -d -s "jpeg_lpn" "python3 run.py --verbose --force --filter='jpeg_decoder-gem5_o3-lpn' --force pyexps/jpeg_decoder.py --repo $HOME/SimBricks-LPN/"  

# tmux new-session -d -s "jpeg_lpn_multithreaded_1" "python3 run.py --verbose --force --filter='jpeg_decoder_multithreaded_post-gem5_o3-lpn-1' --force pyexps/jpeg_decoder_multithreaded.py --repo $HOME/SimBricks-LPN/"  
# tmux new-session -d -s "jpeg_lpn_multithreaded_2" "python3 run.py --verbose --force --filter='jpeg_decoder_multithreaded_post-gem5_o3-lpn-2' --force pyexps/jpeg_decoder_multithreaded.py --repo $HOME/SimBricks-LPN/"  
# tmux new-session -d -s "jpeg_lpn_multithreaded_4" "python3 run.py --verbose --force --filter='jpeg_decoder_multithreaded_post-gem5_o3-lpn-4' --force pyexps/jpeg_decoder_multithreaded.py --repo $HOME/SimBricks-LPN/"  
# tmux new-session -d -s "jpeg_lpn_multithreaded_8" "python3 run.py --verbose --force --filter='jpeg_decoder_multithreaded_post-gem5_o3-lpn-8' --force pyexps/jpeg_decoder_multithreaded.py --repo $HOME/SimBricks-LPN/"  

# tmux new-session -d -s "jpeg_rtl_multithreaded_1" "python3 run.py --verbose --force --filter='jpeg_decoder_multithreaded_post-gem5_o3-rtl-1' --force pyexps/jpeg_decoder_multithreaded.py --repo $HOME/SimBricks-LPN/"
# tmux new-session -d -s "jpeg_rtl_multithreaded_2" "python3 run.py --verbose --force --filter='jpeg_decoder_multithreaded_post-gem5_o3-rtl-2' --force pyexps/jpeg_decoder_multithreaded.py --repo $HOME/SimBricks-LPN/"
# tmux new-session -d -s "jpeg_rtl_multithreaded_4" "python3 run.py --verbose --force --filter='jpeg_decoder_multithreaded_post-gem5_o3-rtl-4' --force pyexps/jpeg_decoder_multithreaded.py --repo $HOME/SimBricks-LPN/"
# tmux new-session -d -s "jpeg_rtl_multithreaded_8" "python3 run.py --verbose --force --filter='jpeg_decoder_multithreaded_post-gem5_o3-rtl-8' --force pyexps/jpeg_decoder_multithreaded.py --repo $HOME/SimBricks-LPN/"
