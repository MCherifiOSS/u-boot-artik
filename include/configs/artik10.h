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

#ifndef __ARTIK10_H
#define __ARTIK10_H

/* High Level Configuration Options */
#define CONFIG_EXYNOS5			/* which is in a Exynos5 Family */
#define CONFIG_ARCH_EXYNOS5	        /* which is in a Exynos5 Family */
#define CONFIG_CPU_EXYNOS5420		/* which is in a Exynos5420 */
#define CONFIG_CPU_EXYNOS5422		/* which is in a Exynos5422 */
#define CONFIG_CPU_EXYNOS5422_EVT0	/* which is in a Exynos5422 EVT0 */
#define CONFIG_MACH_ARTIK10		/* which is in a ARTIK10 */
#define CONFIG_MACH_TYPE		MACH_TYPE_SMDK5422

#include <configs/artik_common.h>

/* TRUSTZONE */
#define CONFIG_TRUSTZONE_RESERVED_DRAM  0x1600000

#define CONFIG_SPL_TEXT_BASE		0x02027000
#define CONFIG_PHY_IRAM_BASE            (0x02020000)
#define CONFIG_PHY_IRAM_NS_BASE         (CONFIG_PHY_IRAM_BASE + 0x53000)

/* Power Management is enabled */
#define CONFIG_PM
#define CONFIG_PM_VDD_ARM	1.00
#define CONFIG_PM_VDD_KFC	1.0250
#define CONFIG_PM_VDD_INT	1.00
#define CONFIG_PM_VDD_G3D	1.00
#define CONFIG_PM_VDD_MIF	1.10

/* select serial console configuration */
#define CONFIG_SERIAL3			/* use SERIAL 3 */
#define EXYNOS5_DEFAULT_UART_OFFSET	0x010000

/* SD/MMC configuration */
#define CONFIG_MMC_64BIT_BUS

#define SDR_CH0			0x03030003
#define DDR_CH0			0x03020001
#define SDR_CH2			0x03020001
#define DDR_CH2			0x03030002

/* USB */
#define CONFIG_CMD_USB
#define CONFIG_USB_EHCI
#define CONFIG_USB_EHCI_EXYNOS
#define CONFIG_USB_STORAGE

/* OHCI : Host 1.0 */
#define CONFIG_USB_OHCI

#define CONFIG_EXYNOS_USBD3
#undef CONFIG_USB_CPUMODE

#define CONFIG_EXYNOS_USBD3_CH0

#define EXYNOS_SYSREG_BASE		EXYNOS5_SYSREG_BASE
#define EXYNOS_POWER_BASE		EXYNOS5_POWER_BASE

#define CONFIG_REVISION_TAG

/*
 * USBD 3.0 SFR
 */
#define USBDEVICE3_LINK_CH0_BASE	0x12000000
#define USBDEVICE3_PHYCTRL_CH0_BASE	0x12100000
#define USBDEVICE3_LINK_CH1_BASE	0x12400000
#define USBDEVICE3_PHYCTRL_CH1_BASE	0x12500000

#define EXYNOS_USB_PHY_BASE		EXYNOS5_USBPHY_BASE
#define EXYNOS_USB_LINK_BASE		EXYNOS5_USBOTG_BASE

#define CONFIG_EXYNOS_THERMAL
#define CONFIG_EXYNOS_THERMAL_STABLE_TEMP	82

/* Fastboot variables */
#define CFG_FASTBOOT_MMC_BUFFER			(CONFIG_SYS_SDRAM_BASE + \
						0x58000000)
/* MMC SPL */
#define IROM_FNPTR_BASE				0x02020030
#define SECCOND_BOOT_INFORM_OFFSET		0x00000004

/* Miscellaneous configurable options */
#define CONFIG_SYS_PROMPT		"ARTIK10 # "
#define CONFIG_DEFAULT_CONSOLE		"console=ttySAC3,115200n8\0"
#define CONFIG_SYS_LOAD_ADDR		(CONFIG_SYS_SDRAM_BASE + 0x3E00000)

