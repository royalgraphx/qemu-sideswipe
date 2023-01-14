# qemu-sideswipe
Modified QEMU for Android VM

Current Selected Branch: main

# Building:

# Install required dependencies

## Debian Based

```
sudo apt-get update

sudo apt-get install -y git libglib2.0-dev libfdt-dev libpixman-1-dev zlib1g-dev ninja-build git-email libaio-dev libbluetooth-dev libcapstone-dev libbrlapi-dev libbz2-dev libcap-ng-dev libcurl4-gnutls-dev libgtk-3-dev libibverbs-dev libjpeg8-dev libncurses5-dev libnuma-dev librbd-dev librdmacm-dev libsasl2-dev libsdl2-dev libseccomp-dev libsnappy-dev libssh-dev libvde-dev libvdeplug-dev libvte-2.91-dev libxen-dev liblzo2-dev valgrind xfslibs-dev libnfs-dev libiscsi-dev binwalk p7zip-full qemu-utils adb libusb-1.0-0-dev build-essential libepoxy-dev libdrm-dev libgbm-dev libx11-dev libvirglrenderer-dev libpulse-dev libsdl2-dev libgtk-3-dev
```

## Arch/pacman based

```
sudo pacman -Syu

sudo pacman -S git glib2 dtc pixman zlib ninja libaio bluez-libs capstone brltty bzip2 libcap-ng libcurl-gnutls gtk3 libjpeg-turbo ncurses numactl libsasl sdl2 libseccomp snappy libssh libusb vte3 lzo valgrind libnfs libiscsi binwalk p7zip qemu-tools android-tools base-devel libepoxy libdrm mesa libx11 virglrenderer libpulse qemu-desktop
```

Add user to kvm group
```
sudo adduser $USER kvm
```

Download Repository
```
git clone --recursive https://github.com/royalgraphx/qemu-sideswipe.git
```

Build QEMU
```
cd qemu-sideswipe
mkdir build
cd build
../configure --enable-sdl --enable-opengl --enable-virglrenderer --enable-system --enable-modules --audio-drv-list=pa --target-list=x86_64-softmmu --enable-kvm --enable-gtk  --enable-libusb
make
```

Useful Links:
- https://github.com/royalgraphx/SideswipeOnQEMU/blob/main/preinstall.sh
- https://github.com/royalgraphx/SideswipeOnQEMU/blob/main/postinstall.sh
- https://github.com/royalgraphx/SideswipeOnQEMU/blob/main/launch_scripts/launch.sh


Current Launch Command:
```
./qemu-system-x86_64 -enable-kvm -cpu host -smp 4 -drive file=android.img,format=raw,cache=none,if=virtio -m 4G -usb -device usb-tablet,bus=usb-bus.0 -device usb-host,vendorid=0x054c,productid=0x0ce6 -display sdl,gl=on,show-cursor=on -device virtio-vga-gl,xres=1920,yres=1080 -net nic,model=virtio-net-pci -net user,hostfwd=tcp::5555-:5555 -machine vmport=off -device virtio-tablet-pci -device virtio-keyboard-pci -device AC97 -machine q35 -kernel $kernel -append "root=/dev/ram0 quiet GRALLOC=gbm video=1920x1080 SRC=/" -initrd $initrd.img
```
