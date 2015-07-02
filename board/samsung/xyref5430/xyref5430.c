/*
 * Copyright (C) 2012 Samsung Electronics
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

#include <common.h>
#include <asm/io.h>
#include <netdev.h>
#include <asm/arch/cpu.h>
#include <asm/arch/clk.h>
#include <asm/arch/clock.h>
#include <asm/arch/power.h>
#include <asm/arch/gpio.h>
#include <asm/arch/mmc.h>
#include <asm/arch/pinmux.h>
#include <asm/arch/pmu.h>
#include <asm/arch/sromc.h>
#include <asm/arch/sysreg.h>
#include "pmic.h"
#ifndef CONFIG_MACH_UNIVERSAL5430
#include "pmic_lm3560.h"
#endif
#include "setup.h"

#define DEBOUNCE_DELAY	10000

#define Inp32(addr) (*(volatile u32 *)(addr))
#define GetBits(uAddr, uBaseBit, uMaskValue) \
    ((Inp32(uAddr)>>(uBaseBit))&(uMaskValue))
#define GetPOPType() (GetBits(0x10000004, 4, 0x3))

DECLARE_GLOBAL_DATA_PTR;
unsigned int pmic;

static void display_bl1_version(void)
{
	char bl1_version[9] = {0};

	/* display BL1 version */
#if defined(CONFIG_TRUSTZONE_ENABLE)
	printf("\nTrustZone Enabled BSP");
	strncpy(&bl1_version[0], (char *)CONFIG_BL1_VERSION_INFORM, 8);
	printf("\nBL1 version: %s\n", &bl1_version[0]);
#endif
}

static void display_chip_id(void)
{
#if defined(CONFIG_DISPLAY_CHIPID)
	s5p_chip_id[0] = readl(EXYNOS5430_PRO_ID + CHIPID0_OFFSET);
	s5p_chip_id[1] = (readl(EXYNOS5430_PRO_ID + CHIPID1_OFFSET) & 0xFFFF);

	printf("\nChip ID : %04x%08x\n", s5p_chip_id[1], s5p_chip_id[0]);
#endif
}

