/*
 * Copyright (C) 2010 Samsung Electronics
 * Minkyu Kang <mk7.kang@samsung.com>
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
#include <asm/arch/clock.h>
#include <asm/arch/clk.h>
#include <asm/arch/periph.h>

/* exynos4: return pll clock frequency */
static unsigned long exynos4_get_pll_clk(int pllreg)
{
	struct exynos4_clock *clk =
		(struct exynos4_clock *)samsung_get_base_clock();
	unsigned long r, m, p, s, k = 0, mask, fout;
	unsigned int freq;

	switch (pllreg) {
	case APLL:
		r = readl(&clk->apll_con0);
		break;
	case MPLL:
		r = readl(&clk->mpll_con0);
		break;
	case EPLL:
		r = readl(&clk->epll_con0);
		k = readl(&clk->epll_con1);
		break;
	case VPLL:
		r = readl(&clk->vpll_con0);
		k = readl(&clk->vpll_con1);
		break;
#ifdef CONFIG_CPU_EXYNOS3250
	case BPLL:
		r = readl(&clk->bpll_con0);
		break;
	case UPLL:
		r = readl(&clk->upll_con0);
		break;
#endif
	default:
		printf("Unsupported PLL (%d)\n", pllreg);
		return 0;
	}

	/*
	 * APLL_CON: MIDV [25:16]
	 * MPLL_CON: MIDV [25:16]
	 * EPLL_CON: MIDV [24:16]
	 * VPLL_CON: MIDV [24:16]
	 */
	if (pllreg == APLL || pllreg == MPLL)
		mask = 0x3ff;
	else
		mask = 0x1ff;

	m = (r >> 16) & mask;

	/* PDIV [13:8] */
	p = (r >> 8) & 0x3f;
	/* SDIV [2:0] */
	s = r & 0x7;

	freq = CONFIG_SYS_CLK_FREQ;

	if (pllreg == EPLL) {
		k = k & 0xffff;
		/* FOUT = (MDIV + K / 65536) * FIN / (PDIV * 2^SDIV) */
		fout = (m + k / 65536) * (freq / (p * (1 << s)));
	} else if (pllreg == VPLL) {
		k = k & 0xfff;
		/* FOUT = (MDIV + K / 1024) * FIN / (PDIV * 2^SDIV) */
		fout = (m + k / 1024) * (freq / (p * (1 << s)));
	} else {
		if ((s5p_cpu_id == 0x4415) || (s5p_cpu_id == 0x3250)) {
			/* FOUT = MDIV * FIN / (PDIV * 2^(SDIV)) */
			fout = m * (freq / (p * (1 << s)));
		} else {
			if (s < 1)
				s = 1;
			/* FOUT = MDIV * FIN / (PDIV * 2^(SDIV - 1)) */
			fout = m * (freq / (p * (1 << (s - 1))));
		}
	}

	return fout;
}

/* exynos5: return pll clock frequency */
static unsigned long exynos5_get_pll_clk(int pllreg)
{
#ifdef CONFIG_CPU_EXYNOS5410
	struct exynos5410_clock *clk =
		(struct exynos5_clock *)samsung_get_base_clock();
#elif defined(CONFIG_CPU_EXYNOS5420)
	struct exynos5420_clock *clk =
		(struct exynos5420_clock *)samsung_get_base_clock();
#else
	struct exynos5_clock *clk =
		(struct exynos5_clock *)samsung_get_base_clock();
#endif
	unsigned long r, m, p, s, k = 0, mask, fout;
	unsigned int freq;
	unsigned int  clk_mux_stat_cdrex, mpll_fout_sel;

	switch (pllreg) {
	case APLL:
		r = readl(&clk->apll_con0);
		break;
	case MPLL:
		r = readl(&clk->mpll_con0);
		break;
	case EPLL:
		r = readl(&clk->epll_con0);
		k = readl(&clk->epll_con1);
		break;
	case VPLL:
		r = readl(&clk->vpll_con0);
		k = readl(&clk->vpll_con1);
		break;
#if defined(CONFIG_CPU_EXYNOS5410) || defined(CONFIG_CPU_EXYNOS5420)
	case KPLL:
		r = readl(&clk->kpll_con0);
		break;
	case BPLL:
		r = readl(&clk->bpll_con0);
		break;
	case CPLL:
		r = readl(&clk->cpll_con0);
		break;
	case SPLL:
		r = readl(&clk->spll_con0);
		break;
	case DPLL:
		r = readl(&clk->dpll_con0);
		break;
	case RPLL:
		r = readl(&clk->rpll_con0);
		k = readl(&clk->rpll_con1);
		break;
#endif
	default:
		printf("Unsupported PLL (%d)\n", pllreg);
		return 0;
	}

	/*
	 * APLL_CON: MIDV [25:16]
	 * MPLL_CON: MIDV [25:16]
	 * EPLL_CON: MIDV [24:16]
	 * VPLL_CON: MIDV [24:16]
	 */
	if (pllreg == APLL || pllreg == MPLL ||
			pllreg == KPLL || pllreg == BPLL ||
			pllreg == CPLL || pllreg == SPLL ||
			pllreg == DPLL)
		mask = 0x3ff;
	else
		mask = 0x1ff;

	m = (r >> 16) & mask;

	/* PDIV [13:8] */
	p = (r >> 8) & 0x3f;
	/* SDIV [2:0] */
	s = r & 0x7;

	freq = CONFIG_SYS_CLK_FREQ;

	if ((pllreg == EPLL) || (pllreg == RPLL)) {
		k = k & 0xffff;
		/* FOUT = (MDIV + K / 65536) * FIN / (PDIV * 2^SDIV) */
		fout = (m + k / 65536) * (freq / (p * (1 << s)));
	} else if (pllreg == VPLL) {
		k = k & 0xfff;
		/* FOUT = (MDIV + K / 1024) * FIN / (PDIV * 2^SDIV) */
		fout = (m + k / 1024) * (freq / (p * (1 << s)));
	} else {
#if defined(CONFIG_CPU_EXYNOS5410) || defined(CONFIG_CPU_EXYNOS5420)
		s += 1;
#else
		if (s < 1)
			s = 1;
#endif
		/* FOUT = MDIV * FIN / (PDIV * 2^(SDIV - 1)) */
		fout = m * (freq / (p * (1 << (s - 1))));

		/* Check mpll_fout_sel status for Rev1.0 */
		if ((pllreg == MPLL) && (s5p_cpu_id == 0x5250) &&
				(s5p_get_cpu_rev() >= 0x1)) {
			clk_mux_stat_cdrex = readl(&clk->mux_stat_cdrex);

			mpll_fout_sel = ( clk_mux_stat_cdrex >> 16 ) && 0x1;

			if(mpll_fout_sel)
				fout = fout >> 1;
		}
	}

	return fout;
}

