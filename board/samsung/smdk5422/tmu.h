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

#ifndef __TMU_H__
#define __TMU_H__

int exynos_tmu_read(int id);
int exynos_tmu_read_max_temp(void);
int exynos_tmu_probe(void);
void exynos_tmu_shutdown(void);

#endif