static void display_pmic_info(void)
{
#if defined(CONFIG_PM)
	unsigned char pmic_id = 0;
	unsigned char rtc_ctrl = 0;
	unsigned char wrstbi_ctrl = 0;
	unsigned char read_vol_mif = 0;
	unsigned char read_vol_egl = 0;
	unsigned char read_vol_kfc = 0;
	unsigned char read_vol_int = 0;
	unsigned char read_vol_g3d = 0;
	unsigned char ldo6 = 0, ldo7 = 0;
	unsigned char ldo19 = 0, ldo20 = 0;
	unsigned char ldo27 = 0, ldo28 = 0;
	unsigned char ldo_dvs1 = 0, ldo_dvs2 = 0, ldo_dvs3 = 0;
#ifndef CONFIG_MACH_UNIVERSAL5430
	unsigned char enable_reg = 0;
#endif

	/* Read PMIC register value */
	IIC0_ERead(S2MPS13_ADDR, PMIC_ID, &pmic_id);
	IIC0_ERead(S2MPS13_ADDR, RTC_BUF, &rtc_ctrl);
	IIC0_ERead(S2MPS13_ADDR, WRSTBI_CTRL, &wrstbi_ctrl);
	IIC0_ERead(S2MPS13_ADDR, LDO_DVS1, &ldo_dvs1);
	IIC0_ERead(S2MPS13_ADDR, LDO_DVS2, &ldo_dvs2);
	IIC0_ERead(S2MPS13_ADDR, LDO_DVS3, &ldo_dvs3);
	IIC0_ERead(S2MPS13_ADDR, LDO6_CTRL, &ldo6);
	IIC0_ERead(S2MPS13_ADDR, LDO7_CTRL, &ldo7);
	IIC0_ERead(S2MPS13_ADDR, LDO19_CTRL, &ldo19);
	IIC0_ERead(S2MPS13_ADDR, LDO20_CTRL, &ldo20);
	IIC0_ERead(S2MPS13_ADDR, LDO27_CTRL, &ldo27);
	IIC0_ERead(S2MPS13_ADDR, LDO28_CTRL, &ldo28);
	IIC0_ERead(S2MPS13_ADDR, BUCK1_OUT, &read_vol_mif);
	IIC0_ERead(S2MPS13_ADDR, BUCK2_OUT, &read_vol_egl);
	IIC0_ERead(S2MPS13_ADDR, BUCK3_OUT, &read_vol_kfc);
	IIC0_ERead(S2MPS13_ADDR, BUCK4_OUT, &read_vol_int);
	IIC0_ERead(S2MPS13_ADDR, BUCK6_OUT, &read_vol_g3d);

	printf("\nPMIC: S2MPS13(REV%x)\n", pmic_id);
	/* print BUCK voltage */
	if (pmic_id == REV_00) {
		read_vol_mif += VDD_REG_VALUE_BASE;
		read_vol_egl += VDD_REG_VALUE_BASE;
		read_vol_kfc += VDD_REG_VALUE_BASE;
		read_vol_int += VDD_REG_VALUE_BASE;
		read_vol_g3d += VDD_REG_VALUE_BASE;
	}
	printf("MIF: %dmV\t", RD_BUCK_VOLT((unsigned int)read_vol_mif));
	printf("EGL: %dmV\t", RD_BUCK_VOLT((unsigned int)read_vol_egl));
	printf("KFC: %dmV\t\n", RD_BUCK_VOLT((unsigned int)read_vol_kfc));
	printf("INT: %dmV\t", RD_BUCK_VOLT((unsigned int)read_vol_int));
	printf("G3D: %dmV\t\n", RD_BUCK_VOLT((unsigned int)read_vol_g3d));
	/* print rtc_buf & wrstbi register value */
	printf("RTC_BUF: 0x%x, WRSTBI: 0x%x\n", rtc_ctrl, wrstbi_ctrl);
	/* print ldo register value */
	printf("LDO6: 0x%x, LDO7: 0x%x\n", ldo6, ldo7);
	printf("LDO19: 0x%x, LDO20: 0x%x\n", ldo19, ldo20);
	printf("LDO27: 0x%x, LDO28: 0x%x\n", ldo27, ldo28);
	printf("LDO_DVS1: 0x%x, LDO_DVS2: 0x%x, LDO_DVS3: 0x%x\n",
					ldo_dvs1, ldo_dvs2, ldo_dvs3);

#ifndef CONFIG_MACH_UNIVERSAL5430
	/* setup PMIC lm3560 */
	IIC2_ESetport();
	/* set GPC0[2], GPC0[3] output low */
	*(unsigned int *)0x14CC0040 &= ~0xFF00;
	*(unsigned int *)0x14CC0040 |= 0x1100;
	*(unsigned int *)0x14CC0044 &= ~0xC;
	/* set GPF0[1] output high */
	*(unsigned int *)0x14CC01E0 &= ~0xF0;
	*(unsigned int *)0x14CC01E0 |= 0x10;
	*(unsigned int *)0x14CC01E4 |= 0x2;

	IIC2_EWrite(LM3560_ADDR, 0xE0, 0xEF);
	IIC2_EWrite(LM3560_ADDR, 0xC0, 0xFF);
	IIC2_EWrite(LM3560_ADDR, 0x11, 0x78);
#endif
#endif
}

static void display_boot_device_info(void)
{
	struct exynos5430_power *pmu = (struct exynos5430_power *)EXYNOS5430_POWER_BASE;
	int OmPin;

	OmPin = readl(&pmu->inform3);

	printf("\nChecking Boot Mode ...");

	if (OmPin == BOOT_MMCSD) {
		printf(" SDMMC\n");
	} else if (OmPin == BOOT_EMMC) {
		printf(" EMMC\n");
	} else if (OmPin == BOOT_EMMC_4_4) {
		printf(" EMMC\n");
	} else {
		printf(" Please check OM_pin\n");
	}
}

int board_init(void)
{
	display_bl1_version();

	display_chip_id();

	display_pmic_info();

	display_boot_device_info();

	gd->bd->bi_boot_params = (PHYS_SDRAM_1 + 0x100UL);

	return 0;
}