static unsigned long exynos5260_get_pll_clk(int pllreg)
{
	struct exynos5260_clock_egl *clk_egl =
		(struct exynos5260_clock_egl *)samsung_get_base_clock_egl();
	struct exynos5260_clock_kfc *clk_kfc =
		(struct exynos5260_clock_kfc *)samsung_get_base_clock_kfc();
	struct exynos5260_clock_mif *clk_mif =
		(struct exynos5260_clock_mif *)samsung_get_base_clock_mif();
	unsigned long r, m, p, s, k = 0, mask, fout;
	unsigned int pll_ratio = 0;
	unsigned int freq;

	switch (pllreg) {
	case EGL_PLL:
		r = readl(&clk_egl->egl_pll_con0);
		break;
	case EGL_DPLL:
		r = readl(&clk_egl->egl_dpll_con0);
		break;
	case KFC_PLL:
		r = readl(&clk_kfc->kfc_pll_con0);
		break;
	case MEM_PLL:
		r = readl(&clk_mif->mem_pll_con0);
		pll_ratio = (readl(&clk_mif->clk_div_mif) >> 4) & 0x7;
		break;
	case BUS_PLL:
		r = readl(&clk_mif->bus_pll_con0);
		pll_ratio = (readl(&clk_mif->clk_div_mif) >> 8) & 0x7;
		break;
	case MEDIA_PLL:
		r = readl(&clk_mif->media_pll_con0);
		pll_ratio = readl(&clk_mif->clk_div_mif) & 0x7;
		break;
	default:
		printf("Unsupported PLL (%d)\n", pllreg);
		return 0;
	}

	/*
	 * PLL_CON: MIDV [25:16]
	 */
	mask = 0x3ff;
	m = (r >> 9) & mask;

	/* PDIV [13:8] */
	p = (r >> 3) & 0x3f;
	/* SDIV [2:0] */
	s = r & 0x7;

	freq = CONFIG_SYS_CLK_FREQ;

	/* FOUT = MDIV * FIN / (PDIV * 2^(SDIV)) */
	fout = m * (freq / (p * (1 << s)));
	fout = fout / ((unsigned long)pll_ratio + 1);

	return fout;
}

static unsigned long exynos5430_get_pll_clk(int pllreg)
{
	struct exynos5430_clock_egl *clk_egl =
		(struct exynos5430_clock_egl *)exynos5430_get_base_clock_egl();
	struct exynos5430_clock_kfc *clk_kfc =
		(struct exynos5430_clock_kfc *)exynos5430_get_base_clock_kfc();
	struct exynos5430_clock_mif *clk_mif =
		(struct exynos5430_clock_mif *)exynos5430_get_base_clock_mif();
	struct exynos5430_clock_top *clk_top =
		(struct exynos5430_clock_top *)exynos5430_get_base_clock_top();
	struct exynos5430_clock_cpif *clk_cpif =
				(struct exynos5430_clock_cpif *)
					    exynos5430_get_base_clock_cpif();
	unsigned long r, m, p, s, k = 0, mask, fout;
	unsigned int pll_ratio = 0;
	unsigned int bus_pll_sub_sel = 0;
	unsigned int freq;

	switch (pllreg) {
	case EGL_PLL:
		r = readl(&clk_egl->egl_pll_con0);
		break;
	case EGL_DPLL:
		r = readl(&clk_egl->egl_dpll_con0);
		break;
	case KFC_PLL:
		r = readl(&clk_kfc->kfc_pll_con0);
		break;
	case MEM_PLL:
		r = readl(&clk_mif->mem_pll_con0);
		pll_ratio = (readl(&clk_mif->clk_div_mif0) >> 4) & 0x3;
		break;
	case BUS_PLL:
		bus_pll_sub_sel = readl(&clk_mif->clk_mux_sel_mif1);
		if (bus_pll_sub_sel & 0x1) {
			r = readl(&clk_mif->bus_pll_con0);
			pll_ratio = readl(&clk_mif->clk_div_mif0) & 0x3;
		} else {
			r = readl(&clk_mif->bus_dpll_con0);
		}
		break;
	case MFC_PLL:
		r = readl(&clk_mif->mfc_pll_con0);
		pll_ratio = (readl(&clk_mif->clk_div_mif0) >> 8) & 0x3;
		break;
	case ISP_PLL:
		r = readl(&clk_top->isp_pll_con0);
		break;
	case MPHY_PLL:
		r = readl(&clk_cpif->mphy_pll_con0);
		break;
	default:
		printf("Unsupported PLL (%d)\n", pllreg);
		return 0;
	}

	/*
	 * PLL_CON: MIDV [25:16]
	 */
	mask = 0x3ff;
	m = (r >> 16) & mask;

	/* PDIV [13:8] */
	p = (r >> 8) & 0x3f;
	/* SDIV [2:0] */
	s = r & 0x7;

	freq = CONFIG_SYS_CLK_FREQ;

	/* FOUT = MDIV * FIN / (PDIV * 2^(SDIV)) */
	fout = m * (freq / (p * (1 << s)));
	fout = fout / ((unsigned long)pll_ratio + 1);

	return fout;
}

/* exynos4: return ARM clock frequency */
static unsigned long exynos4_get_arm_clk(void)
{
	struct exynos4_clock *clk =
		(struct exynos4_clock *)samsung_get_base_clock();
	unsigned long div;
	unsigned long armclk;
	unsigned int core_ratio;
	unsigned int core2_ratio;

	div = readl(&clk->div_cpu0);

	/* CORE_RATIO: [2:0], CORE2_RATIO: [30:28] */
	core_ratio = (div >> 0) & 0x7;
	core2_ratio = (div >> 28) & 0x7;

	armclk = get_pll_clk(APLL) / (core_ratio + 1);
	armclk /= (core2_ratio + 1);

	return armclk;
}

/* exynos5: return ARM clock frequency */
static unsigned long exynos5_get_arm_clk(void)
{
	struct exynos5_clock *clk =
		(struct exynos5_clock *)samsung_get_base_clock();
	unsigned long div;
	unsigned long armclk;
	unsigned int arm_ratio;
	unsigned int arm2_ratio;

	div = readl(&clk->div_cpu0);

	/* ARM_RATIO: [2:0], ARM2_RATIO: [30:28] */
	arm_ratio = (div >> 0) & 0x7;
	arm2_ratio = (div >> 28) & 0x7;

	armclk = get_pll_clk(APLL) / (arm_ratio + 1);
	armclk /= (arm2_ratio + 1);

	return armclk;
}

/* exynos5260: return ARM clock frequency */
static unsigned long exynos5260_get_arm_clk(void)
{
	struct exynos5260_clock_egl *clk =
		(struct exynos5260_clock_egl *)samsung_get_base_clock_egl();
	unsigned long div;
	unsigned long armclk;
	unsigned int egl1_ratio;
	unsigned int egl2_ratio;

	div = readl(&clk->clk_div_egl);

	/* EGL1_RATIO: [2:0], EGL2_RATIO: [6:4] */
	egl1_ratio = (div >> 0) & 0x7;
	egl2_ratio = (div >> 4) & 0x7;

	armclk = get_pll_clk(APLL) / (egl1_ratio + 1);
	armclk /= (egl2_ratio + 1);

	return armclk;
}

/* exynos5430: return ARM clock frequency */
static unsigned long exynos5430_get_arm_clk(void)
{
	struct exynos5430_clock_egl *clk =
		(struct exynos5430_clock_egl *)exynos5430_get_base_clock_egl();
	unsigned long div;
	unsigned long armclk;
	unsigned int egl1_ratio;
	unsigned int egl2_ratio;

	div = readl(&clk->clk_div_egl0);

	/* EGL1_RATIO: [2:0], EGL2_RATIO: [6:4] */
	egl1_ratio = (div >> 0) & 0x7;
	egl2_ratio = (div >> 4) & 0x7;

	armclk = get_pll_clk(APLL) / (unsigned long)(egl1_ratio + 1);
	armclk /= (unsigned long)(egl2_ratio + 1);

	return armclk;
}

