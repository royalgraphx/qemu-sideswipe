# patches used by qemu-sideswipe

# QEMU_Virtio_Tablet

This patch fixes QEMU's Virtio Tablet to work in Android as a Touch display, allowing for mouse to touch inputs working natively in Android.
This is to be paired with QEMU_Virtio_Tablet.idc patch, found in SideswipeOnQEMU. Manual patching of Android idc file in your own install is also possible. automated scripts found in SidewipeOnQEMU.

Source/Credit: https://groups.google.com/g/android-x86/c/M0X3rggHs3k?pli=1
This fix was found years ago, but is now implemented automagically for our Android Emulatikon use-cases. Upstream fix would be welcomed, both in Android ROM's and QEMU's src (wishful thinking).


# 0001-Virgil3D-with-SDL2-OpenGL

This patch was created by kjliew and is used in the current builds of wx86_64 qemu-sideswipe to allow Windows Hosts to use VirGL and OpenGL for Sideswipe.
Any additional info for this patch and it's source, please refer to the below Source/Credits!

Source/Credit: https://github.com/kjliew/qemu-3dfx/tree/master/virgil3d