int dram_init(void)
{
	gd->ram_size	= get_ram_size((long *)PHYS_SDRAM_1, PHYS_SDRAM_1_SIZE)
			+ get_ram_size((long *)PHYS_SDRAM_2, PHYS_SDRAM_2_SIZE)
			+ get_ram_size((long *)PHYS_SDRAM_3, PHYS_SDRAM_3_SIZE)
			+ get_ram_size((long *)PHYS_SDRAM_4, PHYS_SDRAM_5_SIZE)
			+ get_ram_size((long *)PHYS_SDRAM_5, PHYS_SDRAM_4_SIZE)
			+ get_ram_size((long *)PHYS_SDRAM_6, PHYS_SDRAM_5_SIZE)
			+ get_ram_size((long *)PHYS_SDRAM_7, PHYS_SDRAM_6_SIZE);

	gd->ram_size += get_ram_size((long *)PHYS_SDRAM_8, PHYS_SDRAM_8_END_SIZE);

	return 0;
}

void dram_init_banksize(void)
{
	gd->bd->bi_dram[0].start = PHYS_SDRAM_1;
	gd->bd->bi_dram[0].size = get_ram_size((long *)PHYS_SDRAM_1,
							PHYS_SDRAM_1_SIZE);
	gd->bd->bi_dram[1].start = PHYS_SDRAM_2;
	gd->bd->bi_dram[1].size = get_ram_size((long *)PHYS_SDRAM_2,
							PHYS_SDRAM_2_SIZE);
	gd->bd->bi_dram[2].start = PHYS_SDRAM_3;
	gd->bd->bi_dram[2].size = get_ram_size((long *)PHYS_SDRAM_3,
							PHYS_SDRAM_3_SIZE);
	gd->bd->bi_dram[3].start = PHYS_SDRAM_4;
	gd->bd->bi_dram[3].size = get_ram_size((long *)PHYS_SDRAM_4,
							PHYS_SDRAM_4_SIZE);
	gd->bd->bi_dram[4].start = PHYS_SDRAM_5;
	gd->bd->bi_dram[4].size = get_ram_size((long *)PHYS_SDRAM_5,
							PHYS_SDRAM_5_SIZE);
	gd->bd->bi_dram[5].start = PHYS_SDRAM_6;
	gd->bd->bi_dram[5].size = get_ram_size((long *)PHYS_SDRAM_6,
							PHYS_SDRAM_6_SIZE);
	gd->bd->bi_dram[6].start = PHYS_SDRAM_7;
	gd->bd->bi_dram[6].size = get_ram_size((long *)PHYS_SDRAM_7,
							PHYS_SDRAM_7_SIZE);
	if (GetPOPType() == 0) {
		gd->bd->bi_dram[7].start = PHYS_SDRAM_8;
		gd->bd->bi_dram[7].size = get_ram_size((long *)PHYS_SDRAM_8,
			PHYS_SDRAM_8_END_SIZE - CONFIG_UBOOT_ATAG_RESERVED_DRAM);
	} else if (GetPOPType() == 1) {
		gd->bd->bi_dram[7].start = PHYS_SDRAM_8;
		gd->bd->bi_dram[7].size = PHYS_SDRAM_8_SIZE;
		gd->bd->bi_dram[8].start = PHYS_SDRAM_9;
		gd->bd->bi_dram[8].size = PHYS_SDRAM_9_SIZE;
		gd->bd->bi_dram[9].start = PHYS_SDRAM_10;
		gd->bd->bi_dram[9].size = PHYS_SDRAM_10_SIZE;
		gd->bd->bi_dram[10].start = PHYS_SDRAM_11;
		gd->bd->bi_dram[10].size = PHYS_SDRAM_11_SIZE;
		gd->bd->bi_dram[11].start = PHYS_SDRAM_12;
		gd->bd->bi_dram[11].size = PHYS_SDRAM_12_SIZE - CONFIG_UBOOT_ATAG_RESERVED_DRAM;
	}
}