/* exynos4: return pwm clock frequency */
static unsigned long exynos4_get_pwm_clk(void)
{
	struct exynos4_clock *clk =
		(struct exynos4_clock *)samsung_get_base_clock();
	unsigned long pclk, sclk;
	unsigned int sel;
	unsigned int ratio;

	if (s5p_cpu_id == 0x4415)
		return 100000000;
	else if (s5p_cpu_id == 0x3250)
		return 50000000;

	if (s5p_get_cpu_rev() == 0) {
		/*
		 * CLK_SRC_PERIL0
		 * PWM_SEL [27:24]
		 */
		sel = readl(&clk->src_peril0);
		sel = (sel >> 24) & 0xf;

		if (sel == 0x6)
			sclk = get_pll_clk(MPLL);
		else if (sel == 0x7)
			sclk = get_pll_clk(EPLL);
		else if (sel == 0x8)
			sclk = get_pll_clk(VPLL);
		else
			return 0;

		/*
		 * CLK_DIV_PERIL3
		 * PWM_RATIO [3:0]
		 */
		ratio = readl(&clk->div_peril3);
		ratio = ratio & 0xf;
	} else if (s5p_get_cpu_rev() >= 1) {
		sclk = get_pll_clk(MPLL);
		ratio = 8;
	} else
		return 0;

	pclk = sclk / (ratio + 1);

	return pclk;
}

/* exynos5: return pwm clock frequency */
static unsigned long exynos5_get_pwm_clk(void)
{
	struct exynos5_clock *clk =
		(struct exynos5_clock *)samsung_get_base_clock();
	unsigned long pclk, div_aclk_pre, div_aclk;
	unsigned int ratio;

	/*
	 * CLK_DIV_PERIC3
	 * PWM_RATIO [3:0]
	 */
	div_aclk_pre = readl(&clk->div_top1);
	div_aclk_pre = (div_aclk_pre >> 24)& 0x7;

	div_aclk = readl(&clk->div_top0);
	div_aclk = div_aclk& 0x7;

	pclk = (get_pll_clk(MPLL) / (div_aclk_pre + 1)) / (div_aclk +1);

	return pclk;
}

/* exynos5260: return pwm clock frequency */
static unsigned long exynos5260_get_pwm_clk(void)
{
	struct exynos5260_clock_top *clk =
		(struct exynos5260_clock_top *)samsung_get_base_clock_top();
	unsigned long pclk;
	unsigned int aclk_peri_66_ratio;

	/*
	 * CLK_DIV_TOP_PERI2[23:20]
	 * ACLK_PERI_66_RATIO
	 */
	aclk_peri_66_ratio = readl(&clk->clk_div_top_peri2);
	aclk_peri_66_ratio = (aclk_peri_66_ratio >> 20)& 0xf;

	pclk = get_pll_clk(BUS_PLL) / (aclk_peri_66_ratio + 1);

	return pclk;
}

/* exynos5430: return pwm clock frequency */
static unsigned long exynos5430_get_pwm_clk(void)
{
	struct exynos5430_clock_top *clk =
		(struct exynos5430_clock_top *)exynos5430_get_base_clock_top();
	unsigned long pclk;
	unsigned int clk_div_top3;
	unsigned int aclk_peri_66_ratio_a, aclk_peri_66_ratio_b;

	/*
	 * CLK_DIV_TOP3
	 * ACLK_PERI_66_RATIO_A[10:8], ACLK_PERI_66_RATIO_B[14:12]
	 */
	clk_div_top3 = readl(&clk->clk_div_top3);
	aclk_peri_66_ratio_a = ((clk_div_top3 >> 8) & 0x7) + 1;
	aclk_peri_66_ratio_b = ((clk_div_top3 >> 12) & 0x7) + 1;

	pclk = get_pll_clk(BUS_PLL) / (unsigned long)aclk_peri_66_ratio_a;
	pclk = pclk / (unsigned long)aclk_peri_66_ratio_b;

	return pclk;
}

/* exynos4: return uart clock frequency */
static unsigned long exynos4_get_uart_clk(int dev_index)
{
	struct exynos4_clock *clk =
		(struct exynos4_clock *)samsung_get_base_clock();
	unsigned long uclk, sclk;
	unsigned int sel;
	unsigned int ratio;
	unsigned int mpll_ratio_pre;

	/*
	 * CLK_SRC_PERIL0
	 * UART0_SEL [3:0]
	 * UART1_SEL [7:4]
	 * UART2_SEL [8:11]
	 * UART3_SEL [12:15]
	 * UART4_SEL [16:19]
	 * UART5_SEL [23:20]
	 */
	sel = readl(&clk->src_peril0);
	sel = (sel >> (dev_index << 2)) & 0xf;

	if (sel == 0x6) {
		sclk = get_pll_clk(MPLL);
#ifdef CONFIG_CPU_EXYNOS3250
		mpll_ratio_pre = (readl(&clk->div_top) >> 28) && 0x3;
		sclk = sclk / MPLL_PRE_DIV / (mpll_ratio_pre + 1);
#endif
	} else if (sel == 0x7)
		sclk = get_pll_clk(EPLL);
	else if (sel == 0x8)
		sclk = get_pll_clk(VPLL);
	else if ((sel == 0x0) ||(sel == 0x1))
		sclk = CONFIG_SYS_CLK_FREQ;
	else
		return 0;

	/*
	 * CLK_DIV_PERIL0
	 * UART0_RATIO [3:0]
	 * UART1_RATIO [7:4]
	 * UART2_RATIO [8:11]
	 * UART3_RATIO [12:15]
	 * UART4_RATIO [16:19]
	 * UART5_RATIO [23:20]
	 */
	ratio = readl(&clk->div_peril0);
	ratio = (ratio >> (dev_index << 2)) & 0xf;

	uclk = sclk / (ratio + 1);

	return uclk;
}

/* exynos5: return uart clock frequency */
static unsigned long exynos5_get_uart_clk(int dev_index)
{
	struct exynos5_clock *clk =
		(struct exynos5_clock *)samsung_get_base_clock();
	unsigned long uclk, sclk;
	unsigned int sel;
	unsigned int ratio;

	/*
	 * CLK_SRC_PERIC0
	 * UART0_SEL [3:0]
	 * UART1_SEL [7:4]
	 * UART2_SEL [8:11]
	 * UART3_SEL [12:15]
	 * UART4_SEL [16:19]
	 * UART5_SEL [23:20]
	 */
	sel = readl(&clk->src_peric0);
#if defined(CONFIG_CPU_EXYNOS5420)
	sel = (sel >> ((dev_index + 1) << 2)) & 0xf;
#else
	sel = (sel >> (dev_index << 2)) & 0xf;
#endif

#if defined(CONFIG_CPU_EXYNOS5420)
	if (sel == 0x3)
		sclk = get_pll_clk(MPLL);
	else if (sel == 0x6)
		sclk = get_pll_clk(EPLL);
	else if (sel == 0x1)
		sclk = get_pll_clk(CPLL);
	else
		return 0;
#else
	if (sel == 0x6)
		sclk = get_pll_clk(MPLL);
	else if (sel == 0x7)
		sclk = get_pll_clk(EPLL);
	else if (sel == 0x8)
		sclk = get_pll_clk(VPLL);
	else if (sel == 0x9)
		sclk = get_pll_clk(CPLL);
	else
		return 0;
#endif
	/*
	 * CLK_DIV_PERIC0
	 * UART0_RATIO [3:0]
	 * UART1_RATIO [7:4]
	 * UART2_RATIO [8:11]
	 * UART3_RATIO [12:15]
	 * UART4_RATIO [16:19]
	 * UART5_RATIO [23:20]
	 */
	ratio = readl(&clk->div_peric0);
#if defined(CONFIG_CPU_EXYNOS5420)
	ratio = (ratio >> ((dev_index + 2) << 2)) & 0xf;
#else
	ratio = (ratio >> (dev_index << 2)) & 0xf;
#endif

	uclk = sclk / (ratio + 1);

	return uclk;
}

