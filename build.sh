#!/bin/bash
sh ../configure --enable-sdl --enable-opengl --enable-virglrenderer --enable-system --enable-modules --audio-drv-list=pa --target-list=x86_64-softmmu --enable-kvm --enable-gtk  --enable-libusb
make
cd ..
zip -r qemu-sideswipe.zip build
