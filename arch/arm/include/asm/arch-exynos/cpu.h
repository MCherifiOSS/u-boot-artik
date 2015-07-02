/*
 * (C) Copyright 2010 Samsung Electronics
 * Minkyu Kang <mk7.kang@samsung.com>
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
 *
 */

#ifndef _EXYNOS4_CPU_H
#define _EXYNOS4_CPU_H

#if defined(CONFIG_CPU_EXYNOS5422_EVT0)
#include "cpu_exynos5422.h"
#endif
#define DEVICE_NOT_AVAILABLE		0

#define EXYNOS_CPU_NAME			"Exynos"
#define EXYNOS4_ADDR_BASE		0x10000000

/* RAMDUMP */
#define CONFIG_RAMDUMP_BASE		(CONFIG_SYS_SDRAM_BASE + 0x6000000)
#define CONFIG_RAMDUMP_LOGBUF		(CONFIG_RAMDUMP_BASE + 0x400000)
#define CONFIG_RAMDUMP_LOGSZ		0x200000
#define CONFIG_RAMDUMP_SCRATCH		(CONFIG_RAMDUMP_BASE + 0x10)
#define CONFIG_RAMDUMP_LASTBUF		(CONFIG_RAMDUMP_BASE + 0x14)

/* EXYNOS4 */
#define EXYNOS4_GPIO_PART3_BASE		0x03860000
#define EXYNOS4_PRO_ID			0x10000000
#define EXYNOS4_SYSREG_BASE		0x10010000
#define EXYNOS4_POWER_BASE		0x10020000
#define EXYNOS4_MCT_BASE		0x10050000
#define EXYNOS4_SWRESET			0x10020400
#define EXYNOS4_CLOCK_BASE		0x10030000
#define EXYNOS4_SYSTIMER_BASE		0x10050000
#define EXYNOS4_WATCHDOG_BASE		0x10060000
#define EXYNOS4_MIU_BASE		0x10600000
#define EXYNOS4_DMC0_BASE		0x10400000
#define EXYNOS4_DMC1_BASE		0x10410000
#define EXYNOS4_GPIO_PART2_BASE		0x11000000
#define EXYNOS4_GPIO_PART1_BASE		0x11400000
#define EXYNOS4_FIMD_BASE		0x11C00000
#define EXYNOS4_MIPI_DSIM_BASE		0x11C80000
#define EXYNOS4_USBOTG_BASE		0x12480000
#define EXYNOS4_MMC_BASE		0x12510000
#define EXYNOS4_SROMC_BASE		0x12570000
#define EXYNOS4_USB_HOST_EHCI_BASE	0x12580000
#define EXYNOS4_USBPHY_BASE		0x125B0000
#define EXYNOS4_UART_BASE		0x13800000
#define EXYNOS4_ADC_BASE		0x13910000
#define EXYNOS4_PWMTIMER_BASE		0x139D0000
#define EXYNOS4_MODEM_BASE		0x13A00000
#define EXYNOS4_USBPHY_CONTROL		0x10020704

#define EXYNOS4_GPIO_PART4_BASE		DEVICE_NOT_AVAILABLE