/* exynos5260: return uart clock frequency */
static unsigned long exynos5260_get_uart_clk(int dev_index)
{
	struct exynos5260_clock_top *clk =
		(struct exynos5260_clock_top *)samsung_get_base_clock_top();
	unsigned long uclk, sclk;
	unsigned int sel;
	unsigned int ratio;

	/*
	 * CLK_MUX_SEL_TOP_PERI1
	 * - UART0_SEL [20], UART1_SEL [12], UART2_SEL [16]
	 *
	 * CLK_DIV_TOP_PERI1
	 * - UART0_RATIO [27:24], UART1_RATIO [19:16], UART2_RATIO [23:20]
	 */
	sel = readl(&clk->clk_mux_sel_top_peri1);
	ratio = readl(&clk->clk_div_top_peri1);

	switch (dev_index) {
	case 0:
		sel = (sel >> 20) & 0xf;
		ratio = (ratio >> 24) & 0xf;
		break;
	case 1:
		ratio = (ratio >> 16) & 0xf;
		sel = (sel >> 16) & 0xf;
		break;
	case 2:
		sel = (sel >> 12) & 0xf;
		ratio = (ratio >> 20) & 0xf;
		break;
	}

	if (sel) {
		sclk = get_pll_clk(BUS_PLL);
	} else {
		sclk = CONFIG_SYS_CLK_FREQ;
		return 0;
	}

	uclk = sclk / (ratio + 1);

	return uclk;
}

/* exynos5430: return uart clock frequency */
static unsigned long exynos5430_get_uart_clk(int dev_index)
{
	struct exynos5430_clock_top *clk =
		(struct exynos5430_clock_top *)exynos5430_get_base_clock_top();
	unsigned long uclk, sclk;
	unsigned int sel;
	unsigned int ratio;

	/*
	 * CLK_MUX_SEL_TOP_PERI1
	 * - UART0_SEL [20], UART1_SEL [12], UART2_SEL [16]
	 *
	 * CLK_DIV_TOP_PERI1
	 * - UART0_RATIO [27:24], UART1_RATIO [19:16], UART2_RATIO [23:20]
	 */
	sel = readl(&clk->clk_mux_sel_top_peric0);
	ratio = readl(&clk->clk_div_top_peric2);

	switch (dev_index) {
	case 0:
		sel = (sel >> 12) & 0x1;
		ratio = (ratio >> 0) & 0xf;
		break;
	case 1:
		sel = (sel >> 16) & 0x1;
		ratio = (ratio >> 4) & 0xf;
		break;
	case 2:
		sel = (sel >> 20) & 0x1;
		ratio = (ratio >> 8) & 0xf;
		break;
	}

	if (sel) {
		sclk = get_pll_clk(BUS_PLL);
	} else {
		sclk = CONFIG_SYS_CLK_FREQ;
	}

	uclk = sclk / (ratio + 1);

	return uclk;
}

/* exynos4: get the usbdrd clock */
static unsigned int exynos4_get_usbdrd_clk(void)
{
	/* TODO */
	return 0;
}

/* exynos5: get the usbdrd clock */
#if defined(CONFIG_CPU_EXYNOS5260)
static unsigned long exynos5_get_usbdrd_clk(void)
{
	struct exynos5260_clock_top *clk =
		(struct exynos5260_clock_top *)samsung_get_base_clock_top();
	unsigned int addr;
	unsigned int ratio;
	unsigned long sclk;

	sclk = 24000000;

	/*
	 * CLK_DIV_TOP_FSYS0
	 * SCLK_FSYS_USBDRD30_SUSPEND_CLK_RATIO[7:4]
	 */
	addr = (unsigned int)&clk->clk_div_top_fsys0;
	ratio = readl(addr);

	ratio = (ratio >> 4) & 0xff;

	return (sclk / (ratio + 1));
}
#else
static unsigned long exynos5_get_usbdrd_clk(void)
{
	struct exynos5_clock *clk =
		(struct exynos5_clock *)samsung_get_base_clock();
	unsigned int addr;
	unsigned int sel;
	unsigned int ratio;
	unsigned long sclk;

	sel = readl(&clk->src_fsys);
	sel = (sel >> 28) & 0xf;

#if defined(CONFIG_CPU_EXYNOS5410) || defined(CONFIG_CPU_EXYNOS5420)
	sclk = 24000000;
#else
	if (sel == 0x0)
		sclk = get_pll_clk(MPLL);
	else {
		printf("Can't get SCLK_CPLL\n");
		return 0;
	}
#endif

	/*
	 * CLK_DIV_FSYS0
	 * USBDRD30_RATIO[27:24], SATA_RATIO[23:20]
	 */
	addr = (unsigned int)&clk->div_fsys0;
	ratio = readl(addr);

	ratio = (ratio >> 24) & 0xff;

	return (sclk / (ratio + 1));
}
#endif
static unsigned long exynos5430_get_usbdrd_clk(void)
{
	struct exynos5430_clock_top *clk =
		(struct exynos5430_clock_top *)exynos5430_get_base_clock_top();
	unsigned int addr;
	unsigned int ratio;
	unsigned long sclk;

	sclk = CONFIG_SYS_CLK_FREQ;

	/*
	 * CLK_DIV_TOP_FSYS2
	 * SCLK_USBDRD30_RATIO[3:0]
	 */
	addr = (unsigned int)&clk->clk_div_top_fsys2;
	ratio = readl(addr);

	ratio = ratio & 0xff;

	return (sclk / (ratio + 1));
}

/* exynos4: set the mmc clock */
static void exynos4_set_mmc_clk(int dev_index, unsigned int div)
{
	struct exynos4_clock *clk =
		(struct exynos4_clock *)samsung_get_base_clock();
	unsigned int addr;
	unsigned int val;

	/*
	 * CLK_DIV_FSYS1
	 * MMC0_PRE_RATIO [15:8], MMC1_PRE_RATIO [31:24]
	 * MMC0_RATIO [3:0],      MMC1_RATIO [16:19]
	 * CLK_DIV_FSYS2
	 * MMC2_PRE_RATIO [15:8], MMC3_PRE_RATIO [31:24]
	 * MMC2_RATIO [3:0],      MMC3_RATIO [16:19]
	 * CLK_DIV_FSYS3
	 * MMC4_PRE_RATIO [15:8]
	 * MMC4_RATIO [3:0]
	 */
	if (dev_index < 2) {
		addr = (unsigned int)&clk->div_fsys1;
	} else if (2 <= dev_index && dev_index < 4) {
		addr = (unsigned int)&clk->div_fsys2;
		dev_index -= 2;
	} else {
		addr = (unsigned int)&clk->div_fsys3;
		dev_index = 0;
	}

	val = readl(addr);
	/* clear MMCx_PRE_RATIO */
	val &= ~(0xff << ((dev_index << 4) + 8));
	/* clear MMCx_RATIO */
	val &= ~(0xff << (dev_index << 4));
	val |= (div & 0xff) << ((dev_index << 4) + 8);
	writel(val, addr);
}

