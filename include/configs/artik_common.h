/*
 * Copyright (C) 2015 Samsung Electronics
 *
 * Configuration settings for the SAMSUNG ARTIK10 (EXYNOS5422) board.
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */
#ifndef __ARTIK_COMMON_H
#define __ARTIK_COMMON_H

#define CONFIG_SAMSUNG			/* in a SAMSUNG core */
#define CONFIG_S5P			/* S5P Family */
#define CONFIG_ARCH_EXYNOS		/* which is in a Exynos Family */

#include <asm/arch/cpu.h>		/* get chip and board defs */

#define CONFIG_ARCH_CPU_INIT
#define CONFIG_DISPLAY_CPUINFO
#define CONFIG_DISPLAY_BOARDINFO

/* TRUSTZONE */
#define CONFIG_TRUSTZONE_ENABLE
#undef CONFIG_TZPC
#define CONFIG_SMC_CMD

/* Configuration of bl1 partition size */
#define CONFIG_BL_MONITOR

/* Keep L2 Cache Disabled */
#define CONFIG_SYS_DCACHE_OFF

#define CONFIG_SYS_SDRAM_BASE		0x40000000
#define CONFIG_SYS_TEXT_BASE		0x43E00000

#define CONFIG_SYS_CLK_FREQ		24000000

/*   RAMDUMP MODE */
#define CONFIG_RAMDUMP_MODE		0xD

#define CONFIG_SETUP_MEMORY_TAGS
#define CONFIG_CMDLINE_TAG
#define CONFIG_INITRD_TAG
#define CONFIG_CMDLINE_EDITING

/* Power Down Modes */
#define S5P_CHECK_SLEEP			0x00000BAD
#define S5P_CHECK_DIDLE			0xBAD00000
#define S5P_CHECK_LPA			0xABAD0000

/* Offset for OM status registers */
#define OM_STATUS_OFFSET                0x0

/* Offset for inform registers */
#define INFORM0_OFFSET			0x800
#define INFORM1_OFFSET			0x804
#define INFORM2_OFFSET			0x808
#define INFORM3_OFFSET			0x80C
#define INFORM4_OFFSET			0x810

/* Offset for pmu reset status */
#define RST_STAT_OFFSET			0x404

/* select serial console configuration */
#define CONFIG_SERIAL_MULTI
#define CONFIG_BAUDRATE			115200

#define TZPC_BASE_OFFSET		0x10000

/* SD/MMC configuration */
#define CONFIG_GENERIC_MMC
#define CONFIG_MMC
#define CONFIG_SDHCI
#define CONFIG_S5P_MSHC
#define CONFIG_S5P_SDHCI

#define CONFIG_MMC_SMU_INIT
#define CONFIG_MMC_EARLY_INIT
#define MMC_MAX_CHANNEL		4
#define USE_MMC0
#define USE_MMC2

#define PHASE_DEVIDER		4

/* Boot configuration */
#define BOOT_ONENAND		0x1
#define BOOT_NAND		0x40000
#define BOOT_MMCSD		0x3
#define BOOT_NOR		0x4
#define BOOT_SEC_DEV		0x5
#define BOOT_EMMC		0x6
#define BOOT_EMMC_4_4		0x7
#define BOOT_EMMC_5_0		BOOT_EMMC_4_4
#define BOOT_USB                0x100

/*
 *  Boot device
 */
#define SDMMC_CH2		0x0
#define SDMMC_CH0		0x4
#define EMMC			0x14
#define SATA			0x18
#define SPI_SF			0x28
#define SFMC			0x34
#define USB			0x40

/* PWM */
#define CONFIG_PWM

#define CONFIG_BOARD_EARLY_INIT_F
#define CONFIG_BOARD_LATE_INIT