/* EXYNOS5 */
#if defined(CONFIG_CPU_EXYNOS5420)
#define EXYNOS5_GPIO_PART4_BASE		0x14010000
#else
#define EXYNOS5_GPIO_PART4_BASE		0x03860000
#endif
#define EXYNOS5_PRO_ID			0x10000000
#define EXYNOS5_CLOCK_BASE		0x10010000
#define EXYNOS5_POWER_BASE		0x10040000
#define EXYNOS5_SWRESET			0x10040400
#define EXYNOS5_SYSREG_BASE		0x10050000
#if defined(CONFIG_CPU_EXYNOS5420)
#define EXYNOS5_MCT_BASE			0x101C0000
#endif
#define EXYNOS5_WATCHDOG_BASE		0x101D0000
#define EXYNOS5_DMC_PHY0_BASE		0x10C00000
#define EXYNOS5_DMC_PHY1_BASE		0x10C10000
#if defined(CONFIG_CPU_EXYNOS5420)
#define EXYNOS5_GPIO_PART3_BASE		0x14000000
#else
#define EXYNOS5_GPIO_PART3_BASE		0x10D10000
#endif
#define EXYNOS5_DMC_CTRL_BASE		0x10DD0000
#if defined(CONFIG_CPU_EXYNOS5410) || defined(CONFIG_CPU_EXYNOS5420)
#define EXYNOS5_GPIO_PART1_BASE		0x13400000
#else
#define EXYNOS5_GPIO_PART1_BASE		0x11400000
#endif
#define EXYNOS5_MIPI_DSIM_BASE		0x11D00000
#define EXYNOS5_USB_HOST_EHCI_BASE	0x12110000
#define EXYNOS5_USBPHY_BASE		0x12130000
#define EXYNOS5_USBOTG_BASE		0x12140000
#define EXYNOS5_MMC_BASE		0x12200000
#define EXYNOS5_SROMC_BASE		0x12250000
#define EXYNOS5_UART_BASE		0x12C00000
#define EXYNOS5_PWMTIMER_BASE		0x12DD0000
#if defined(CONFIG_CPU_EXYNOS5420)
#define EXYNOS5_GPIO_PART2_BASE		0x13410000
#else
#define EXYNOS5_GPIO_PART2_BASE		0x13400000
#endif
#define EXYNOS5_FIMD_BASE		0x14400000

#define EXYNOS5_ADC_BASE		DEVICE_NOT_AVAILABLE
#define EXYNOS5_MODEM_BASE		DEVICE_NOT_AVAILABLE

/* EXYNOS5260 */
#define EXYNOS5260_PRO_ID		0x10000000
#define EXYNOS5260_CMU_TOP_BASE		0x10010000
#define EXYNOS5260_MCT_BASE		0x100B0000
#define EXYNOS5260_WATCHDOG_BASE	0x101D0000
#define EXYNOS5260_CMU_PERI_BASE	0x10200000
#define EXYNOS5260_SYSREG_BASE		0x10220000
#define EXYNOS5260_CMU_EGL_BASE		0x10600000
#define EXYNOS5260_CMU_KFC_BASE		0x10700000
#define EXYNOS5260_LPDDR3PHY_0_BASE	0x10C00000
#define EXYNOS5260_LPDDR3PHY_1_BASE	0x10C10000
#define EXYNOS5260_DREXI_0_NS_BASE	0x10C20000
#define EXYNOS5260_DREXI_1_NS_BASE	0x10C30000
#define EXYNOS5260_DREXI_0_S_BASE	0x10C40000
#define EXYNOS5260_DREXI_1_S_BASE	0x10C50000
#define EXYNOS5260_CMU_MIF_BASE		0x10CE0000
#define EXYNOS5260_POWER_BASE		0x10D50000
#define EXYNOS5260_SWRESET		0x10D50400
#define EXYNOS5260_GPIO_PART1_BASE	0x11600000
#define EXYNOS5260_MMC_BASE		0x12140000
#define EXYNOS5260_SROMC_BASE		0x12180000
#define EXYNOS5260_GPIO_PART2_BASE	0x12290000
#define EXYNOS5260_CMU_FSYS_BASE	0x122E0000
#define EXYNOS5260_GPIO_PART3_BASE	0x128B0000
#define EXYNOS5260_UART_BASE		0x12C00000
#define EXYNOS5260_PWMTIMER_BASE	0x12D90000
/* unnecessary definition */
#define EXYNOS5260_ADC_BASE		0x0
#define EXYNOS5260_CLOCK_BASE		0x0
#define EXYNOS5260_FIMD_BASE		0x0
#define EXYNOS5260_MIPI_DSIM_BASE	0x0
#define EXYNOS5260_GPIO_PART4_BASE	0x0
#define EXYNOS5260_MODEM_BASE		0x0
#define EXYNOS5260_USBPHY_BASE		0x0
#define EXYNOS5260_USB_HOST_EHCI_BASE	0x0
#define EXYNOS5260_USBOTG_BASE		0x0

