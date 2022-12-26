# qemu-sideswipe
Modified QEMU for Android VM

Current Selected Branch: wx86_64

Building:

Requirements
- MSYS2 MINGW64
- Patience

Useful Links:
- https://packages.msys2.org/queue
- https://qemu.weilnetz.de/doc/BUILD.txt
- https://github.com/royalgraphx/SideswipeOnQEMU/blob/main/postinstall.sh
- https://wiki.qemu.org/Hosts/W32#Native_builds_with_MSYS2

Current Launch Command:
```
./qemu-system-x86_64.exe -accel whpx,kernel-irqchip=off -machine q35 -bios pc-bios/edk2-x86_64.fd --boot c -m 12G -display sdl,gl=on,show-cursor=on -device virtio-vga-gl,xres=1920,yres=1080 -smp 14 -drive file=bliss-15.img,if=virtio -net nic,model=virtio-net-pci -net user,hostfwd=tcp::5555-:5555 -cpu kvm64,ssse3=on,sse4_1=on,sse4_2=on,aes=on
```

Build Commands:

Install https://www.msys2.org/ , and run the following in mingw64

```
pacman -Syu
*it'll restart mingw64*
pacman -Syu
*yes again, make sure its fully up to date*


pacman -S make mingw-w64-x86_64-diffutils \
mingw-w64-x86_64-gcc mingw-w64-x86_64-glib2 \
mingw-w64-x86_64-libslirp mingw-w64-x86_64-make \
mingw-w64-x86_64-pixman mingw-w64-x86_64-pkgconf \
mingw-w64-x86_64-SDL2 mingw-w64-x86_64-virglrenderer \
mingw-w64-x86_64-lzo2 \
ninja patch python


git clone --recursive https://github.com/royalgraphx/qemu-sideswipe.git --branch wx86_64


cd qemu-sideswipe
mkdir build
cd build


../configure \
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
--enable-slirp

make -j12

cat pc-bios/edk2-i386-vars.fd \
pc-bios/edk2-x86_64-code.fd > pc-bios/edk2-x86_64.fd
```

TO-DO's:
custom android lol
