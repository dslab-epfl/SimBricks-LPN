#!/bin/bash

tmux new-session -d -s "classify_lpn_8" "cd $HOME/SimBricks-LPN/experiments && python3 run.py --verbose --force --filter='classify_multi-resnet18_v1-vta-go3-lpn-8' --force pyexps/classify_multi.py --repo $HOME/SimBricks-LPN/"

tmux new-session -d -s "classify_rtl_8" "cd $HOME/SimBricks-LPN/experiments && python3 run.py --verbose --force --filter='classify_multi-resnet18_v1-vta-go3-rtl-8' --force pyexps/classify_multi.py --repo $HOME/SimBricks-LPN/"

tmux new-session -d -s "classify_lpn_4" "cd $HOME/SimBricks-LPN/experiments && python3 run.py --verbose --force --filter='classify_multi-resnet18_v1-vta-go3-lpn-4' --force pyexps/classify_multi.py --repo $HOME/SimBricks-LPN/"

tmux new-session -d -s "classify_rtl_4" "cd $HOME/SimBricks-LPN/experiments && python3 run.py --verbose --force --filter='classify_multi-resnet18_v1-vta-go3-rtl-4' --force pyexps/classify_multi.py --repo $HOME/SimBricks-LPN/"