/* EXYNOS5430 */
#define EXYNOS5430_PRO_ID		0x10000000
#define EXYNOS5430_PMU_PERIS_BASE	0x10010000
#define EXYNOS5430_CMU_TOP_BASE		0x10030000
#define EXYNOS5430_CMU_PERIS_BASE	0x10040000
#define EXYNOS5430_SYSREG_BASE		0x10050000
#define EXYNOS5430_MCT_BASE		0x101C0000
#define EXYNOS5430_WATCHDOG_BASE	0x101D0000
#define EXYNOS5430_DREX0_BASE		0x10400000
#define EXYNOS5430_DREX0_TZ_BASE	0x10410000
#define EXYNOS5430_LPDDR3PHY_0_BASE	0x10420000
#define EXYNOS5430_DREXI1_BASE		0x10440000
#define EXYNOS5430_DREXI1_TZ_BASE	0x10450000
#define EXYNOS5430_LPDDR3PHY_1_BASE	0x10460000
#define EXYNOS5430_GPIO_ALIVE_BASE	0x10580000
#define EXYNOS5430_CMU_MIF_BASE		0x105B0000
#define EXYNOS5430_POWER_BASE		0x105C0000
#define EXYNOS5430_SWRESET		0x105C0400
#define EXYNOS5430_CMU_CPIF_BASE	0x10FC0000
#define EXYNOS5430_GPIO_CPIF_BASE	0x10FE0000
#define EXYNOS5430_CMU_IMEM_BASE	0x11060000
#define EXYNOS5430_GPIO_AUD_BASE	0x114B0000
#define EXYNOS5430_CMU_EGL_BASE		0x11800000
#define EXYNOS5430_CMU_KFC_BASE		0x11900000
#define EXYNOS5430_CMU_BUS2_BASE	0x13400000
#define EXYNOS5430_CMU_BUS1_BASE	0x14800000
#define EXYNOS5430_UART_BASE		0x14C10000
#define EXYNOS5430_CMU_PERIC_BASE	0x14C80000
#define EXYNOS5430_GPIO_PERIC_BASE	0x14CC0000
#define EXYNOS5430_GPIO_NFC_BASE	0x14CD0000
#define EXYNOS5430_GPIO_TOUCH_BASE	0x14CE0000
#define EXYNOS5430_PWMTIMER_BASE	0x14DD0000
#define EXYNOS5430_MMC_BASE		0x15540000
#define EXYNOS5430_SROMC_BASE		0x15580000
#define EXYNOS5430_GPIO_FSYS_BASE	0x15690000
#define EXYNOS5430_CMU_FSYS_BASE	0x156E0000

/* unnecessary definition */
#define EXYNOS5430_GPIO_PART1_BASE	0x0
#define EXYNOS5430_GPIO_PART2_BASE	0x0
#define EXYNOS5430_GPIO_PART3_BASE	0x0
#define EXYNOS5430_ADC_BASE		0x0
#define EXYNOS5430_CLOCK_BASE		0x0
#define EXYNOS5430_FIMD_BASE		0x0
#define EXYNOS5430_MIPI_DSIM_BASE	0x0
#define EXYNOS5430_GPIO_PART4_BASE	0x0
#define EXYNOS5430_MODEM_BASE		0x0
#define EXYNOS5430_USBPHY_BASE		0x0
#define EXYNOS5430_USB_HOST_EHCI_BASE	0x0
#define EXYNOS5430_USBOTG_BASE		0x0

#define __REG(x)			(*(unsigned int *)(x))

