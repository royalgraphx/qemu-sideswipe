# qemu-sideswipe
Modified QEMU for Android VM

Current Selected Branch: wx86_64

Building:

Requirements
- MSYS2 MINGW64
- Lots of dependencies
- Patience

Useful Links:
- https://packages.msys2.org/queue
- https://qemu.weilnetz.de/doc/BUILD.txt
- https://github.com/royalgraphx/SideswipeOnQEMU/blob/main/postinstall.sh
- https://wiki.qemu.org/Hosts/W32#Native_builds_with_MSYS2

Current Launch Command:
```
./qemu-system-x86_64.exe --cdrom ubuntu-22.04.1-desktop-amd64.iso -accel whpx,kernel-irqchip=off -m 25G -device virtio-vga -display sdl -device qemu-xhci,id=xhci0 -device usb-tablet,bus=xhci0.0 -machine vmport=off
```

Build Commands:
```
mkdir -p bin/ndebug/x86_64-w64-mingw32
cd bin/ndebug/x86_64-w64-mingw32
../../../configure --disable-guest-agent-msi --disable-werror --enable-sdl --enable-gtk --enable-whpx --enable-libusb --enable-virglrenderer --target-list=x86_64-softmmu
make
```

TO-DO's:
Get Epoxy OpenGL support, so I can do --enable-opengl and -display sdl,gl=on

There's honestly so many dependencies scattered around so rebuilding this is basically extremely difficult, as long as I can get it built though, I'll distribute precompiled executables to include in SideswipeOnQEMU-W64