/* FASTBOOT */
#define CONFIG_FASTBOOT
#define CFG_FASTBOOT_SDMMCBSP
/* Fastboot variables */
#define CFG_FASTBOOT_TRANSFER_BUFFER		(0x48000000)
#define CFG_FASTBOOT_TRANSFER_BUFFER_SIZE	(0x30000000)   /* 768MB */
#define CFG_FASTBOOT_ADDR_KERNEL		(0x40008000)
#define CFG_FASTBOOT_ADDR_RAMDISK		(0x41000000)
#define CFG_FASTBOOT_PAGESIZE			(2048)
#define CFG_FASTBOOT_SDMMC_BLOCKSIZE		(512)

#define CONFIG_FASTBOOT_GET_MMC_PARTITIONS
#define CONFIG_FASTBOOT_FLASH_CHUNK
#ifdef CONFIG_FASTBOOT_FLASH_CHUNK
#define CONFIG_FASTBOOT_CHUNK_SIZE		64
#endif

#define CONFIG_FASTBOOT_SDFUSE

#define CONFIG_FASTBOOT_AUTO_REBOOT
#define CONFIG_FASTBOOT_AUTO_REBOOT_MODE	0x3

#define CONFIG_SPL
#define SDMMC_DEV_OFFSET			0x00000000
#define EMMC_DEV_OFFSET				0x00000014

#define CONFIG_BOOTCOMMAND	"run mmcboot"

#define CONFIG_BOOTCOMMAND_RAMDUMP	"fastboot"

/* allow to overwrite serial and ethaddr */
#define CONFIG_ENV_OVERWRITE

/* Miscellaneous configurable options */
#define CONFIG_SYS_LONGHELP		/* undef to save memory */
#define CONFIG_SYS_HUSH_PARSER		/* use "hush" command parser */
#define CONFIG_SYS_CBSIZE		256	/* Console I/O Buffer Size */
#define CONFIG_SYS_PBSIZE		384	/* Print Buffer Size */
#define CONFIG_SYS_MAXARGS		16	/* max number of command args */
#define CONFIG_SYS_BARGSIZE		CONFIG_SYS_CBSIZE

/* memtest works on */
#define CONFIG_SYS_MEMTEST_START	CONFIG_SYS_SDRAM_BASE
#define CONFIG_SYS_MEMTEST_END		(CONFIG_SYS_SDRAM_BASE + 0x5E00000)

#define CONFIG_SYS_HZ			1000

/* Stack sizes */
#define CONFIG_STACKSIZE		(256 << 10)	/* 256KB */

#define CONFIG_SYS_MONITOR_BASE	0x00000000

/* FLASH and environment organization */
#define CONFIG_SYS_NO_FLASH
#undef CONFIG_CMD_IMLS

#define CONFIG_ENV_IS_IN_MMC
#define CONFIG_SYS_MMC_ENV_DEV		0

/* Configuration for Partition */
#define CONFIG_DOS_PARTITION

/* Command definition*/
#include <config_cmd_default.h>

#define CONFIG_CMD_PING
#define CONFIG_CMD_ELF
#define CONFIG_CMD_MMC
#define CONFIG_CMD_EXT4
#define CONFIG_CMD_FAT
#define CONFIG_FAT_WRITE

/* GPT */
#define CONFIG_RANDOM_UUID
#define CONFIG_EFI_PARTITION
#define CONFIG_PARTITION_UUIDS
#define CONFIG_CMD_GPT
#define CONFIG_CMD_PART

#define CONFIG_CMD_MOVI
#define CONFIG_CMD_MOVINAND
#define CONFIG_CMD_BOOTZ

#define CONFIG_OF_LIBFDT

#define CONFIG_BOOTDELAY		3
#define CONFIG_ZERO_BOOTDELAY_CHECK

/* USB */
#define USBD_DOWN_ADDR			0x40000000

/* Configuration of ENV size on mmc */
#define CONFIG_ENV_SIZE		(16 << 10)	/* 16 KB */

#include <asm/arch/movi_partition.h>

/* Size of malloc() pool */
#define CONFIG_SYS_MALLOC_LEN		(CONFIG_ENV_SIZE + (1 << 20))

