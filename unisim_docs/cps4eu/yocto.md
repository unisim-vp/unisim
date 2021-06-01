# General information

Current experiments are entirely based on gatesgarth.
Note: even when configured for cortex-a57, gatesgarth seems to produce a lot of ARMv8.1 (bad system-register-based cpu identification ?)

    EXTRA_IMAGE_FEATURES ?= "dbg-pkgs dev-pkgs tools-sdk tools-debug tools-profile debug-tweaks"
    IMAGE_INSTALL_append = " python3 python3-pip"
    IMAGE_ROOTFS_EXTRA_SPACE = "524288"
    SDKIMAGE_FEATURES += "staticdev-pkgs"

# Good to know

To obtain a toolchain a standalone [YOCTO toolchain](https://www.blaess.fr/christophe/yocto-lab/sequence-III-2/index.html):

    bitbake core-minimal-image -c populate_sdk

## Interesting paths

### utils

tmp/work/aarch64-poky-linux/libmpc/1.1.0-r0/recipe-sysroot-native/usr/bin/aarch64-poky-linux/aarch64-poky-linux-objdump

### Linux

tmp/work/qemuarm64-poky-linux/linux-yocto/5.4.49+gitAUTOINC+30814016b3_ec485bd4af-r0/image/boot/vmlinux-5.4.49-yocto-standard

## contacts:

Richard Purdie helped with yocto [bug 14374](https://bugzilla.yoctoproject.org/show_bug.cgi?id=14374)