#define CONFIG_NR_DRAM_BANKS	8
#define SDRAM_BANK_SIZE		(256UL << 20UL)	/* 256 MB */
#define PHYS_SDRAM_1		CONFIG_SYS_SDRAM_BASE
#define PHYS_SDRAM_1_SIZE	SDRAM_BANK_SIZE
#define PHYS_SDRAM_2		(CONFIG_SYS_SDRAM_BASE + SDRAM_BANK_SIZE)
#define PHYS_SDRAM_2_SIZE	SDRAM_BANK_SIZE
#define PHYS_SDRAM_3		(CONFIG_SYS_SDRAM_BASE + (2 * SDRAM_BANK_SIZE))
#define PHYS_SDRAM_3_SIZE	SDRAM_BANK_SIZE
#define PHYS_SDRAM_4		(CONFIG_SYS_SDRAM_BASE + (3 * SDRAM_BANK_SIZE))
#define PHYS_SDRAM_4_SIZE	SDRAM_BANK_SIZE
#define PHYS_SDRAM_5		(CONFIG_SYS_SDRAM_BASE + (4 * SDRAM_BANK_SIZE))
#define PHYS_SDRAM_5_SIZE	SDRAM_BANK_SIZE
#define PHYS_SDRAM_6		(CONFIG_SYS_SDRAM_BASE + (5 * SDRAM_BANK_SIZE))
#define PHYS_SDRAM_6_SIZE	SDRAM_BANK_SIZE
#define PHYS_SDRAM_7		(CONFIG_SYS_SDRAM_BASE + (6 * SDRAM_BANK_SIZE))
#define PHYS_SDRAM_7_SIZE	SDRAM_BANK_SIZE
#define PHYS_SDRAM_8		(CONFIG_SYS_SDRAM_BASE + (7 * SDRAM_BANK_SIZE))
#define PHYS_SDRAM_8_SIZE	SDRAM_BANK_SIZE
#define PHYS_SDRAM_8_END_SIZE   (SDRAM_BANK_SIZE -                            \
                                               CONFIG_TRUSTZONE_RESERVED_DRAM)
#define PHYS_SDRAM_9		(CONFIG_SYS_SDRAM_BASE + (8 * SDRAM_BANK_SIZE))
#define PHYS_SDRAM_9_SIZE	SDRAM_BANK_SIZE
#define PHYS_SDRAM_10		(CONFIG_SYS_SDRAM_BASE + (9 * SDRAM_BANK_SIZE))
#define PHYS_SDRAM_10_SIZE	SDRAM_BANK_SIZE
#define PHYS_SDRAM_11		(CONFIG_SYS_SDRAM_BASE + (10 * SDRAM_BANK_SIZE))
#define PHYS_SDRAM_11_SIZE	SDRAM_BANK_SIZE
#define PHYS_SDRAM_12		(CONFIG_SYS_SDRAM_BASE + (11 * SDRAM_BANK_SIZE))
#define PHYS_SDRAM_12_SIZE       (SDRAM_BANK_SIZE -                            \
                                               CONFIG_TRUSTZONE_RESERVED_DRAM)

#define CONFIG_IDENT_STRING		" for ARTIK10"

#define CFG_PARTITION_START	0x4000000
#define CONFIG_IRAM_STACK	0x02074000

#define CONFIG_SYS_INIT_SP_ADDR	(CONFIG_SYS_LOAD_ADDR - 0x1000000)

#define CONFIG_FDT_FILE		"exynos5422-artik10.dtb"

/* Base address for secondary boot information */
#define CONFIG_SECONDARY_BOOT_INFORM_BASE	(CONFIG_SYS_TEXT_BASE - 0x8)

/* RST_STAT */
#define WRESET				(1 << 10)
#define SYS_WDTRESET			(1 << 9)

#endif	/* __ARTIK10_H */
