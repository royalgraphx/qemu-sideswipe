#!/bin/bash
sh ../configure --enable-sdl --enable-opengl --enable-virglrenderer --enable-system --target-list=x86_64-softmmu --enable-whpx --enable-gtk --enable-libusb
make
cd ..
zip -r qemu-sideswipe-w64.zip build-msys