/* exynos5: set the mmc clock */
static void exynos5_set_mmc_clk(int dev_index, unsigned int div)
{
	struct exynos5_clock *clk =
		(struct exynos5_clock *)samsung_get_base_clock();
	unsigned int addr;
	unsigned int val;

	/*
	 * CLK_DIV_FSYS1
	 * MMC0_PRE_RATIO [15:8], MMC1_PRE_RATIO [31:24]
	 * MMC0_RATIO [3:0],	  MMC1_RATIO [16:19]
	 * CLK_DIV_FSYS2
	 * MMC2_PRE_RATIO [15:8], MMC3_PRE_RATIO [31:24]
	 * MMC2_RATIO [3:0],	  MMC3_RATIO [16:19]
	 */
#if defined(CONFIG_CPU_EXYNOS5420)
	addr = (unsigned int)&clk->div_fsys1;
#else
	if (dev_index < 2) {
		addr = (unsigned int)&clk->div_fsys1;
	} else {
		addr = (unsigned int)&clk->div_fsys2;
		dev_index -= 2;
	}
#endif

#if defined(CONFIG_CPU_EXYNOS5420)
	val = readl(addr);
	if (dev_index == 0) {
		val &= ~(0x3ff);
		val |= (div & 0x3ff);
	} else {
		val &= ~(((0x3ff) << dev_index * 10));
		val |= ((div & 0x3ff) << (dev_index * 10));
	}
	writel(val, addr);
#else
	val = readl(addr);
	/* clear MMCx_PRE_RATIO */
	val &= ~(0xff << ((dev_index << 4) + 8));
	/* clear MMCx_RATIO */
	val &= ~(0xff << (dev_index << 4));
	val |= (div & 0xff) << ((dev_index << 4) + 8);
	writel(val, addr);
#endif
}

/* exynos5260: set the mmc clock */
static void exynos5260_set_mmc_clk(int dev_index, unsigned int div)
{
	struct exynos5260_clock_top *clk =
		(struct exynos5260_clock_top *)samsung_get_base_clock_top();
	unsigned int addr;
	unsigned int ratio;

	/*
	 * CLK_DIV_TOP_FSYS0
	 * SCLK_FSYS_MMC0_SDCLKIN_B_RATIO[23:16]
	 * SCLK_FSYS_MMC0_SDCLKIN_A_RATIO[15:12]
	 * CLK_DIV_TOP_FSYS1
	 * SCLK_FSYS_MMC2_SDCLKIN_B_RATIO[23:16]
	 * SCLK_FSYS_MMC2_SDCLKIN_A_RATIO[15:12]
	 * SCLK_FSYS_MMC1_SDCLKIN_B_RATIO[11:4]
	 * SCLK_FSYS_MMC1_SDCLKIN_A_RATIO[3:0]
	 */
	if (dev_index == 0) {
		addr = (unsigned int)&clk->clk_div_top_fsys0;
		ratio = readl(addr);
		/* set SCLK_FSYS_MMC0_SDCLKIN_A_RATIO for div_1 */
		ratio &= ~(0xf << 12);
		/* set SCLK_FSYS_MMC0_SDCLKIN_B_RATIO */
		ratio &= ~(0xff << 16);
		ratio |= ((div & 0xff) << 16);
	} else {
		addr = (unsigned int)&clk->clk_div_top_fsys1;
		ratio = readl(addr);
		/* set SCLK_FSYS_MMCx_SDCLKIN_A_RATIO for div_1 */
		ratio &= ~(0xf << ((dev_index - 1) * 12));
		/* get SCLK_FSYS_MMCx_SDCLKIN_B_RATIO */
		ratio &= ~(0xff << (((dev_index - 1) * 12) + 4));
		ratio |= ((div & 0xff) << (((dev_index - 1) * 12) + 4));
	}

	writel(ratio, addr);
}

/* exynos5430: set the mmc clock */
static void exynos5430_set_mmc_clk(int dev_index, unsigned int div)
{
	struct exynos5430_clock_top *clk =
		(struct exynos5430_clock_top *)exynos5430_get_base_clock_top();
	unsigned int ratio, addr;

	/*
	 * CLK_DIV_TOP_FSYS0
	 * SCLK_MMC1_B_RATIO[23:16], SCLK_MMC1_A_RATIO[15:12]
	 * SCLK_MMC0_B_RATIO[11:4], SCLK_MMC0_A_RATIO[3:0]
	 * CLK_DIV_TOP_FSYS1
	 * SCLK_MMC2_B_RATIO[11:4], SCLK_MMC2_A_RATIO[3:0]
	 */
	if (dev_index < 2) {
		addr = (unsigned int)&clk->clk_div_top_fsys0;
		ratio = readl(addr);
		/* clear SCLK_MMCx_A_RATIO */
		ratio &= ~(0xf << (dev_index * 12));
		/* clear SCLK_MMCx_B_RATIO */
		ratio &= ~(0xff << ((dev_index * 12) + 4));
		/* set SCLK_MMCx_B_RATIO */
		ratio |= ((div & 0xff) << ((dev_index * 12) + 4));
	} else {
		addr = (unsigned int)&clk->clk_div_top_fsys1;
		ratio = readl(addr);
		/* clear SCLK_MMCx_A_RATIO */
		ratio &= ~(0xf);
		/* clear SCLK_MMCx_B_RATIO */
		ratio &= ~(0xff << 4);
		/* set SCLK_MMCx_B_RATIO */
		ratio |= ((div & 0xff) << 4);
	}

	writel(ratio, addr);
}

/* exynos4: get the mmc clock */
static unsigned int exynos4_get_mmc_clk(int dev_index)
{
	struct exynos4_clock *clk =
		(struct exynos4_clock *)samsung_get_base_clock();
	unsigned int addr;
	unsigned int sel;
	unsigned int pre_ratio, ratio;
	unsigned long sclk;
	unsigned int mpll_ratio_pre;

	sel = readl(&clk->src_fsys);
	sel = (sel >> (dev_index << 2)) & 0xf;

	if (sel == 0x6) {
		sclk = get_pll_clk(MPLL);
#ifdef CONFIG_CPU_EXYNOS3250
		mpll_ratio_pre = (readl(&clk->div_top) >> 28) && 0x3;
		sclk = sclk / MPLL_PRE_DIV / (mpll_ratio_pre + 1);
#endif
	} else if (sel == 0x7)
		sclk = get_pll_clk(EPLL);
	else if (sel == 0x8)
		sclk = get_pll_clk(VPLL);
	else
		return 0;

	/*
	 * CLK_DIV_FSYS1
	 * MMC0_PRE_RATIO [15:8], MMC1_PRE_RATIO [31:24]
	 * MMC0_RATIO [3:0],	  MMC1_RATIO [16:19]
	 * CLK_DIV_FSYS2
	 * MMC2_PRE_RATIO [15:8], MMC3_PRE_RATIO [31:24]
	 * MMC2_RATIO [3:0],	  MMC3_RATIO [16:19]
	 * CLK_DIV_FSYS3
	 * MMC4_PRE_RATIO [15:8]
	 * MMC4_RATIO [3:0]
	 */
	if (dev_index < 2) {
		addr = (unsigned int)&clk->div_fsys1;
	} else if (2 <= dev_index && dev_index < 4) {
		addr = (unsigned int)&clk->div_fsys2;
		dev_index -= 2;
	} else {
		addr = (unsigned int)&clk->div_fsys3;
		dev_index = 0;
	}

	ratio = readl(addr);
	/* get MMCx_PRE_RATIO */
	pre_ratio = (ratio >> ((dev_index << 4) + 8)) & 0xff;
	/* get MMCx_RATIO */
	ratio = (ratio >> (dev_index << 4)) & 0xff;

	return (sclk / (pre_ratio + 1)) / (ratio + 1);
}

