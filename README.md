# U-boot for ARTIK5 and ARTIK10
## Contents
1. [Introduction](#1-introduction)
2. [Build guide](#2-build-guide)
3. [Update guide](#3-update-guide)

## 1. Introduction
This 'u-boot-artik' repository is u-boot source for artik5(artik520) and
artik10(artik1020). The base version of artik u-boot is 2012-07 and based
on Samsung Exynos u-boot.

---
## 2. Build guide
### 2.1 Install cross compiler
```
sudo apt-get install gcc-arm-linux-gnueabihf
```
If you can't install the above toolchain, you can use linaro toolchain.
```
wget http://releases.linaro.org/components/toolchain/binaries/latest-5/arm-linux-gnueabihf/gcc-linaro-5.3-2016.02-x86_64_arm-linux-gnueabihf.tar.xz
tar xf gcc-linaro-5.3-2016.02-x86_64_arm-linux-gnueabihf.tar.xz
export PATH=~/gcc-linaro-5.3-2016.02-x86_64_arm-linux-gnueabihf/bin:$PATH
```
You can the path permernently through adding it into ~/.bashrc

### 2.2 Build the u-boot
+ For artik5>
```
make ARCH=arm artik5_config
```
If you want to change u-boot configurations, please change include/configs/artik5.h and include/configs/artik_common.h

```
make ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf-
```

+ For artik10>
```
make ARCH=arm artik10_config
```
If you want to change u-boot configurations, please change include/configs/artik10.h and include/configs/artik_common.h

```
make ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf-
```

### 2.3 Generate params.bin
The params.bin contains u-boot environment variables and you can generate the file using below commands:
```
cp `find . -name "env_common.o"` copy_env_common.o
arm-linux-gnueabihf-objcopy -O binary --only-section=.rodata `find . -name "copy_env_common.o"`
tr '\0' '\n' < copy_env_common.o | grep '=' > default_envs.txt
cp default_envs.txt default_envs.txt.orig
tools/mkenvimage -s 16384 -o params.bin default_envs.txt
```

---
## 3. Update Guide
You can update the u-boot through fastboot or micro sd card(ext4 partition)

### 3.1 Fastboot
For artik5, you should prepare mini-A(not microUSB) USB cable.
For artik10, you should prepare Type-A to Type-A USB cable.

Install android-tools-fastboot
```
sudo apt-get install android-tools-fastboot
wget -S -O - http://source.android.com/source/51-android.rules | sed "s/<username>/$USER/" | sudo tee >/dev/null /etc/udev/rules.d/51-android.rules; sudo udevadm control --reload-rules
```

Insert the USB cable(not MicroUSB Cable) into your board.

Enter u-boot shell mode during boot countdown:
```
Net:   No ethernet found.
Hit any key to stop autoboot:  0
ARTIK10 #
ARTIK10 #
ARTIK10 # fastboot
[Partition table on MoviNAND]
ptn 0 name='fwbl1' start=0x0 len=N/A (use hard-coded info. (cmd: movi))
ptn 1 name='bl2' start=N/A len=N/A (use hard-coded info. (cmd: movi))
ptn 2 name='bootloader' start=N/A len=N/A (use hard-coded info. (cmd: movi))
ptn 3 name='tzsw' start=N/A len=N/A (use hard-coded info. (cmd: movi))
ptn 4 name='env' start=N/A len=N/A (use hard-coded info. (cmd: movi))
ptn 5 name='kernel' start=N/A len=N/A
ptn 6 name='ramdisk' start=N/A len=0x1a00000(~26624KB)
ptn 7 name='dtb' start=N/A len=N/A
ptn 8 name='boot' start=0x100000 len=0x2000000(~32768KB)
ptn 9 name='modules' start=0x2100000 len=0x2000000(~32768KB)
ptn 10 name='rootfs' start=0x4100000 len=0x39fcfbe00(~15201263KB)
USB cable Connected![CH-0]
```

On your Host PC(Linux), flash the u-boot using below command
```
sudo fastboot flash bootloader u-boot.bin
sudo fastboot flash env params.bin
sudo fastboot reboot
```

### 3.2 microSD Card
Prepare a micro SD card and format to ext4 file system.
```
sudo mkfs.ext4 /dev/sd[X]1
sudo mount /dev/sd[X]1 /mnt
```
Copy compiled binaries(u-boot.bin, params.bin) into a micro sd card.
```
sudo cp u-boot.bin params.bin /mnt
sudo umount /mnt
```

Insert the microSD card into your board and enter u-boot shell during boot countdown
```
sdfuse flashall 1
```

Please ignore below warnings
```
File not found boot.img
File not found zImage
File not found uInitrd
File not found exynos5422-artik10.dtb
File not found modules.img
File not found rootfs.img
File not found data.img
```