int board_eth_init(bd_t *bis)
{
	return 0;
}

#ifdef CONFIG_DISPLAY_BOARDINFO
int checkboard(void)
{
#if defined(CONFIG_MACH_UNIVERSAL5430)
	printf("\nBoard: UNIVERSAL5430\n");
#else
	printf("\nBoard: XYREF5430\n");
#endif

	return 0;
}
#endif

#ifdef CONFIG_GENERIC_MMC
int board_mmc_init(bd_t *bis)
{
	struct exynos5430_power *pmu = (struct exynos5430_power *)EXYNOS5430_POWER_BASE;
	int err, OmPin;

	OmPin = readl(&pmu->inform3);

	err = exynos_pinmux_config(PERIPH_ID_SDMMC2, PINMUX_FLAG_NONE);
	if (err) {
		debug("SDMMC2 not configured\n");
		return err;
	}

	err = exynos_pinmux_config(PERIPH_ID_SDMMC0, PINMUX_FLAG_8BIT_MODE);
	if (err) {
		debug("MSHC0 not configured\n");
		return err;
	}

	switch (OmPin) {
	case BOOT_EMMC_4_4:
#if defined(USE_MMC0)
		set_mmc_clk(PERIPH_ID_SDMMC0, 1);

		err = s5p_mmc_init(PERIPH_ID_SDMMC0, 8);
#endif
#if defined(USE_MMC2)
		set_mmc_clk(PERIPH_ID_SDMMC2, 1);

		err = s5p_mmc_init(PERIPH_ID_SDMMC2, 4);
#endif
		break;
	default:
#if defined(USE_MMC2)
		set_mmc_clk(PERIPH_ID_SDMMC2, 1);

		err = s5p_mmc_init(PERIPH_ID_SDMMC2, 4);
#endif
#if defined(USE_MMC0)
		set_mmc_clk(PERIPH_ID_SDMMC0, 1);

		err = s5p_mmc_init(PERIPH_ID_SDMMC0, 8);
#endif
		break;
	}

	return err;
}
#endif

static int board_uart_init(void)
{
	int err;

	err = exynos_pinmux_config(PERIPH_ID_UART0, PINMUX_FLAG_NONE);
	if (err) {
		debug("UART0 not configured\n");
		return err;
	}

	err = exynos_pinmux_config(PERIPH_ID_UART1, PINMUX_FLAG_NONE);
	if (err) {
		debug("UART1 not configured\n");
		return err;
	}

	err = exynos_pinmux_config(PERIPH_ID_UART2, PINMUX_FLAG_NONE);
	if (err) {
		debug("UART2 not configured\n");
		return err;
	}

	return 0;
}

#ifdef CONFIG_BOARD_EARLY_INIT_F
int board_early_init_f(void)
{
#ifdef CONFIG_PREP_BOARDCONFIG_FOR_KERNEL
	/* power off domain */
	writel(0x00000000, EXYNOS5430_POWER_G2D_CONFIGURATION);
	writel(0x00000000, EXYNOS5430_POWER_GSCL_CONFIGURATION);
	writel(0x00000000, EXYNOS5430_POWER_MSCL_CONFIGURATION);
	writel(0x00000000, EXYNOS5430_POWER_G3D_CONFIGURATION);
	writel(0x00000000, EXYNOS5430_POWER_AUD_CONFIGURATION);
	//writel(0x00000000, EXYNOS5430_POWER_FSYS_CONFIGURATION);
	writel(0x00000000, EXYNOS5430_POWER_MFC0_CONFIGURATION);
	writel(0x00000000, EXYNOS5430_POWER_MFC1_CONFIGURATION);
	writel(0x00000000, EXYNOS5430_POWER_HEVC_CONFIGURATION);
	writel(0x00000000, EXYNOS5430_POWER_ISP_CONFIGURATION);
	writel(0x00000000, EXYNOS5430_POWER_CAM0_CONFIGURATION);
	writel(0x00000000, EXYNOS5430_POWER_CAM1_CONFIGURATION);
	writel(0x00000000, EXYNOS5430_POWER_DISP_CONFIGURATION);
#endif

	return board_uart_init();
}
#endif