/* exynos5: get the mmc clock */
static unsigned long exynos5_get_mmc_clk(int dev_index)
{
	struct exynos5_clock *clk =
		(struct exynos5_clock *)samsung_get_base_clock();
	unsigned int addr;
	unsigned int sel;
	unsigned int pre_ratio, ratio;
	unsigned long sclk;

	sel = readl(&clk->src_fsys);
#if defined(CONFIG_CPU_EXYNOS5420)
	sel = (sel >> ((dev_index + 2) << 2)) & 0xf;
#else
	sel = (sel >> (dev_index << 2)) & 0xf;
#endif

#if defined(CONFIG_CPU_EXYNOS5420)
	if (sel == 0x3)
		sclk = get_pll_clk(MPLL);
	else if (sel == 0x4)
		sclk = get_pll_clk(SPLL);
	else if (sel == 0x6)
		sclk = get_pll_clk(EPLL);
	else if (sel == 0x1)
		sclk = get_pll_clk(CPLL);
	else
		return 0;
#else
	if (sel == 0x6)
		sclk = get_pll_clk(MPLL);
	else if (sel == 0x7)
		sclk = get_pll_clk(EPLL);
	else if (sel == 0x8)
		sclk = get_pll_clk(VPLL);
	else
		return 0;
#endif

	/*
	 * CLK_DIV_FSYS1
	 * MMC0_PRE_RATIO [15:8], MMC1_PRE_RATIO [31:24]
	 * MMC0_RATIO [3:0],	  MMC1_RATIO [16:19]
	 * CLK_DIV_FSYS2
	 * MMC2_PRE_RATIO [15:8], MMC3_PRE_RATIO [31:24]
	 * MMC2_RATIO [3:0],	  MMC3_RATIO [16:19]
	 */
#if defined(CONFIG_CPU_EXYNOS5420)
	addr = (unsigned int)&clk->div_fsys1;
#else
	if (dev_index < 2) {
		addr = (unsigned int)&clk->div_fsys1;
	} else {
		addr = (unsigned int)&clk->div_fsys2;
		dev_index -= 2;
	}
#endif

	ratio = readl(addr);
#if defined(CONFIG_CPU_EXYNOS5420)
	if (dev_index == 0)
		ratio = ratio & 0x3ff;
	else
		ratio = (ratio >> (dev_index * 10)) & 0x3ff;

	return sclk / (ratio + 1);
#else
	/* get MMCx_PRE_RATIO */
	pre_ratio = (ratio >> ((dev_index << 4) + 8)) & 0xff;
	/* get MMCx_RATIO */
	ratio = (ratio >> (dev_index << 4)) & 0xff;

	return (sclk / (pre_ratio + 1)) / (ratio + 1);
#endif
}

/* exynos5260: get the mmc clock */
static unsigned long exynos5260_get_mmc_clk(int dev_index)
{
	struct exynos5260_clock_top *clk =
		(struct exynos5260_clock_top *)samsung_get_base_clock_top();
	unsigned int sel, sel_a, sel_b;
	unsigned int ratio, ratio_a, ratio_b;
	unsigned long sclk;

	sel = readl(&clk->clk_mux_sel_top_fsys);
	sel_b = (sel >> (24 - (dev_index * 8))) & 0x1;

	if (sel_b) {
		sclk = get_pll_clk(MEDIA_PLL);
	} else {
		sel_a = (sel >> (20 - (dev_index * 8))) & 0x1;
		if (sel_a) {
			sclk = get_pll_clk(BUS_PLL);
		} else {
			sclk = CONFIG_SYS_CLK_FREQ;
		}
	}

	/*
	 * CLK_DIV_TOP_FSYS0
	 * SCLK_FSYS_MMC0_SDCLKIN_B_RATIO[23:16]
	 * SCLK_FSYS_MMC0_SDCLKIN_A_RATIO[15:12]
	 * CLK_DIV_TOP_FSYS1
	 * SCLK_FSYS_MMC2_SDCLKIN_B_RATIO[23:16]
	 * SCLK_FSYS_MMC2_SDCLKIN_A_RATIO[15:12]
	 * SCLK_FSYS_MMC1_SDCLKIN_B_RATIO[11:4]
	 * SCLK_FSYS_MMC1_SDCLKIN_A_RATIO[3:0]
	 */
	if (dev_index == 0) {
		ratio = readl(&clk->clk_div_top_fsys0);
		/* get SCLK_FSYS_MMC0_SDCLKIN_A_RATIO */
		ratio_a = (ratio >> 12) & 0xf;
		/* get SCLK_FSYS_MMC0_SDCLKIN_B_RATIO */
		ratio_b = (ratio >> 16) & 0xff;
	} else {
		ratio = readl(&clk->clk_div_top_fsys1);
		/* get SCLK_FSYS_MMCx_SDCLKIN_A_RATIO */
		ratio_a = (ratio >> ((dev_index - 1) * 12)) & 0xf;
		/* get SCLK_FSYS_MMCx_SDCLKIN_B_RATIO */
		ratio_b = (ratio >> (((dev_index - 1) * 12) + 4)) & 0xff;
	}

	return (sclk / (ratio_a + 1)) / (ratio_b + 1);
}

/* exynos5430: get the mmc clock */
#define MMC0_SRC_MASK		0x1111
#define MMCx_SRC_MASK		0x0011
#define MMC_SRC_ISP_PLL		0x1000
#define MMC_SRC_MPHY_PLL_USER	0x0100
#define MMC_SRC_MFC_PLL_USER	0x0010
#define MMC_SRC_BUS_PLL_USER	0x0001