#ifndef __ASSEMBLY__
#include <asm/io.h>
/* CHIP ID */
extern unsigned int s5p_chip_id[2];

/* CPU detection macros */
extern unsigned int s5p_cpu_id;
extern unsigned int s5p_cpu_rev;

static inline int s5p_get_cpu_rev(void)
{
	return s5p_cpu_rev;
}

static inline unsigned int s5p_get_cpu_id(void)
{
	unsigned int pro_id = (readl(EXYNOS4_PRO_ID) & 0x00FFF000) >> 12;

	switch (pro_id) {
	case 0x200:
		/* Exynos4210 EVT0 */
		return 0x4210;
	case 0x210:
		/* Exynos4210 EVT1 */
		return 0x4210;
	case 0x220:
		/* Exynos4212 */
		return 0x4212;
	case 0x415:
		/* Exynos4415 */
		return 0x4415;
	case 0x260:
		/* Exynos5260 */
		return 0x5260;
	case 0x412:
		pro_id = (readl(EXYNOS4_PRO_ID) & 0x0FFFF000) >> 12;
		if (pro_id == 0x4412) {
			/* Exynos4412 */
			return 0x4412;
		} else {
			/* Exynos5412 */
			return 0x5412;
		}
	case 0x520:
		/* Exynos5250 */
		return 0x5250;
	case 0x410:
		/* Exynos5410 */
		return 0x5410;
	case 0x420:
		/* Exynos5420 */
		return 0x5420;
	case 0x422:
		/* Exynos5422 */
		return 0x5422;
	case 0x430:
		/* Exynos5430 */
		return 0x5430;
	case 0x472:
		/* Exynos3250 */
		return 0x3250;
	}
}

static inline void s5p_set_cpu_id(void)
{
	unsigned int rev_num = (readl(EXYNOS4_PRO_ID) & 0x000000F0) >> 4;

	s5p_cpu_id = s5p_get_cpu_id();

	if(s5p_cpu_id == 0x4210)
		s5p_cpu_rev = 0;
	else
		s5p_cpu_rev = rev_num;
}

static inline char *s5p_get_cpu_name(void)
{
	return EXYNOS_CPU_NAME;
}

#ifndef CONFIG_SPL_BUILD
#define IS_SAMSUNG_TYPE(type, id)			\
static inline int cpu_is_##type(void)			\
{							\
	return (s5p_cpu_id >> 12) == id;		\
}
#else
#define IS_SAMSUNG_TYPE(type, id)			\
static inline int cpu_is_##type(void)			\
{							\
	return (s5p_get_cpu_id() >> 12) == id;		\
}
#endif

#ifdef CONFIG_CPU_EXYNOS3250
IS_SAMSUNG_TYPE(exynos4, 0x3)
#else
IS_SAMSUNG_TYPE(exynos4, 0x4)
#endif

IS_SAMSUNG_TYPE(exynos5, 0x5)

#define IS_SAMSUNG_PKG_TYPE(type, inform)			\
static inline int exynos_pkg_is_##type(void)			\
{							        \
	return ((readl(EXYNOS4_PRO_ID) & 0x00000F00) >> 8)  == inform; \
}

IS_SAMSUNG_PKG_TYPE(pop, 0x2)

#define SAMSUNG_BASE(device, base)				\
static inline unsigned int samsung_get_base_##device(void)	\
{								\
	if (cpu_is_exynos4()) {					\
		return EXYNOS4_##base;				\
	} else if (cpu_is_exynos5()) {				\
		if (s5p_get_cpu_id() == 0x5430) {		\
			return EXYNOS5430_##base;		\
		} else if (s5p_get_cpu_id() == 0x5260) {	\
			return EXYNOS5260_##base;		\
		} else {					\
			return EXYNOS5_##base;			\
		}						\
	} else {						\
		return 0;					\
	}							\
}

#define EXYNOS5260_BASE(device, base)				\
static inline unsigned int samsung_get_base_##device(void)	\
{								\
	return EXYNOS5260_##base;				\
}

