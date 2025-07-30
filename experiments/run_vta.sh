#!/bin/bash

tmux new-session -d -s "classify_resnet18_lpn" "cd $HOME/SimBricks-LPN/experiments && python3 run.py --verbose --force --filter='classify-resnet18_v1-vta-gem5_o3-lpn-4-2000' --force pyexps/classify_simple.py --repo $HOME/SimBricks-LPN/"
tmux new-session -d -s "classify_resnet18_rtl" "cd $HOME/SimBricks-LPN/experiments && python3 run.py --verbose --force --filter='classify-resnet18_v1-vta-gem5_o3-rtl-4-2000' --force pyexps/classify_simple.py --repo $HOME/SimBricks-LPN/"

tmux new-session -d -s "classify_resnet34_lpn" "cd $HOME/SimBricks-LPN/experiments && python3 run.py --verbose --force --filter='classify-resnet34_v1-vta-gem5_o3-lpn-4-2000' --force pyexps/classify_simple.py --repo $HOME/SimBricks-LPN/"
tmux new-session -d -s "classify_resnet34_rtl" "cd $HOME/SimBricks-LPN/experiments && python3 run.py --verbose --force --filter='classify-resnet34_v1-vta-gem5_o3-rtl-4-2000' --force pyexps/classify_simple.py --repo $HOME/SimBricks-LPN/"

tmux new-session -d -s "classify_resnet50_lpn" "cd $HOME/SimBricks-LPN/experiments && python3 run.py --verbose --force --filter='classify-resnet50_v1-vta-gem5_o3-lpn-4-2000' --force pyexps/classify_simple.py --repo $HOME/SimBricks-LPN/"
tmux new-session -d -s "classify_resnet50_rtl" "cd $HOME/SimBricks-LPN/experiments && python3 run.py --verbose --force --filter='classify-resnet50_v1-vta-gem5_o3-rtl-4-2000' --force pyexps/classify_simple.py --repo $HOME/SimBricks-LPN/"

tmux new-session -d -s "classify_lpn_8" "cd $HOME/SimBricks-LPN/experiments && python3 run.py --verbose --force --filter='classify_multi-resnet18_v1-vta-go3-lpn-8' --force pyexps/classify_multi.py --repo $HOME/SimBricks-LPN/"

tmux new-session -d -s "classify_rtl_8" "cd $HOME/SimBricks-LPN/experiments && python3 run.py --verbose --force --filter='classify_multi-resnet18_v1-vta-go3-rtl-8' --force pyexps/classify_multi.py --repo $HOME/SimBricks-LPN/"

tmux new-session -d -s "classify_lpn_4" "cd $HOME/SimBricks-LPN/experiments && python3 run.py --verbose --force --filter='classify_multi-resnet18_v1-vta-go3-lpn-4' --force pyexps/classify_multi.py --repo $HOME/SimBricks-LPN/"

tmux new-session -d -s "classify_rtl_4" "cd $HOME/SimBricks-LPN/experiments && python3 run.py --verbose --force --filter='classify_multi-resnet18_v1-vta-go3-rtl-4' --force pyexps/classify_multi.py --repo $HOME/SimBricks-LPN/"

tmux new-session -d -s "detect_lpn" "cd $HOME/SimBricks-LPN/experiments && python3 run.py --verbose --force --filter='detect-vta-gem5_o3-lpn-2000' --force pyexps/detect_simple.py --repo $HOME/SimBricks-LPN/"
tmux new-session -d -s "detect_rtl" "cd $HOME/SimBricks-LPN/experiments && python3 run.py --verbose --force --filter='detect-vta-gem5_o3-rtl-2000' --force pyexps/detect_simple.py --repo $HOME/SimBricks-LPN/"

tmux new-session -d -s "vtatest-gt-rtl" "cd $HOME/SimBricks-LPN/experiments && python3 run.py --verbose --force --filter='vtatest-gt-rtl' --force pyexps/vtatest.py --repo $HOME/SimBricks-LPN/"
tmux new-session -d -s "vtatest-gt-lpn" "cd $HOME/SimBricks-LPN/experiments && python3 run.py --verbose --force --filter='vtatest-gt-lpn' --force pyexps/vtatest.py --repo $HOME/SimBricks-LPN/"