static unsigned long exynos5430_get_mmc_clk(int dev_index)
{
	struct exynos5430_clock_top *clk =
		(struct exynos5430_clock_top *)exynos5430_get_base_clock_top();
	unsigned int sel, sel_mmc;
	unsigned int ratio, ratio_a, ratio_b;
	unsigned long sclk;

	sel = readl(&clk->clk_mux_sel_top_fsys0);

	if (dev_index == 0) {
		sel_mmc = sel & MMC0_SRC_MASK;
	} else {
		sel_mmc = (sel >> (24 - ((2 - dev_index) * 8))) & MMCx_SRC_MASK;
	}

	/* get src pll clock */
	if (sel_mmc & MMC_SRC_ISP_PLL) {
		sclk = get_pll_clk(ISP_PLL);
	} else if(sel_mmc & MMC_SRC_MPHY_PLL_USER) {
		sclk = get_pll_clk(MPHY_PLL);
	} else if(sel_mmc & MMC_SRC_MFC_PLL_USER) {
		sclk = get_pll_clk(MFC_PLL);
	} else if(sel_mmc & MMC_SRC_BUS_PLL_USER) {
		sclk = get_pll_clk(BUS_PLL);
	} else {
		sclk = CONFIG_SYS_CLK_FREQ;
	}

	/*
	 * CLK_DIV_TOP_FSYS0
	 * SCLK_MMC1_B_RATIO[23:16], SCLK_MMC1_A_RATIO[15:12]
	 * SCLK_MMC0_B_RATIO[11:4], SCLK_MMC0_A_RATIO[3:0]
	 * CLK_DIV_TOP_FSYS1
	 * SCLK_MMC2_B_RATIO[11:4], SCLK_MMC2_A_RATIO[3:0]
	 */
	if (dev_index < 2) {
		ratio = readl(&clk->clk_div_top_fsys0);
		/* get SCLK_MMCx_A_RATIO */
		ratio_a = (ratio >> (dev_index * 12)) & 0xf;
		/* get SCLK_MMCx_B_RATIO */
		ratio_b = (ratio >> ((dev_index * 12) + 4)) & 0xff;
	} else {
		ratio = readl(&clk->clk_div_top_fsys1);
		/* get SCLK_MMCx_A_RATIO */
		ratio_a = (ratio >> 0) & 0xf;
		/* get SCLK_MMCx_B_RATIO */
		ratio_b = (ratio >> 4) & 0xff;
	}

	return (sclk / (ratio_a + 1)) / (ratio_b + 1);
}

/* get_lcd_clk: return lcd clock frequency */
static unsigned long exynos4_get_lcd_clk(void)
{
	struct exynos4_clock *clk =
		(struct exynos4_clock *)samsung_get_base_clock();
	unsigned long pclk, sclk;
	unsigned int sel;
	unsigned int ratio;

	/*
	 * CLK_SRC_LCD0
	 * FIMD0_SEL [3:0]
	 */
	sel = readl(&clk->src_lcd0);
	sel = sel & 0xf;

	/*
	 * 0x6: SCLK_MPLL * 3250 : SCLK_MPLL_PRE_DIV (MPLL/2)
	 * 0x7: SCLK_EPLL
	 * 0x8: SCLK_VPLL
	 */
	if (sel == 0x6)
	{
#ifdef CONFIG_CPU_EXYNOS3250
		sclk = get_pll_clk(MPLL)/2;
#else
		sclk = get_pll_clk(MPLL);
#endif
	}
	else if (sel == 0x7)
		sclk = get_pll_clk(EPLL);
	else if (sel == 0x8)
		sclk = get_pll_clk(VPLL);
	else
		return 0;

	/*
	 * CLK_DIV_LCD0
	 * FIMD0_RATIO [3:0]
	 */
	ratio = readl(&clk->div_lcd0);
	ratio = ratio & 0xf;

	pclk = sclk / (ratio + 1);

	return pclk;
}

void exynos4_set_lcd_clk(void)
{
	struct exynos4_clock *clk =
	    (struct exynos4_clock *)samsung_get_base_clock();
	unsigned int cfg = 0;

	/*
	 * CLK_GATE_BLOCK
	 * CLK_CAM	[0]
	 * CLK_TV	[1]
	 * CLK_MFC	[2]
	 * CLK_G3D	[3]
	 * CLK_LCD0	[4]
	 * CLK_LCD1	[5]
	 * CLK_GPS	[7]
	 */
	cfg = readl(&clk->gate_block);
	cfg |= 1 << 4;
	writel(cfg, &clk->gate_block);

	/*
	 * CLK_SRC_LCD0
	 * FIMD0_SEL		[3:0]
	 * MDNIE0_SEL		[7:4]
	 * MDNIE_PWM0_SEL	[8:11]
	 * MIPI0_SEL		[12:15]
	 * set lcd0 src clock 0x6: SCLK_MPLL
	 */
	cfg = readl(&clk->src_lcd0);
	cfg &= ~(0xf);
	cfg |= 0x6;
	writel(cfg, &clk->src_lcd0);

	/*
	 * CLK_GATE_IP_LCD0
	 * CLK_FIMD0		[0]
	 * CLK_MIE0		[1]
	 * CLK_MDNIE0		[2]
	 * CLK_DSIM0		[3]
	 * CLK_SMMUFIMD0	[4]
	 * CLK_PPMULCD0		[5]
	 * Gating all clocks for FIMD0
	 */
	cfg = readl(&clk->gate_ip_lcd0);
	cfg |= 1 << 0;
	writel(cfg, &clk->gate_ip_lcd0);

	/*
	 * CLK_DIV_LCD0
	 * FIMD0_RATIO		[3:0]
	 * MDNIE0_RATIO		[7:4]
	 * MDNIE_PWM0_RATIO	[11:8]
	 * MDNIE_PWM_PRE_RATIO	[15:12]
	 * MIPI0_RATIO		[19:16]
	 * MIPI0_PRE_RATIO	[23:20]
	 * set fimd ratio
	 */
	cfg &= ~(0xf);
#ifdef CONFIG_SHIRI_LCD
	cfg |= 0xf;
#else
	cfg |= 0x1;
#endif
	writel(cfg, &clk->div_lcd0);
}

void exynos4_set_mipi_clk(void)
{
	struct exynos4_clock *clk =
	    (struct exynos4_clock *)samsung_get_base_clock();
	unsigned int cfg = 0;

	/*
	 * CLK_SRC_LCD0
	 * FIMD0_SEL		[3:0]
	 * MDNIE0_SEL		[7:4]
	 * MDNIE_PWM0_SEL	[8:11]
	 * MIPI0_SEL		[12:15]
	 * set mipi0 src clock 0x6: SCLK_MPLL
	 */
	cfg = readl(&clk->src_lcd0);
	cfg &= ~(0xf << 12);
	cfg |= (0x6 << 12);
	writel(cfg, &clk->src_lcd0);

	/*
	 * CLK_SRC_MASK_LCD0
	 * FIMD0_MASK		[0]
	 * MDNIE0_MASK		[4]
	 * MDNIE_PWM0_MASK	[8]
	 * MIPI0_MASK		[12]
	 * set src mask mipi0 0x1: Unmask
	 */
	cfg = readl(&clk->src_mask_lcd0);
	cfg |= (0x1 << 12);
	writel(cfg, &clk->src_mask_lcd0);

	/*
	 * CLK_GATE_IP_LCD0
	 * CLK_FIMD0		[0]
	 * CLK_MIE0		[1]
	 * CLK_MDNIE0		[2]
	 * CLK_DSIM0		[3]
	 * CLK_SMMUFIMD0	[4]
	 * CLK_PPMULCD0		[5]
	 * Gating all clocks for MIPI0
	 */
	cfg = readl(&clk->gate_ip_lcd0);
	cfg |= 1 << 3;
	writel(cfg, &clk->gate_ip_lcd0);

	/*
	 * CLK_DIV_LCD0
	 * FIMD0_RATIO		[3:0]
	 * MDNIE0_RATIO		[7:4]
	 * MDNIE_PWM0_RATIO	[11:8]
	 * MDNIE_PWM_PRE_RATIO	[15:12]
	 * MIPI0_RATIO		[19:16]
	 * MIPI0_PRE_RATIO	[23:20]
	 * set mipi ratio
	 */
	cfg &= ~(0xf << 16);
#ifdef CONFIG_SHIRI_LCD
	cfg |= (0x0 << 16);
#else
	cfg |= (0x1 << 16);
#endif
	writel(cfg, &clk->div_lcd0);
}

