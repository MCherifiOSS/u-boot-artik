/*
 * (C) Copyright 2015 Samsung Electronics Co. Ltd
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 */

#include <common.h>
#include <asm/arch/cpu.h>
#include <asm/io.h>
#include <asm/processor.h>

#define EXYNOS_TMU_COUNT		5
#define EXYNOS_TMU_REG_TRIMINFO		0x0
#define EXYNOS_TMU_REG_CONTROL		0x20
#define EXYNOS_TMU_REG_STATUS		0x28
#define EXYNOS_TMU_REG_CURRENT_TEMP	0x40
#define MAX_TEMP	125
#define MIN_TEMP	15

#define EXYNOS_TMU_CONTROL_CORE_EN	1

struct exynos_tmu_data {
	int id;
	unsigned long reg_base;
	unsigned int efuse;
} tmu_data[EXYNOS_TMU_COUNT];

inline unsigned long exynos_tmu_base(int id)
{
	if (id == 4)
		return 0x100A0000;
	else
		return 0x10060000 + (0x4000 * id);
}

unsigned int tmu_code_to_temp(unsigned char temp_code, int efuse)
{
	int temp;

	temp = temp_code - efuse + 25;

	/* temperature should range between minimum and maximum */
	if (temp > MAX_TEMP)
		temp = MAX_TEMP;
	else if (temp < MIN_TEMP)
		temp = MIN_TEMP;

	return temp;
}

static int exynos_tmu_read(struct exynos_tmu_data *data)
{
	unsigned char temp_code;
	int temp;

	temp_code = readl(data->reg_base + EXYNOS_TMU_REG_CURRENT_TEMP);
	temp = tmu_code_to_temp(temp_code, data->efuse);

	return temp;
}

int exynos_tmu_read_max_temp(void)
{
	int i;
	unsigned int temp, max = 0;

	for (i = 0; i < EXYNOS_TMU_COUNT; i++) {
		temp = exynos_tmu_read(&tmu_data[i]);
		if (temp > max)
			max = temp;
	}

	return max;
}

static int exynos_tmu_init(struct exynos_tmu_data *data, int id, int on)
{
	int timeout = 20000;
	unsigned int status, con;

	data->id = id;
	data->reg_base = exynos_tmu_base(id);
	data->efuse = readb(data->reg_base + EXYNOS_TMU_REG_TRIMINFO);

	while (1) {
		status = readb(data->reg_base + EXYNOS_TMU_REG_STATUS);
		if (status)
			break;

		timeout--;
		if (!timeout) {
			printf("%s: timeout TMU %d busy\n", __func__, id);
			return -1;
		}

		udelay(10);
	};

	con = readl(data->reg_base + EXYNOS_TMU_REG_CONTROL);
	if (on)
		con |= EXYNOS_TMU_CONTROL_CORE_EN;
	else
		con &= ~EXYNOS_TMU_CONTROL_CORE_EN;
	writel(con, data->reg_base + EXYNOS_TMU_REG_CONTROL);

	mdelay(10);

	return 0;
}

int exynos_tmu_probe(void)
{
	int i;
	for (i = 0; i < EXYNOS_TMU_COUNT; i++) {
		if (exynos_tmu_init(&tmu_data[i], i, 1))
			return -1;
	}

	return 0;
}

void exynos_tmu_shutdown(void)
{
	int i;
	for (i = 0; i < EXYNOS_TMU_COUNT; i++) {
		if (exynos_tmu_init(&tmu_data[i], i, 0))
			return -1;
	}
}
