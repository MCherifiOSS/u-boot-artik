/*
 * Copyright (C) 2015 Samsung Electronics
 *
 * Configuration settings for the SAMSUNG ARTIK5 (EXYNOS3250) board.
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

#ifndef __ARTIK5_H
#define __ARTIK5_H

/* High Level Configuration Options */
#define CONFIG_ARCH_EXYNOS4
#define CONFIG_CPU_EXYNOS3250			/* which is in a Exynos3250 */
#define CONFIG_MACH_SMDK3250
#define CONFIG_MACH_ARTIK5

#include <configs/artik_common.h>

/* TRUSTZONE */
#define CONFIG_TRUSTZONE_RESERVED_DRAM  0x800000	/* USE 8MB */

#define CONFIG_SPL_TEXT_BASE		0x02025000
#define CONFIG_PHY_IRAM_BASE		(0x02020000)
#define CONFIG_PHY_IRAM_NS_BASE		(CONFIG_PHY_IRAM_BASE + 0x3F000)

/*
*	clock setting:
*	APLL = 700MHz
*	MPLL = 1600MHz
* 	BPLL = 800MHz
*	EPLL = 800MHz
*	VPLL = 335MHz
*/
#define CONFIG_SYS_FIN_24

/*	Specify the ARM_CLK frequency in MHz	*/
#define CONFIG_ARM_CLK			700
/* MCLK_CDREX	*/
#define MCLK_CDREX_400			1

/* select serial console configuration */
#define CONFIG_SERIAL2			/* use SERIAL 2 */

/* SD/MMC configuration */

#define SDR_CH0                 0x03040000
#define DDR_CH0                 0x03010000
#define SDR_CH2                 0x03010000
#define DDR_CH2                 0x03010000

/* USB */
#define CONFIG_S3C_USBD
#undef CONFIG_USB_CPUMODE

#define EXYNOS_SYSREG_BASE              EXYNOS4_SYSREG_BASE
#define EXYNOS_POWER_BASE               EXYNOS4_POWER_BASE

#define CFG_FASTBOOT_MMC_BUFFER			(0x78000000)

/* MMC SPL */
#define CONFIG_SPL_15KB
#define IROM_FNPTR_BASE                         0x020200A0
#define SECCOND_BOOT_INFORM_OFFSET              0x00000028

/* Miscellaneous configurable options */
#define CONFIG_SYS_PROMPT		"ARITK5 # "
#define CONFIG_DEFAULT_CONSOLE		"console=ttySAC2,115200n8\0"
#define CONFIG_SYS_LOAD_ADDR		CONFIG_SYS_TEXT_BASE

#define CONFIG_NR_DRAM_BANKS	8
#define SDRAM_BANK_SIZE		(64UL << 20UL)	/* 64 MB */
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
#define PHYS_SDRAM_8_SIZE	(SDRAM_BANK_SIZE - \
				 CONFIG_TRUSTZONE_RESERVED_DRAM)

#define CONFIG_IDENT_STRING		" for ARTIK5"

#define CONFIG_PMIC_S5M8767A

/* Configuration for Partition */
#define CFG_PARTITION_START     0x6400000
#define CONFIG_IRAM_STACK	0x02060000

#define CONFIG_SYS_INIT_SP_ADDR	(CONFIG_SYS_LOAD_ADDR - 0x1000000)

/* Base address for secondary boot information */
#define CONFIG_SECONDARY_BOOT_INFORM_BASE	(CONFIG_SYS_TEXT_BASE - 0x8)

/* RST_STAT */
#define SWRESET			(1 << 29)
#define WRESET			(1 << 28)
#define ISP_ARM_WDTRESET	(1 << 25)
#define SYS_WDTRESET		(1 << 20)
#define PINRESET		(1 << 16)

#endif	/* __ARTIK5_H */