/* Configuration of ROOTFS_ATAGS */
#define CONFIG_ROOTFS_ATAGS
#define CONFIG_ROOTFS_LEN		100000

#define CONFIG_BOOT_PART_SIZE		64
#define CONFIG_ROOTFS_PART_SIZE		3072

#define CONFIG_ROOT_DEV		0
#define CONFIG_BOOT_PART	1
#define CONFIG_ROOT_PART	2

#define PARTS_DEFAULT							\
	"uuid_disk=${uuid_gpt_disk};"					\
	"name=boot,start=1MiB,size=" __stringify(CONFIG_BOOT_PART_SIZE) \
		"MiB,uuid=${uuid_gpt_boot};"				\
	"name=rootfs,size=-,uuid=${uuid_gpt_rootfs}\0"

#define PARTS_ANDROID							\
	"uuid_disk=${uuid_gpt_disk};"					\
	"name=boot,start=1MiB,size=" __stringify(CONFIG_BOOT_PART_SIZE) \
		"MiB,uuid=${uuid_gpt_boot};"				\
	"name=system,size=1024MiB,uuid=${uuid_gpt_system};"		\
	"name=cache,size=128MiB,uuid=${uuid_gpt_cache};"		\
	"name=userdata,size=-,uuid=${uuid_gpt_userdata}\0"

#define CONFIG_EXTRA_ENV_SETTINGS					\
	"console=" CONFIG_DEFAULT_CONSOLE				\
	"consoleon=set console=" CONFIG_DEFAULT_CONSOLE			\
		"; saveenv; reset\0"					\
	"consoleoff=set console=ram; saveenv; reset\0"			\
	"rootfslen=" __stringify(CONFIG_ROOTFS_LEN) "\0"		\
	"partitions=" PARTS_DEFAULT					\
	"partitions_android=" PARTS_ANDROID				\
	"rootdev=" __stringify(CONFIG_ROOT_DEV) "\0"			\
	"rootpart=" __stringify(CONFIG_ROOT_PART) "\0"			\
	"bootpart=" __stringify(CONFIG_BOOT_PART) "\0"			\
	"root_rw=rw\0"							\
	"opts=loglevel=4\0"						\
	"fdtfile=" CONFIG_FDT_FILE "\0"					\
	"kernel_file=zImage\0"						\
	"kernel_addr=40008000\0"					\
	"fdtaddr=40800000\0"						\
	"initrd_file=uInitrd\0"						\
	"initrd_addr=43000000\0"					\
	"sdrecovery=sdfuse format; sdfuse flashall 2\0"			\
	"boot_cmd=fatload mmc 0:1 $kernel_addr $kernel_file;"		\
		"fatload mmc 0:1 $fdtaddr $fdtfile;"			\
		"bootz $kernel_addr - $fdtaddr\0"			\
	"boot_cmd_initrd=fatload mmc 0:1 $kernel_addr $kernel_file;"	\
		"fatload mmc 0:1 $fdtaddr $fdtfile;"			\
		"fatload mmc 0:1 $initrd_addr $initrd_file;"		\
		"bootz $kernel_addr $initrd_addr $fdtaddr\0"		\
	"android_boot="							\
		"setenv bootargs ${console} root=/dev/ram0 "		\
		"${opts};"						\
		"run boot_cmd_initrd\0"					\
	"android_format=gpt write mmc 0 $partitions_android;"		\
		"setenv bootcmd run android_boot;"			\
		"saveenv; mmc rescan; fastboot\0"			\
	"ramfsboot=run sdrecovery;"					\
		"setenv bootargs ${console} root=/dev/ram0 "		\
		"rootfstype=ext2 ${opts} recovery;"			\
		"run boot_cmd_initrd\0"					\
	"mmcboot=setenv bootargs ${console} "				\
		"root=/dev/mmcblk${rootdev}p${rootpart} ${root_rw} "	\
		"rootfstype=ext4 ${opts};run boot_cmd\0"		\
	"bootcmd=run mmcboot\0"

#endif /* __ARTIK_COMMON_H */