int board_late_init(void)
{
	struct exynos5430_power *pmu = (struct exynos5430_power *)EXYNOS5430_POWER_BASE;
	unsigned int rst_stat = 0;

	rst_stat = readl(&pmu->rst_stat);
	printf("rst_stat : 0x%x\n", rst_stat);
#ifdef CONFIG_RAMDUMP_MODE
#ifndef CONFIG_MACH_UNIVERSAL5430
	struct exynos5430_gpio_alive *gpio_alive =
		(struct exynos5430_gpio_alive *) exynos5430_get_base_gpio_alive();
	unsigned int gpio_debounce_cnt = 0;
	int err;

	err = exynos_pinmux_config(PERIPH_ID_INPUT_X2_0, PINMUX_FLAG_NONE);
	if (err) {
		debug("GPX2_0 INPUT not configured\n");
		return err;
	}
	while (s5p_gpio_get_value(&gpio_alive->a2, 0) == 0) {
		/* wait for 50ms */
		if (gpio_debounce_cnt < 5) {
			udelay(DEBOUNCE_DELAY);
			gpio_debounce_cnt++;
		} else {
			/* run fastboot */
			run_command("fastboot", 0);
			break;
		}
	}
#endif
	/* check reset status for ramdump */
	if ((rst_stat & (WRESET | CORTEXA7_WDTRESET | CORTEXA15_WDTRESET))
		|| (readl(&pmu->sysip_dat0) == CONFIG_RAMDUMP_MODE)
		|| (readl(CONFIG_RAMDUMP_SCRATCH) == CONFIG_RAMDUMP_MODE)) {
		/* run fastboot */
		run_command("fastboot", 0);
	}
#endif
#ifdef CONFIG_RECOVERY_MODE
	u32 second_boot_info = readl(second_boot_info_base);

	if (second_boot_info == 1) {
		printf("###Recovery Mode###\n");
		writel(0x0, second_boot_info_base);
		setenv("bootcmd", CONFIG_RECOVERYCOMMAND);
	}
#endif

	if ((readl(&pmu->sysip_dat0)) == CONFIG_FACTORY_RESET_MODE) {
		writel(0x0, &pmu->sysip_dat0);
		setenv("bootcmd", CONFIG_FACTORY_RESET_BOOTCOMMAND);
	}
	return 0;
}

#ifdef CONFIG_PREP_BOARDCONFIG_FOR_KERNEL
void prep_boardconfig_for_kernel(void)
{
	/* power on domain */
	writel(0x0000000f, EXYNOS5430_POWER_G2D_CONFIGURATION);
	writel(0x0000000f, EXYNOS5430_POWER_GSCL_CONFIGURATION);
	writel(0x0000000f, EXYNOS5430_POWER_MSCL_CONFIGURATION);
	writel(0x0000000f, EXYNOS5430_POWER_G3D_CONFIGURATION);
	writel(0x0000000f, EXYNOS5430_POWER_AUD_CONFIGURATION);
	//writel(0x0000000f, EXYNOS5430_POWER_FSYS_CONFIGURATION);
	writel(0x0000000f, EXYNOS5430_POWER_MFC0_CONFIGURATION);
	writel(0x0000000f, EXYNOS5430_POWER_MFC1_CONFIGURATION);
	writel(0x0000000f, EXYNOS5430_POWER_HEVC_CONFIGURATION);
	writel(0x0000000f, EXYNOS5430_POWER_ISP_CONFIGURATION);
	writel(0x0000000f, EXYNOS5430_POWER_CAM0_CONFIGURATION);
	writel(0x0000000f, EXYNOS5430_POWER_CAM1_CONFIGURATION);
	writel(0x0000000f, EXYNOS5430_POWER_DISP_CONFIGURATION);
}
#endif

unsigned int get_board_rev(void)
{
	struct exynos5430_power *pmu = (struct exynos5430_power *)EXYNOS5430_POWER_BASE;
	unsigned int rev = 0;
	int adc_val = 0;
	unsigned int timeout, con;

	return (rev | pmic);
}
