/*
 * (C) Copyright 2011 Samsung Electronics Co. Ltd
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 */

#ifndef _EXYNOS5422_CPU_H
#define _EXYNOS5422_CPU_H


#define EXYNOS5422_POWER_BASE           0x10040000

/*
 * POWER
 */
#define ELFIN_POWER_BASE                EXYNOS5422_POWER_BASE

#define SW_RST_REG_OFFSET				0x400
#define SW_RST_REG_STAT_OFFSET			0x404
#define SW_RST_REG						__REG(EXYNOS5422_POWER_BASE + SW_RST_REG_OFFSET)
#define SW_RST_STAT_REG 				__REG(EXYNOS5422_POWER_BASE + SW_RST_REG_STAT_OFFSET)

/* Define Reset Status */
#define SOFTWARE_RESET	11
#define WARM_RESET		10
#define PINRESET_RESET	8

#endif	/* _EXYNOS5422_CPU_H */
