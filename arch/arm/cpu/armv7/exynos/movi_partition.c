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

#include <common.h>
#include <asm/arch/movi_partition.h>
#include <asm/arch/cpu.h>

#ifdef DEBUG_MOVI_PARTITION
#define dbg(x...)       printf(x)
#else
#define dbg(x...)       do { } while (0)
#endif

raw_area_t raw_area_control;

int init_raw_area_table(block_dev_desc_t * dev_desc, int location)
{
	int i;
	int part_num = 0;
	member_t *image;

	/* init raw_area will be 16MB */
	raw_area_control.start_blk = 16*1024*1024/MOVI_BLKSIZE;
	raw_area_control.next_raw_area = 0;
	strcpy(raw_area_control.description, "initial raw table");

	image = raw_area_control.image;

	/* For eMMC partition BLOCK Change*/

	/* image 0 should be fwbl1 */
	image[part_num].start_blk = location;
	image[part_num].used_blk = MOVI_BL1_BLKCNT;
	image[part_num].size = PART_SIZE_BL1;
	image[part_num].attribute = 0x0;
	strcpy(image[part_num].description, "bl1");
	dbg("fwbl1: %d\n", image[part_num].start_blk);
	part_num++;

	/* image 1 should be bl2 */
	image[part_num].start_blk = image[part_num - 1].start_blk + MOVI_BL1_BLKCNT;
	image[part_num].used_blk = MOVI_BL2_BLKCNT;
	image[part_num].size = PART_SIZE_BL2;
	image[part_num].attribute = 0x1;
	strcpy(image[part_num].description, "bl2");
	dbg("bl2: %d\n", image[part_num].start_blk);
	part_num++;

	/* image 2 should be u-boot */
	image[part_num].start_blk = image[part_num - 1].start_blk + MOVI_BL2_BLKCNT;
	image[part_num].used_blk = MOVI_UBOOT_BLKCNT;
	image[part_num].size = PART_SIZE_UBOOT;
	image[part_num].attribute = 0x2;
	strcpy(image[part_num].description, "u-boot");
	dbg("u-boot: %d\n", image[part_num].start_blk);
	part_num++;

	/* image 3 should be TrustZone S/W */
	image[part_num].start_blk = image[part_num - 1].start_blk + MOVI_UBOOT_BLKCNT;
	image[part_num].used_blk = MOVI_TZSW_BLKCNT;
	image[part_num].size = PART_SIZE_TZSW;
	image[part_num].attribute = 0x3;
	strcpy(image[part_num].description, "TrustZone S/W");
	dbg("TrustZone S/W: %d\n", image[part_num].start_blk);
	part_num++;

	/* image 4 should be environment */
	image[part_num].start_blk = image[part_num - 1].start_blk + MOVI_TZSW_BLKCNT;
	image[part_num].used_blk = MOVI_ENV_BLKCNT;
	image[part_num].size = CONFIG_ENV_SIZE;
	image[part_num].attribute = 0x4;
	strcpy(image[part_num].description, "environment");
	dbg("env: %d\n", image[part_num].start_blk);
	part_num++;

	for (i = part_num; i < 15; i++) {
		raw_area_control.image[i].start_blk = 0;
		raw_area_control.image[i].used_blk = 0;
	}
	return 0;
}

