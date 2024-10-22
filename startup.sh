#!/bin/bash
sudo mount -t vboxsf xinu_windows_shared shared
cd ./shared/xinu/xinu-x86-vm/compile
make clean;make;./upload.sh