#define EXYNOS5430_BASE(device, base)				\
static inline unsigned int exynos5430_get_base_##device(void)	\
{								\
	return EXYNOS5430_##base;				\
}

/* EXYNOS5260, EXYNOS5430 */
EXYNOS5260_BASE(clock_egl, CMU_EGL_BASE)
EXYNOS5260_BASE(clock_kfc, CMU_KFC_BASE)
EXYNOS5260_BASE(clock_mif, CMU_MIF_BASE)
EXYNOS5260_BASE(clock_top, CMU_TOP_BASE)
EXYNOS5260_BASE(clock_fsys, CMU_FSYS_BASE)

/* EXYNOS5430 */
EXYNOS5430_BASE(clock_egl, CMU_EGL_BASE)
EXYNOS5430_BASE(clock_kfc, CMU_KFC_BASE)
EXYNOS5430_BASE(clock_mif, CMU_MIF_BASE)
EXYNOS5430_BASE(clock_top, CMU_TOP_BASE)
EXYNOS5430_BASE(clock_fsys, CMU_FSYS_BASE)
EXYNOS5430_BASE(clock_bus1, CMU_BUS1_BASE)
EXYNOS5430_BASE(clock_bus2, CMU_BUS2_BASE)
EXYNOS5430_BASE(clock_peris, CMU_PERIS_BASE)
EXYNOS5430_BASE(clock_peric, CMU_PERIC_BASE)
EXYNOS5430_BASE(clock_cpif, CMU_CPIF_BASE)
EXYNOS5430_BASE(clock_imem, CMU_IMEM_BASE)
EXYNOS5430_BASE(gpio_alive, GPIO_ALIVE_BASE)
EXYNOS5430_BASE(gpio_cpif, GPIO_CPIF_BASE)
EXYNOS5430_BASE(gpio_aud, GPIO_AUD_BASE)
EXYNOS5430_BASE(gpio_peric, GPIO_PERIC_BASE)
EXYNOS5430_BASE(gpio_nfc, GPIO_NFC_BASE)
EXYNOS5430_BASE(gpio_touch, GPIO_TOUCH_BASE)
EXYNOS5430_BASE(gpio_fsys, GPIO_FSYS_BASE)

/* EXYNOS4, EXYNOS5 */
SAMSUNG_BASE(adc, ADC_BASE)
SAMSUNG_BASE(clock, CLOCK_BASE)
SAMSUNG_BASE(sysreg, SYSREG_BASE)
SAMSUNG_BASE(fimd, FIMD_BASE)
SAMSUNG_BASE(mipi_dsim, MIPI_DSIM_BASE)
SAMSUNG_BASE(gpio_part1, GPIO_PART1_BASE)
SAMSUNG_BASE(gpio_part2, GPIO_PART2_BASE)
SAMSUNG_BASE(gpio_part3, GPIO_PART3_BASE)
SAMSUNG_BASE(gpio_part4, GPIO_PART4_BASE)
SAMSUNG_BASE(pro_id, PRO_ID)
SAMSUNG_BASE(mmc, MMC_BASE)
SAMSUNG_BASE(modem, MODEM_BASE)
SAMSUNG_BASE(sromc, SROMC_BASE)
SAMSUNG_BASE(swreset, SWRESET)
SAMSUNG_BASE(timer, PWMTIMER_BASE)
SAMSUNG_BASE(uart, UART_BASE)
SAMSUNG_BASE(usb_phy, USBPHY_BASE)
SAMSUNG_BASE(usb_ehci, USB_HOST_EHCI_BASE)
SAMSUNG_BASE(usb_otg, USBOTG_BASE)
SAMSUNG_BASE(watchdog, WATCHDOG_BASE)
SAMSUNG_BASE(power, POWER_BASE)
#endif

#endif	/* _EXYNOS4_CPU_H */
