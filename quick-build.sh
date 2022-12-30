#!/bin/bash

sh ../configure \
--target-list=x86_64-softmmu \
--enable-strip \
--enable-whpx \
--enable-sdl \
--disable-gtk \
--enable-virglrenderer \
--enable-tools \
--enable-lzo \
--enable-bzip2 \
--enable-vdi \
--enable-qcow1 \
--disable-curses \
--disable-vnc \
--disable-capstone \
--enable-slirp && make

cd ..
zip -r qemu-sideswipe-w64.zip build