/**
 * Linearly searches for the most accurate main and fine stage clock scalars
 * (divisors) for a specified target frequency and scalar bit sizes by checking
 * all multiples of main_scalar_bits values. Will always return scalars up to or
 * slower than target.
 *
 * @param main_scalar_bits	Number of main scalar bits, must be > 0 and < 32
 * @param fine_scalar_bits	Number of fine scalar bits, must be > 0 and < 32
 * @param input_freq		Clock frequency to be scaled in Hz
 * @param target_freq		Desired clock frequency in Hz
 * @param best_fine_scalar	Pointer to store the fine stage divisor
 *
 * @return best_main_scalar	Main scalar for desired frequency or -1 if none
 * found
 */
static int clock_calc_best_scalar(unsigned int main_scaler_bits,
	unsigned int fine_scalar_bits, unsigned int input_rate,
	unsigned int target_rate, unsigned int *best_fine_scalar)
{
	int i;
	int best_main_scalar = -1;
	unsigned int best_error = target_rate;
	const unsigned int cap = (1 << fine_scalar_bits) - 1;
	const unsigned int loops = 1 << main_scaler_bits;

	debug("Input Rate is %u, Target is %u, Cap is %u\n", input_rate,
			target_rate, cap);

	assert(best_fine_scalar != NULL);
	assert(main_scaler_bits <= fine_scalar_bits);

	*best_fine_scalar = 1;

	if (input_rate == 0 || target_rate == 0)
		return -1;

	if (target_rate >= input_rate)
		return 1;

	for (i = 1; i <= loops; i++) {
		const unsigned int effective_div = max(min(input_rate / i /
							target_rate, cap), 1);
		const unsigned int effective_rate = input_rate / i /
							effective_div;
		const int error = target_rate - effective_rate;

		debug("%d|effdiv:%u, effrate:%u, error:%d\n", i, effective_div,
				effective_rate, error);

		if (error >= 0 && error <= best_error) {
			best_error = error;
			best_main_scalar = i;
			*best_fine_scalar = effective_div;
		}
	}

	return best_main_scalar;
}

static int exynos5_set_spi_clk(enum periph_id periph_id,
					unsigned int rate)
{
	struct exynos5_clock *clk =
		(struct exynos5_clock *)samsung_get_base_clock();
	int main;
	unsigned int fine;
	unsigned shift, pre_shift;
	unsigned mask = 0xff;
	u32 *reg;

	main = clock_calc_best_scalar(4, 8, 400000000, rate, &fine);
	if (main < 0) {
		debug("%s: Cannot set clock rate for periph %d",
				__func__, periph_id);
		return -1;
	}
	main = main - 1;
	fine = fine - 1;

	switch (periph_id) {
	case PERIPH_ID_SPI0:
		reg = &clk->div_peric1;
		shift = 0;
		pre_shift = 8;
		break;
	case PERIPH_ID_SPI1:
		reg = &clk->div_peric1;
		shift = 16;
		pre_shift = 24;
		break;
	case PERIPH_ID_SPI2:
		reg = &clk->div_peric2;
		shift = 0;
		pre_shift = 8;
		break;
	case PERIPH_ID_SPI3:
		reg = &clk->sclk_div_isp;
		shift = 0;
		pre_shift = 4;
		break;
	case PERIPH_ID_SPI4:
		reg = &clk->sclk_div_isp;
		shift = 12;
		pre_shift = 16;
		break;
	default:
		debug("%s: Unsupported peripheral ID %d\n", __func__,
		      periph_id);
		return -1;
	}
	clrsetbits_le32(reg, mask << shift, (main & mask) << shift);
	clrsetbits_le32(reg, mask << pre_shift, (fine & mask) << pre_shift);

	return 0;
}

unsigned long get_pll_clk(int pllreg)
{
	if (cpu_is_exynos5()) {
		if (s5p_cpu_id == 0x5260) {
			return exynos5260_get_pll_clk(pllreg);
		} else if(s5p_cpu_id == 0x5430) {
			return exynos5430_get_pll_clk(pllreg);
		} else {
			return exynos5_get_pll_clk(pllreg);
		}
	} else
		return exynos4_get_pll_clk(pllreg);
}

unsigned long get_arm_clk(void)
{
	if (cpu_is_exynos5()) {
		if (s5p_cpu_id == 0x5260) {
			return exynos5260_get_arm_clk();
		} else if(s5p_cpu_id == 0x5430) {
			return exynos5430_get_arm_clk();
		} else {
			return exynos5_get_arm_clk();
		}
	} else
		return exynos4_get_arm_clk();
}

unsigned long get_pwm_clk(void)
{
	if (cpu_is_exynos5()) {
		if (s5p_cpu_id == 0x5260) {
			return exynos5260_get_pwm_clk();
		} else if(s5p_cpu_id == 0x5430) {
			return exynos5430_get_pwm_clk();
		} else {
			return exynos5_get_pwm_clk();
		}
	} else
		return exynos4_get_pwm_clk();
}

unsigned long get_uart_clk(int dev_index)
{
	if (cpu_is_exynos5()) {
		if (s5p_cpu_id == 0x5260) {
			return exynos5260_get_uart_clk(dev_index);
		} else if(s5p_cpu_id == 0x5430) {
			return exynos5430_get_uart_clk(dev_index);
		} else {
			return exynos5_get_uart_clk(dev_index);
		}
	} else
		return exynos4_get_uart_clk(dev_index);
}

unsigned long get_usbdrd_clk(void)
{
	if (cpu_is_exynos5())
		if(s5p_cpu_id == 0x5430) {
			return exynos5430_get_usbdrd_clk();
		} else {
			return exynos5_get_usbdrd_clk();
		}
	else
		return exynos4_get_usbdrd_clk();
}

void set_mmc_clk(int dev_index, unsigned int div)
{
	if (cpu_is_exynos5()) {
		if (s5p_cpu_id == 0x5260) {
			exynos5260_set_mmc_clk(dev_index, div);
		} else if(s5p_cpu_id == 0x5430) {
			exynos5430_set_mmc_clk(dev_index, div);
		} else {
			exynos5_set_mmc_clk(dev_index, div);
		}
	} else
		exynos4_set_mmc_clk(dev_index, div);
}

unsigned long get_mmc_clk(int dev_index)
{
	if (cpu_is_exynos5()) {
		if (s5p_cpu_id == 0x5260) {
			return exynos5260_get_mmc_clk(dev_index);
		} else if(s5p_cpu_id == 0x5430) {
			return exynos5430_get_mmc_clk(dev_index);
		} else {
			return exynos5_get_mmc_clk(dev_index);
		}
	} else
		return exynos4_get_mmc_clk(dev_index);
}

unsigned long get_lcd_clk(void)
{
	if (cpu_is_exynos4())
		return exynos4_get_lcd_clk();
	else
		return 0;
}

void set_lcd_clk(void)
{
	if (cpu_is_exynos4())
		exynos4_set_lcd_clk();
}

void set_mipi_clk(void)
{
	if (cpu_is_exynos4())
		exynos4_set_mipi_clk();
}

int set_spi_clk(int periph_id, unsigned int rate)
{
	if (cpu_is_exynos5())
		return exynos5_set_spi_clk(periph_id, rate);
	else
		return 0;
}
