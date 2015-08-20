/*
 * Copyright (C) 2010 Samsung Electronics Co. Ltd
 *
 * Many parts of this program were copied from the work of Windriver.
 * Major modifications are as follows:
 * - Adding default partition table.
 * - Supporting OneNAND device.
 * - Supporting SDMMC device.
 * - Adding new command, sdfuse.
 * - Removing Lock scheme.
 * - Removing direct flash operations because they are implemented at others.
 * - Fixing several bugs
 * This program is under the same License with the their work.
 *
 * This is their Copyright:
 *
 * (C) Copyright 2008 - 2009
 * Windriver, <www.windriver.com>
 * Tom Rix <Tom.Rix@windriver.com>
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
 * Part of the rx_handler were copied from the Android project
 * Specifically rx command parsing in the  usb_rx_data_complete
 * function of the file bootable/bootloader/legacy/usbloader/usbloader.c
 *
 * The logical naming of flash comes from the Android project
 * Thse structures and functions that look like fastboot_flash_*
 * They come from bootable/bootloader/legacy/libboot/flash.c
 *
 * This is their Copyright:
 *
 * Copyright (C) 2008 The Android Open Source Project
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *  * Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */
#include <asm/byteorder.h>
#include <common.h>
#include <command.h>
#include <asm/arch/movi_partition.h>
#include <fastboot.h>
#if defined(CFG_FASTBOOT_SDMMCBSP)
#include <mmc.h>
#endif
#include <decompress_ext4.h>

#if defined(CONFIG_S5P6450)
DECLARE_GLOBAL_DATA_PTR;
#endif

#if defined(CONFIG_EXYNOS4X12)
#define OmPin	readl(EXYNOS4_POWER_BASE + INFORM3_OFFSET)
#elif defined(CONFIG_CPU_EXYNOS4415) || defined(CONFIG_CPU_EXYNOS3250)
#define OmPin	readl(EXYNOS4_POWER_BASE + INFORM3_OFFSET)
#elif defined(CONFIG_CPU_EXYNOS5260)
#define OmPin	readl(EXYNOS5260_POWER_BASE + INFORM3_OFFSET)
#elif defined(CONFIG_CPU_EXYNOS5430)
#define OmPin	readl(EXYNOS5430_POWER_BASE + INFORM3_OFFSET)
#else
#define OmPin	readl(EXYNOS5_POWER_BASE + INFORM3_OFFSET)
#endif

#ifdef CONFIG_FASTBOOT_AUTO_REBOOT
inline void do_reset_fastboot(void)
{
#ifdef CONFIG_CPU_EXYNOS3250
	writel(CONFIG_FASTBOOT_AUTO_REBOOT_MODE,
			EXYNOS4_POWER_BASE + INFORM4_OFFSET);
#elif defined(CONFIG_CPU_EXYNOS5422_EVT0)
	writel(CONFIG_FASTBOOT_AUTO_REBOOT_MODE,
			EXYNOS5_POWER_BASE + INFORM4_OFFSET);
#endif
	do_reset(NULL, 0, 0, NULL);
}
#endif

#if defined(CONFIG_FASTBOOT)

/* Use do_reset for fastboot's 'reboot' command */
//extern int do_reset (cmd_tbl_t *cmdtp, int flag, int argc, char *argv[]);
/* Use do_fat_fsload for direct image fusing from sd card */
//extern int do_fat_fsload (cmd_tbl_t *cmdtp, int flag, int argc, char *argv[]);
/* Use do_setenv and do_saveenv to permenantly save data */
int do_saveenv (cmd_tbl_t *cmdtp, int flag, int argc, char *argv[]);
int do_setenv ( cmd_tbl_t *cmdtp, int flag, int argc, char *argv[]);
/* Use do_bootm and do_go for fastboot's 'boot' command */
//int do_bootm (cmd_tbl_t *cmdtp, int flag, int argc, char *argv[]);
int do_go (cmd_tbl_t *cmdtp, int flag, int argc, char *argv[]);

#if defined(CFG_FASTBOOT_ONENANDBSP)
#define CFG_FASTBOOT_FLASHCMD			do_onenand
/* Use do_onenand for fastboot's flash commands */
extern int do_onenand(cmd_tbl_t * cmdtp, int flag, int argc, char *argv[]);
#elif defined(CFG_FASTBOOT_NANDBSP)
#define CFG_FASTBOOT_FLASHCMD			do_nand
/* Use do_nand for fastboot's flash commands */
extern int do_nand(cmd_tbl_t * cmdtp, int flag, int argc, char *argv[]);
#endif

#if defined(CFG_FASTBOOT_SDMMCBSP)
int do_movi(cmd_tbl_t * cmdtp, int flag, int argc, char *argv[]);
int do_mmcops(cmd_tbl_t * cmdtp, int flag, int argc, char *argv[]);
int do_mmcops_secure(cmd_tbl_t * cmdtp, int flag, int argc, char *argv[]);
int get_mmc_part_info(char *device_name, int part_num, int *start, int *count, unsigned char *pid);
struct mmc *find_mmc_device(int dev_num);
#endif

#ifdef CONFIG_USE_LCD
extern void Display_Turnoff(void);
extern void LCD_setfgcolor(unsigned int color);
extern void LCD_setleftcolor(unsigned int color);
extern void LCD_setprogress(int percentage);
#endif

/* Forward decl */
static int rx_handler (const unsigned char *buffer, unsigned int buffer_size);
static void reset_handler (void);

/* cmd_fastboot_interface	in fastboot.h	*/
static struct cmd_fastboot_interface interface =
{
	.rx_handler            = rx_handler,
	.reset_handler         = reset_handler,
	.product_name          = NULL,
	.serial_no             = NULL,
	.nand_block_size       = 0,
	.transfer_buffer       = (unsigned char *)0xffffffff,
	.transfer_buffer_size  = 0,
};
#if defined(CONFIG_RAMDUMP_MODE)
static unsigned int is_ramdump = 0;
#endif

static unsigned int download_size;
static unsigned int download_bytes;
//static unsigned int download_bytes_unpadded;
static unsigned int download_error;

#ifdef CONFIG_FASTBOOT_FLASH_CHUNK
#define CHUNK_SIZE		(CONFIG_FASTBOOT_CHUNK_SIZE * 1024 * 1024)
#define CHUNK_BLK_COUNT		(CHUNK_SIZE / CFG_FASTBOOT_SDMMC_BLOCKSIZE)
#define FASTBOOT_GETVAR_PART_TYPE	"partition-type"
static unsigned int total_chunk;
static unsigned int download_chunk;
static unsigned int download_is_sparse;
static fastboot_ptentry *download_ptn;
static int download_done;
#endif /* CONFIG_FASTBOOT_FLASH_CHUNK */

static int emmc_dev;
static int sd_dev = 1;
static int force_emmc;

/* To support the Android-style naming of flash */
#define MAX_PTN 16
static fastboot_ptentry ptable[MAX_PTN];
static unsigned int pcount;
static int static_pcount = -1;
static unsigned int gflag_reboot;

/* Default partition table (see cpu/.../fastboot.c) */
extern fastboot_ptentry *ptable_default;
extern unsigned int ptable_default_size;

static void set_env(char *var, char *val)
{
	char *setenv[4]  = { "setenv", NULL, NULL, NULL, };

	setenv[1] = var;
	setenv[2] = val;

	do_env_set(NULL, 0, 3, setenv);
}

static void save_env(struct fastboot_ptentry *ptn,
		     char *var, char *val)
{
#ifndef CFG_FASTBOOT_SDMMCBSP
	char start[32], length[32];
	char ecc_type[32];

	char *lock[5]    = { "nand", "lock",   NULL, NULL, NULL, };
	char *unlock[5]  = { "nand", "unlock", NULL, NULL, NULL, };
	char *ecc[4]     = { "nand", "ecc",    NULL, NULL, };
	char *saveenv[2] = { "setenv", NULL, };

	lock[2] = unlock[2] = start;
	lock[3] = unlock[3] = length;

	set_env (var, val);

	/* Some flashing requires the nand's ecc to be set */
	ecc[2] = ecc_type;
	if ((ptn->flags & FASTBOOT_PTENTRY_FLAGS_WRITE_HW_ECC) &&
	    (ptn->flags & FASTBOOT_PTENTRY_FLAGS_WRITE_SW_ECC))	{
		/* Both can not be true */
		printf("Warning can not do hw and sw ecc for partition '%s'\n", ptn->name);
		printf("Ignoring these flags\n");
	} else if (ptn->flags & FASTBOOT_PTENTRY_FLAGS_WRITE_HW_ECC) {
		sprintf(ecc_type, "hw");
		CFG_FASTBOOT_FLASHCMD(NULL, 0, 3, ecc);
	} else if (ptn->flags & FASTBOOT_PTENTRY_FLAGS_WRITE_SW_ECC) {
		sprintf(ecc_type, "sw");
		CFG_FASTBOOT_FLASHCMD(NULL, 0, 3, ecc);
	}
	sprintf(start, "0x%x", ptn->start);
	sprintf(length, "0x%x", ptn->length);

	/* This could be a problem is there is an outstanding lock */
	CFG_FASTBOOT_FLASHCMD(NULL, 0, 4, unlock);
	do_saveenv(NULL, 0, 1, saveenv);
	CFG_FASTBOOT_FLASHCMD(NULL, 0, 4, lock);
#endif
}

static void reset_handler ()
{
	/* If there was a download going on, bail */
	download_size = 0;
	download_bytes = 0;
	//download_bytes_unpadded = 0;
	download_error = 0;
#ifdef CONFIG_FASTBOOT_FLASH_CHUNK
	download_ptn = NULL;
	download_done = download_is_sparse = total_chunk = download_chunk = 0;
#endif
}


/* When save = 0, just parse.  The input is unchanged
   When save = 1, parse and do the save.  The input is changed */
static int parse_env(void *ptn, char *err_string, int save, int debug)
{
	int ret = 1;
	unsigned int sets = 0;
	unsigned int comment_start = 0;
	char *var = NULL;
	char *var_end = NULL;
	char *val = NULL;
	char *val_end = NULL;
	unsigned int i;

	char *buff = (char *)interface.transfer_buffer;
	//unsigned int size = download_bytes_unpadded;
	unsigned int size = download_bytes;

	/* The input does not have to be null terminated.
	   This will cause a problem in the corner case
	   where the last line does not have a new line.
	   Put a null after the end of the input.

	   WARNING : Input buffer is assumed to be bigger
	   than the size of the input */
	if (save)
		buff[size] = 0;

	for (i = 0; i < size; i++) {

		if (NULL == var) {

			/*
			 * Check for comments, comment ok only on
			 * mostly empty lines
			 */
			if (buff[i] == '#')
				comment_start = 1;

			if (comment_start) {
				if  ((buff[i] == '\r') ||
				     (buff[i] == '\n')) {
					comment_start = 0;
				}
			} else {
				if (!((buff[i] == ' ') ||
				      (buff[i] == '\t') ||
				      (buff[i] == '\r') ||
				      (buff[i] == '\n'))) {
					/*
					 * Normal whitespace before the
					 * variable
					 */
					var = &buff[i];
				}
			}

		} else if (((NULL == var_end) || (NULL == val)) &&
			   ((buff[i] == '\r') || (buff[i] == '\n'))) {

			/* This is the case when a variable
			   is unset. */

			if (save) {
				/* Set the var end to null so the
				   normal string routines will work

				   WARNING : This changes the input */
				buff[i] = '\0';

				save_env(ptn, var, val);

				if (debug)
					printf("Unsetting %s\n", var);
			}

			/* Clear the variable so state is parse is back
			   to initial. */
			var = NULL;
			var_end = NULL;
			sets++;
		} else if (NULL == var_end) {
			if ((buff[i] == ' ') ||
			    (buff[i] == '\t'))
				var_end = &buff[i];
		} else if (NULL == val) {
			if (!((buff[i] == ' ') ||
			      (buff[i] == '\t')))
				val = &buff[i];
		} else if (NULL == val_end) {
			if ((buff[i] == '\r') ||
			    (buff[i] == '\n')) {
				/* look for escaped cr or ln */
				if ('\\' == buff[i - 1]) {
					/* check for dos */
					if ((buff[i] == '\r') &&
					    (buff[i+1] == '\n'))
						buff[i + 1] = ' ';
					buff[i - 1] = buff[i] = ' ';
				} else {
					val_end = &buff[i];
				}
			}
		} else {
			sprintf(err_string, "Internal Error");

			if (debug)
				printf("Internal error at %s %d\n",
				       __FILE__, __LINE__);
			return 1;
		}
		/* Check if a var / val pair is ready */
		if (NULL != val_end) {
			if (save) {
				/* Set the end's with nulls so
				   normal string routines will
				   work.

				   WARNING : This changes the input */
				*var_end = '\0';
				*val_end = '\0';

				save_env(ptn, var, val);

				if (debug)
					printf("Setting %s %s\n", var, val);
			}

			/* Clear the variable so state is parse is back
			   to initial. */
			var = NULL;
			var_end = NULL;
			val = NULL;
			val_end = NULL;

			sets++;
		}
	}

	/* Corner case
	   Check for the case that no newline at end of the input */
	if ((NULL != var) &&
	    (NULL == val_end)) {
		if (save) {
			/* case of val / val pair */
			if (var_end)
				*var_end = '\0';
			/* else case handled by setting 0 past
			   the end of buffer.
			   Similar for val_end being null */
			save_env(ptn, var, val);

			if (debug) {
				if (var_end)
					printf("Trailing Setting %s %s\n", var, val);
				else
					printf("Trailing Unsetting %s\n", var);
			}
		}
		sets++;
	}
	/* Did we set anything ? */
	if (0 == sets)
		sprintf(err_string, "No variables set");
	else
		ret = 0;

	return ret;
}

static int saveenv_to_ptn(struct fastboot_ptentry *ptn, char *err_string)
{
	int ret = 1;
	int save = 0;
	int debug = 0;

	/* err_string is only 32 bytes
	   Initialize with a generic error message. */
	sprintf(err_string, "%s", "Unknown Error");

	/* Parse the input twice.
	   Only save to the enviroment if the entire input if correct */
	save = 0;
	if (0 == parse_env(ptn, err_string, save, debug)) {
		save = 1;
		ret = parse_env(ptn, err_string, save, debug);
	}
	return ret;
}

#if defined(CFG_FASTBOOT_ONENANDBSP) || defined(CFG_FASTBOOT_NANDBSP)
static int write_to_ptn(struct fastboot_ptentry *ptn, unsigned int addr, unsigned int size)
{
	int ret = 1;
	char start[32], length[32];
	char wstart[32], wlength[32], wbuffer[32];
	char write_type[32];

	/* do_nand and do_onenand do not check argv[0] */
	char *argv_erase[5]  = { NULL, "erase",  NULL, NULL, NULL, };
	char *argv_write[6]  = { NULL, NULL,  NULL, NULL, NULL, NULL, };

	argv_erase[2] = start;
	argv_erase[3] = length;

	argv_write[1] = write_type;
	argv_write[2] = wbuffer;
	argv_write[3] = wstart;
	argv_write[4] = wlength;

	printf("flashing '%s'\n", ptn->name);

	sprintf(start, "0x%x", ptn->start);
	if (ptn->length == 0)
	{
		CFG_FASTBOOT_FLASHCMD(NULL, 0, 3, argv_erase);
	}
	else
	{
	        if (strcmp(ptn->name, "bootloader"))
                {
		sprintf(length, "0x%x", ptn->length);
		CFG_FASTBOOT_FLASHCMD(NULL, 0, 4, argv_erase);
		}
	}
	/* Which flavor of write to use */
	if (ptn->flags & FASTBOOT_PTENTRY_FLAGS_WRITE_YAFFS)
	{
		sprintf(write_type, "write.yaffs");
		sprintf(wlength, "0x%x", size);
	}
	else
	{
		sprintf(write_type, "write");
		if (interface.nand_block_size &&
			(size % interface.nand_block_size))
		{
			size = (size + interface.nand_block_size - 1)
					/ interface.nand_block_size * interface.nand_block_size;
		}

		sprintf(wlength, "0x%x", size);
	}
	sprintf(wbuffer, "0x%x", addr);
	sprintf(wstart, "0x%x", ptn->start);

        if (!strcmp(ptn->name, "bootloader"))
	{
		argv_write[2]  = "u-boot";
		sprintf(wstart, "0x%x", addr);
	}

	ret = CFG_FASTBOOT_FLASHCMD(NULL, 0, 5, argv_write);

#if 0
	if (0 == repeat) {
		if (ret) /* failed */
			save_block_values(ptn, 0, 0);
		else     /* success */
			save_block_values(ptn, ptn->start, download_bytes);
	}
#endif

	return ret;
}
#endif

#if defined(CFG_FASTBOOT_SDMMCBSP)
#if defined(CONFIG_S5P6450) && !defined(CONFIG_EMMC_4_4)
#define	DEV_NUM 1
#else
#define	DEV_NUM 0
#endif
static int write_buffer_sdmmc(unsigned int addr, unsigned int buflen,
		unsigned int base, unsigned int len, int is_sparse)
{
	int ret = 1;
	char run_cmd[64];

	if (!is_sparse) {
		sprintf(run_cmd, "mmc write %d 0x%x 0x%x 0x%x", emmc_dev,
				addr, base, len);
		ret = run_command(run_cmd, 0);
	} else {
		uint bl_st = base;
		uint bl_cnt = len;

		printf("Compressed ext4 image\n");

		ret = write_compressed_ext4(emmc_dev, (char *)addr, buflen,
					    bl_st, bl_cnt);
	}
	return ret;
}

static int write_to_ptn_sdmmc(struct fastboot_ptentry *ptn, unsigned int addr,
				unsigned int size)
{
	int ret = 1;
	char cmd[32], device[32], part[32], part2[32];
	char start[32], length[32], buffer[32], run_cmd[64];
	char dev_num[2];
	char *argv[6]  = { NULL, NULL, NULL, NULL, NULL, NULL, };
	int argc = 0;
	char *nul_buf;
#if defined(CONFIG_MMC_64BIT_BUS) || defined(CONFIG_CPU_EXYNOS5410_EVT2)
	char *nul_buf_align;
#endif
	int is_sparse;

	if ((ptn->length != 0) && (size > ptn->length)) {
		printf("Error: Image size is larger than partition size!\n");
		return 1;
	}

	printf("flashing '%s'\n", ptn->name);

	argv[1] = cmd;
	sprintf(cmd, "write");

	if (ptn->flags & FASTBOOT_PTENTRY_FLAGS_USE_MMC_CMD)
	{
		is_sparse = !check_compress_ext4((char *)addr, ptn->length);
		ret = write_buffer_sdmmc(addr, size,
				ptn->start / CFG_FASTBOOT_SDMMC_BLOCKSIZE,
				size / CFG_FASTBOOT_SDMMC_BLOCKSIZE,
				is_sparse);
	} else if (ptn->flags & FASTBOOT_PTENTRY_FLAGS_USE_FAT_CMD) {
		char *file_name;

		if (!strcmp(ptn->name, "kernel"))
			file_name = getenv("kernel_file");
		else if (!strcmp(ptn->name, "dtb"))
			file_name = getenv("fdtfile");
		else if (!strcmp(ptn->name, "ramdisk"))
			file_name = getenv("initrd_file");
		else
			return -1;

		sprintf(run_cmd, "fatwrite mmc %d:1 0x%x %s 0x%x",
			emmc_dev, addr, file_name, size);
		ret = run_command(run_cmd, 0);
	} else if (ptn->flags & FASTBOOT_PTENTRY_FLAGS_USE_MOVI_CMD) {
		argv[2] = part;
		argv[3] = dev_num;
		argv[4] = buffer;
		sprintf(dev_num, "%d", emmc_dev);

		argc = 5;

		/* use the partition name that can be understood by a command, movi */
		if (!strcmp(ptn->name, "bootloader"))
		{
			if (OmPin == 7 || force_emmc) {
				argv[2] = part2;
				argv[3] = part;
				argv[4] = dev_num;
				argv[5] = buffer;
				argc = 6;
				strncpy(part2, "zero", 7);
				strncpy(part, "u-boot", 7);
				sprintf(run_cmd, "emmc open %d", emmc_dev);
				run_command(run_cmd, 0);
			}
			else
				strncpy(part, "u-boot", 7);

		}
		else if (!strcmp(ptn->name, "fwbl1"))
		{
			if (OmPin == 7 || force_emmc) {
				argv[2] = part2;
				argv[3] = ptn->name;
				argv[4] = dev_num;
				argv[5] = buffer;
				argc = 6;
				strncpy(part2, "zero", 7);
				sprintf(run_cmd, "emmc open %d", emmc_dev);
				run_command(run_cmd, 0);
			}
			else
				argv[2] = ptn->name;

		}
		else if (!strcmp(ptn->name, "bl2"))
		{
			if (OmPin == 7 || force_emmc) {
				argv[2] = part2;
				argv[3] = ptn->name;
				argv[4] = dev_num;
				argv[5] = buffer;
				argc = 6;
				strncpy(part2, "zero", 7);
				sprintf(run_cmd, "emmc open %d", emmc_dev);
				run_command(run_cmd, 0);
			}
			else
				argv[2] = ptn->name;

		}
		else if (!strcmp(ptn->name, "tzsw"))
		{
			if (OmPin == 7 || force_emmc) {
				argv[2] = part2;
				argv[3] = ptn->name;
				argv[4] = dev_num;
				argv[5] = buffer;
				argc = 6;
				strncpy(part2, "zero", 7);
				sprintf(run_cmd, "emmc open %d", emmc_dev);
				run_command(run_cmd, 0);
			}
			else
				argv[2] = ptn->name;

		}
		else if (!strcmp(ptn->name, "ramdisk"))
		{
			strncpy(part, "rootfs", 7);
			argv[5] = length;
			sprintf(length, "0x%x",
				((size + CFG_FASTBOOT_SDMMC_BLOCKSIZE - 1)
				/ CFG_FASTBOOT_SDMMC_BLOCKSIZE ) * CFG_FASTBOOT_SDMMC_BLOCKSIZE);

#ifdef CONFIG_ROOTFS_ATAGS
			char ramdisk_size[32];
#ifdef CONFIG_SECURE_ROOTFS
			sprintf(ramdisk_size, "0x%x", size - 256);
#else
			sprintf(ramdisk_size, "0x%x", size);
#endif
			setenv("rootfslen", ramdisk_size);
			saveenv_one_variable("rootfslen", ramdisk_size);
#endif

			argc++;
		}
#ifdef CONFIG_CHARGER_LOGO
		else if (!strcmp(ptn->name, "charger"))
		{
			argv[2] = part;
			strncpy(part, "charger", 7);
		}
#endif
#ifdef CONFIG_BOOT_LOGO
		else if (!strcmp(ptn->name, "bootlogo"))
		{
			argv[2] = part;
			strncpy(part, "logo", 7);
		}
#endif
		else	/* kernel */
		{
			argv[2] = ptn->name;
		}
		sprintf(buffer, "0x%x", addr);

		ret = do_movi(NULL, 0, argc, argv);

		if ((OmPin == 7 || force_emmc) &&
			(!strcmp(ptn->name, "fwbl1") ||
			 !strcmp(ptn->name, "bootloader") ||
			 !strcmp(ptn->name, "bl2") ||
			 !strcmp(ptn->name, "tzsw"))) {
			sprintf(run_cmd, "emmc close %d", emmc_dev);
			run_command(run_cmd, 0);
		}

		/* the return value of do_movi is different from usual commands. Hence the followings. */
		ret = 1 - ret;
	}

	return ret;
}
#endif

#if defined(CONFIG_RAMDUMP_MODE)
static void start_ramdump(void *buf)
{
	unsigned *args;
	args = (unsigned *)buf;

	printf("\nramdump start address is [0x%x]\n", *args);
	printf("ramdump size is [0x%x]\n", *(args + 1));

	if (!fastboot_tx_mem((const char *)*args, *(args + 1)))
		printf("Failed ramdump~! \n");
	else
		printf("Finished ramdump~! \n");
}
#endif

#ifdef CONFIG_FASTBOOT_FLASH_CHUNK
static unsigned int check_chunk_downloaded(unsigned int download_bytes)
{
	if (download_bytes == CHUNK_SIZE ||
	    (download_bytes == download_size - (download_chunk * CHUNK_SIZE)))
		return 1;
	else
		return 0;
}

static int flash_chunk_data(const unsigned char *buffer,
				unsigned int buffer_size, char *response)
{
	unsigned int transfer_size = download_size - download_bytes;
	unsigned int start, length;
	int ret;
	if (buffer_size < transfer_size)
		transfer_size = buffer_size;

	/* Save the data to the transfer buffer */
	memcpy(interface.transfer_buffer + download_bytes,
		buffer, transfer_size);

	download_bytes += transfer_size;

	/* Skip until the buffer is reached to chunk size */
	if (check_chunk_downloaded(download_bytes)) {
		/* Check if it is sparse file */
		if (download_chunk == 0) {
			download_is_sparse = !check_compress_ext4(
					(char *)interface.transfer_buffer,
					download_ptn->length);
		}

		start = (download_ptn->start / CFG_FASTBOOT_SDMMC_BLOCKSIZE) +
			(download_chunk * CHUNK_BLK_COUNT);
		length = download_bytes / CFG_FASTBOOT_SDMMC_BLOCKSIZE;
		if (download_bytes % CFG_FASTBOOT_SDMMC_BLOCKSIZE)
			length++;
		ret = write_buffer_sdmmc(interface.transfer_buffer,
					 download_bytes, start, length,
					 download_is_sparse);
		if (ret) {
			sprintf(response, "ERROR");
			fastboot_tx_status(response, strlen(response),
					   FASTBOOT_TX_ASYNC);
			return 0;
		}

		download_bytes = 0;
		download_chunk++;

		printf("(%d/%d) downloaded and flashed successfully\n",
				download_chunk, total_chunk);

		if (download_chunk == total_chunk) {
			download_done = 1;
			sprintf(response, "OKAY");
			fastboot_tx_status(response, strlen(response),
					   FASTBOOT_TX_ASYNC);

			printf("\ndownloading of %d bytes finished\n",
					download_size);
			download_bytes = download_size;
			download_size = 0;
		}
	}

	return 0;
}
#endif /* CONFIG_FASTBOOT_FLASH_CHUNK */

static int download_data(const unsigned char *buffer,
			 unsigned int buffer_size, char *response)
{
	/* Something to download */
	if (buffer_size) {
#ifdef CONFIG_FASTBOOT_FLASH_CHUNK
		if (download_ptn)
			return flash_chunk_data(buffer, buffer_size, response);
#endif /* CONFIG_FASTBOOT_FLASH_CHUNK */

		/* Handle possible overflow */
		unsigned int transfer_size = download_size - download_bytes;

		if (buffer_size < transfer_size)
			transfer_size = buffer_size;

		/* Save the data to the transfer buffer */
		memcpy(interface.transfer_buffer + download_bytes,
				buffer, transfer_size);

		download_bytes += transfer_size;

		/* Check if transfer is done */
		if (download_bytes >= download_size) {
			/* Reset global transfer variable,
			   Keep download_bytes because it will be
			   used in the next possible flashing command */
			download_size = 0;

			if (download_error) {
				/* There was an earlier error */
				sprintf(response, "ERROR");
			} else {
				/* Everything has transferred,
				   send the OK response */
				sprintf(response, "OKAY");
			}
			fastboot_tx_status(response, strlen(response),
					   FASTBOOT_TX_ASYNC);

			printf("\ndownloading of %d bytes finished\n",
				download_bytes);
#ifdef CONFIG_USE_LCD
			LCD_setprogress(0);
#endif
#if defined(CONFIG_RAMDUMP_MODE)
			if (is_ramdump) {
				is_ramdump = 0;
				start_ramdump((void *)buffer);
			}
#endif
		}

		/* Provide some feedback */
		if (download_bytes && download_size &&
				0 == (download_bytes & (0x100000 - 1)))
		{
			/* Some feeback that the download is happening */
			if (download_error)
				printf("X");
			else
				printf(".");

			if (0 == (download_bytes % (80 * 0x100000)))
				printf("\n");
#ifdef CONFIG_USE_LCD
			LCD_setfgcolor(0x2E8B57);
			LCD_setprogress(download_bytes / (download_size/100));
#endif
		}
	} else {
		/* Ignore empty buffers */
		printf("Warning empty download buffer\n");
		printf("Ignoring\n");
	}

	return 0;
}

static int process_cmd_reboot(const char *cmdbuf, char *response)
{
	if (!strcmp(cmdbuf + 6, "-bootloader")) {
#ifdef CONFIG_FASTBOOT_AUTO_REBOOT
		sprintf(response, "OKAY");
		fastboot_tx_status(response, strlen(response),
				   FASTBOOT_TX_SYNC);
		do_reset_fastboot();
#else
		gflag_reboot = 1;
#endif
		return 0;
	} else
		memset(interface.transfer_buffer, 0x0,
				FASTBOOT_REBOOT_MAGIC_SIZE);

	sprintf(response, "OKAY");
	fastboot_tx_status(response, strlen(response), FASTBOOT_TX_SYNC);

#ifdef CONFIG_USE_LCD
	/* Turning LCD off before SW reset. */
	Display_Turnoff();
#endif
	do_reset(NULL, 0, 0, NULL);

	/* This code is unreachable,
	   leave it to make the compiler happy */
	return 0;
}

static void check_ptn_flash_chunk(const char *name)
{
	download_ptn = fastboot_flash_find_ptn(name);

	if (download_ptn && !(download_ptn->flags &
				FASTBOOT_PTENTRY_FLAGS_FLASH_CHUNK))
		download_ptn = NULL;
}

static int process_cmd_getvar(const char *cmdbuf, char *response)
{
	/* getvar
	   Get common fastboot variables
	   Board has a chance to handle other variables */
	strcpy(response, "OKAY");

	if (!strcmp(cmdbuf + 7, "version"))
		strcpy(response + 4, FASTBOOT_VERSION);
	else if (!strcmp(cmdbuf + 7, "product")) {
		if (interface.product_name)
			strcpy(response + 4, interface.product_name);
	} else if (!strcmp(cmdbuf + 7, "serialno")) {
		if (interface.serial_no)
			strcpy(response + 4, interface.serial_no);
	} else if (!strcmp(cmdbuf + 7, "downloadsize")) {
		if (interface.transfer_buffer_size)
			sprintf(response + 4, "%08x",
					interface.transfer_buffer_size);
#ifdef CONFIG_FASTBOOT_FLASH_CHUNK
	} else if (!strncmp(cmdbuf + 7, FASTBOOT_GETVAR_PART_TYPE,
				strlen(FASTBOOT_GETVAR_PART_TYPE))) {
		check_ptn_flash_chunk(cmdbuf + 7 +
				strlen(FASTBOOT_GETVAR_PART_TYPE) + 1);
#endif
	} else
		fastboot_getvar(cmdbuf + 7, response + 4);
	return 0;
}

static int process_cmd_erase(const char *cmdbuf, char *response)
{
	struct fastboot_ptentry *ptn;
	char start[32], length[32];
	int status;

	ptn = fastboot_flash_find_ptn(cmdbuf + 6);
	if (ptn == 0) {
		sprintf(response, "FAILpartition does not exist");
		return 0;
	}

	if (OmPin == BOOT_MMCSD)
		printf("erasing(formatting) '%s'\n", ptn->name);
	else if (OmPin == BOOT_EMMC_4_4 || OmPin == BOOT_EMMC)
		printf("erasing '%s'\n", ptn->name);
	else if (OmPin == BOOT_ONENAND)
		printf("erasing '%s'\n", ptn->name);

#ifdef CONFIG_USE_LCD
	LCD_setfgcolor(0x7FFFD4);
	LCD_setprogress(100);
#endif

	if (OmPin == BOOT_MMCSD) {
		/* Temporary (but, simplest) implementation */
		char run_cmd[80];
		status = 1;
		if (!strcmp(ptn->name, "userdata"))
		{
			sprintf(run_cmd, "ext3format mmc 0:3");
			status = run_command(run_cmd, 0);
		} else if (!strcmp(ptn->name, "cache")) {
			sprintf(run_cmd, "ext3format mmc 0:4");
			status = run_command(run_cmd, 0);
		} else if (!strcmp(ptn->name, "fat")) {
			sprintf(run_cmd, "fatformat mmc 0:1");
			status = run_command(run_cmd, 0);
		}
	} else if (OmPin == BOOT_EMMC_4_4 || OmPin == BOOT_EMMC) {
		char run_cmd[80];
		status = 1;
		if (!strcmp(ptn->name, "userdata")) {
			sprintf(run_cmd, "ext3format mmc 0:3");
			status = run_command(run_cmd, 0);
		} else if (!strcmp(ptn->name, "cache")) {
			sprintf(run_cmd, "ext3format mmc 0:4");
			status = run_command(run_cmd, 0);
		} else if (!strcmp(ptn->name, "fat")) {
			sprintf(run_cmd, "fatformat mmc 0:1");
			status = run_command(run_cmd, 0);
		}
	} else if (OmPin == BOOT_ONENAND) {
#if defined(CFG_FASTBOOT_ONENANDBSP)
		int argc_erase = 4;
		/* do_nand and do_onenand do not check argv[0] */
		char *argv_erase[5]  = { NULL, "erase",  NULL, NULL, NULL, };

		argv_erase[2] = start;
		argv_erase[3] = length;

		sprintf(start, "0x%x", ptn->start);
		sprintf(length, "0x%x", ptn->length);

		if (ptn->length == 0)
			argc_erase = 3;

		status = CFG_FASTBOOT_FLASHCMD(NULL, 0, argc_erase,
						argv_erase);
#endif
	}

	if (status)
		sprintf(response, "FAILfailed to erase partition");
	else {
		printf("partition '%s' erased\n", ptn->name);
		sprintf(response, "OKAY");
	}

	return 0;
}

static int process_cmd_download(const char *cmdbuf, char *response)
{
	/* save the size */
	download_size = simple_strtoul(cmdbuf + 9, NULL, 16);
	/* Reset the bytes count, now it is safe */
	download_bytes = 0;
	/* Reset error */
	download_error = 0;

#ifdef CONFIG_FASTBOOT_FLASH_CHUNK
	if (download_ptn) {
		total_chunk = download_size / CHUNK_SIZE;
		if (download_size % CHUNK_SIZE)
			total_chunk++;
	}
#endif
	printf("Starting download of %d bytes\n", download_size);

	if (0 == download_size) {
		/* bad user input */
		sprintf(response, "FAILdata invalid size");
#ifndef CONFIG_FASTBOOT_FLASH_CHUNK
	} else if (download_size > interface.transfer_buffer_size) {
		/* set download_size to 0 because this is an error */
		download_size = 0;
		sprintf(response, "FAILdata too large");
#endif
	} else {
		/* The default case, the transfer fits
		   completely in the interface buffer */
		sprintf(response, "DATA%08x", download_size);
	}

	return 0;
}

/* boot
   boot what was downloaded

   WARNING WARNING WARNING

   This is not what you expect.
   The fastboot client does its own packaging of the
   kernel.  The layout is defined in the android header
   file bootimage.h.  This layeout is copiedlooks like this,

 **
 ** +-----------------+
 ** | boot header     | 1 page
 ** +-----------------+
 ** | kernel          | n pages
 ** +-----------------+
 ** | ramdisk         | m pages
 ** +-----------------+
 ** | second stage    | o pages
 ** +-----------------+
 **

 What is a page size ?
 The fastboot client uses 2048

 The is the default value of CFG_FASTBOOT_MKBOOTIMAGE_PAGE_SIZE
 */
static int process_cmd_boot(const char *cmdbuf, char *response)
{
	if (download_bytes &&
		(CFG_FASTBOOT_MKBOOTIMAGE_PAGE_SIZE < download_bytes)) {
		/* Note: We store zImage and ramdisk at different partitions */
		char addr_kernel[32];
		char addr_ramdisk[32];
		int pageoffset_ramdisk;

		char *bootz[3] = { "bootz", NULL, NULL, };

		/*
		 * Use this later to determine if a command line was passed
		 * for the kernel.
		 */
		struct fastboot_boot_img_hdr *fb_hdr =
			(struct fastboot_boot_img_hdr *)
				interface.transfer_buffer;

		/* Skip the mkbootimage header */
		image_header_t *hdr = (image_header_t *)
			&interface.transfer_buffer
			[CFG_FASTBOOT_MKBOOTIMAGE_PAGE_SIZE];

		printf("Kernel size: %08x\n", fb_hdr->kernel_size);
		printf("Ramdisk size: %08x\n", fb_hdr->ramdisk_size);

		pageoffset_ramdisk = 1 + (fb_hdr->kernel_size +
				CFG_FASTBOOT_MKBOOTIMAGE_PAGE_SIZE - 1)
				/ CFG_FASTBOOT_MKBOOTIMAGE_PAGE_SIZE;

		bootz[1] = addr_kernel;
		sprintf(addr_kernel, "0x%x", CFG_FASTBOOT_ADDR_KERNEL);
		memcpy((void *)CFG_FASTBOOT_ADDR_KERNEL,
				interface.transfer_buffer +
				CFG_FASTBOOT_MKBOOTIMAGE_PAGE_SIZE,
				fb_hdr->kernel_size);
		bootz[2] = addr_ramdisk;
		sprintf(addr_ramdisk, "0x%x", CFG_FASTBOOT_ADDR_RAMDISK);
		memcpy((void *)CFG_FASTBOOT_ADDR_RAMDISK,
				interface.transfer_buffer +
				(pageoffset_ramdisk *
				 CFG_FASTBOOT_MKBOOTIMAGE_PAGE_SIZE),
				fb_hdr->ramdisk_size);

		/* Execution should jump to kernel so send the response
		   now and wait a bit.  */
		sprintf(response, "OKAY");
		fastboot_tx_status(response, strlen(response),
				FASTBOOT_TX_SYNC);
		udelay(1000000); /* 1 sec */

#ifdef CONFIG_ROOTFS_ATAGS
		char ramdisk_size[32];
		sprintf(ramdisk_size, "0x%x", fb_hdr->ramdisk_size);
		setenv("rootfslen", ramdisk_size);
#endif
		if (ntohl(hdr->ih_magic) == IH_MAGIC) {
			/* Looks like a kernel.. */
			printf("Booting kernel..\n");

			/*
			 * Check if the user sent a bootargs down.
			 * If not, do not override what is already there
			 */
			if (strlen((char *) &fb_hdr->cmdline[0]))
				set_env("bootargs",
					(char *)&fb_hdr->cmdline[0]);
			do_bootz(NULL, 0, 2, bootz);
		} else {
			/* Raw image, maybe another uboot */
			printf("Booting raw image..\n");

			do_bootz(NULL, 0, 3, bootz);
		}
		printf("ERROR : bootting failed\n");
		printf("You should reset the board\n");
	}
	sprintf(response, "FAILinvalid boot image");

	return 0;
}

static int process_cmd_flash_boot(const char *cmdbuf, char *response)
{
	int pageoffset_ramdisk, ret;
	struct fastboot_ptentry *ptn;

	struct fastboot_boot_img_hdr *fb_hdr =
		(struct fastboot_boot_img_hdr *)interface.transfer_buffer;
	image_header_t *hdr = (image_header_t *)
		&interface.transfer_buffer[CFG_FASTBOOT_MKBOOTIMAGE_PAGE_SIZE];

	printf("Kernel size: %08x\n", fb_hdr->kernel_size);
	printf("Ramdisk size: %08x\n", fb_hdr->ramdisk_size);

	ptn = fastboot_flash_find_ptn("kernel");
	if (ptn->length && fb_hdr->kernel_size > ptn->length) {
		sprintf(response, "FAILimage too large for partition");
		return 0;
	}

	if (OmPin == BOOT_ONENAND) {
#if defined(CFG_FASTBOOT_ONENANDBSP)
		ret = write_to_ptn(ptn,
				(unsigned int)interface.transfer_buffer
				+ CFG_FASTBOOT_MKBOOTIMAGE_PAGE_SIZE,
				fb_hdr->kernel_size);
#endif
	} else if (OmPin == BOOT_MMCSD) {
		ret = write_to_ptn_sdmmc(ptn,
				(unsigned int)interface.transfer_buffer
				+ CFG_FASTBOOT_MKBOOTIMAGE_PAGE_SIZE,
				fb_hdr->kernel_size);
	} else if (OmPin == BOOT_EMMC_4_4 || OmPin == BOOT_EMMC) {
		ret = write_to_ptn_sdmmc(ptn,
				(unsigned int)interface.transfer_buffer
				+ CFG_FASTBOOT_MKBOOTIMAGE_PAGE_SIZE,
				fb_hdr->kernel_size);
	}

	pageoffset_ramdisk = 1 + (fb_hdr->kernel_size +
			CFG_FASTBOOT_MKBOOTIMAGE_PAGE_SIZE - 1)
		/ CFG_FASTBOOT_MKBOOTIMAGE_PAGE_SIZE;
	ptn = fastboot_flash_find_ptn("ramdisk");
	if (ptn->length && fb_hdr->ramdisk_size > ptn->length) {
		sprintf(response, "FAILimage too large for partition");
		return 0;
	}

	if (OmPin == BOOT_ONENAND) {
#if defined(CFG_FASTBOOT_ONENANDBSP)
		ret |= write_to_ptn(ptn,
				(unsigned int)interface.transfer_buffer
				+ (pageoffset_ramdisk *
					CFG_FASTBOOT_MKBOOTIMAGE_PAGE_SIZE),
				fb_hdr->ramdisk_size);
#endif
	} else if (OmPin == BOOT_MMCSD) {
		ret |= write_to_ptn_sdmmc(ptn,
				(unsigned int)interface.transfer_buffer +
				(pageoffset_ramdisk *
				 CFG_FASTBOOT_MKBOOTIMAGE_PAGE_SIZE),
				fb_hdr->ramdisk_size);
	} else if (OmPin == BOOT_EMMC_4_4 || OmPin == BOOT_EMMC) {
		ret |= write_to_ptn_sdmmc(ptn,
				(unsigned int)interface.transfer_buffer
				+ (pageoffset_ramdisk *
				   CFG_FASTBOOT_MKBOOTIMAGE_PAGE_SIZE),
				fb_hdr->ramdisk_size);
	}
	if (ret) {
		printf("flashing '%s' failed\n", "kernel+ramdisk");
		sprintf(response, "FAILfailed to flash partition");
	} else {
		printf("partition '%s' flashed\n", "kernel+ramdisk");
		sprintf(response, "OKAY");
	}

	return 0;
}

static int process_cmd_flash_env(const char *cmdbuf, char *response,
	struct fastboot_ptentry *ptn)
{
	/* Since the response can only be 64 bytes,
	   there is no point in having a large error message. */
	char err_string[32];
	if (saveenv_to_ptn(ptn, &err_string[0])) {
		printf("savenv '%s' failed : %s\n", ptn->name, err_string);
		sprintf(response, "FAIL%s", err_string);
	} else {
		printf("partition '%s' saveenv-ed\n", ptn->name);
		sprintf(response, "OKAY");
	}

	return 0;
}

static void process_cmd_download_result(int err, const char *name,
				       char *response)
{
	if (err) {
		printf("flashing '%s' failed\n", name);
		sprintf(response, "FAILfailed to flash partition");
	} else {
		printf("partition '%s' flashed\n", name);
		sprintf(response, "OKAY");
	}
}

static int process_cmd_flash_normal(const char *cmdbuf, char *response,
	struct fastboot_ptentry *ptn)
{
	int err;
	/* Normal case */
	if (OmPin == BOOT_ONENAND) {
#if defined(CFG_FASTBOOT_ONENANDBSP)
		err = write_to_ptn(ptn,
				   (unsigned int)interface.transfer_buffer,
				   download_bytes);
		process_cmd_download_result(err, ptn->name, response);
#endif
	} else if (OmPin == BOOT_MMCSD) {
		err = write_to_ptn_sdmmc(ptn,
		      (unsigned int)interface.transfer_buffer, download_bytes);
		process_cmd_download_result(err, ptn->name, response);
	} else if (OmPin == BOOT_EMMC_4_4 || OmPin == BOOT_EMMC) {
#ifdef CONFIG_FASTBOOT_FLASH_CHUNK
		if (download_ptn) {
			process_cmd_download_result(!download_done, ptn->name,
						    response);
			reset_handler();
			return 0;
		}
#endif

		err = write_to_ptn_sdmmc(ptn,
		      (unsigned int)interface.transfer_buffer, download_bytes);
		process_cmd_download_result(err, ptn->name, response);
	}
	return 0;
}

static int process_cmd_flash(const char *cmdbuf, char *response)
{
	struct fastboot_ptentry *ptn;

	if (download_bytes == 0) {
		sprintf(response, "FAILno image downloaded");
		return 0;
	}

#ifdef CONFIG_USE_LCD
	LCD_setfgcolor(0x8B4500);
	LCD_setprogress(100);
#endif

#ifdef CONFIG_ANDROID_PARTITIONS
	/* Special case: boot.img */
	if (!strcmp("boot", cmdbuf + 6))
		return process_cmd_flash_boot(cmdbuf, response);
#endif

	ptn = fastboot_flash_find_ptn(cmdbuf + 6);
	if (ptn == 0)
		sprintf(response, "FAILpartition does not exist");
	else if ((download_bytes > ptn->length) && (ptn->length != 0) &&
			!(ptn->flags & FASTBOOT_PTENTRY_FLAGS_WRITE_ENV)) {
		sprintf(response, "FAILimage too large for partition");
		/* TODO : Improve check for yaffs write */
	} else {
		/* Check if this is not really a flash write
		   but rather a saveenv */
		if (ptn->flags & FASTBOOT_PTENTRY_FLAGS_WRITE_ENV)
			return process_cmd_flash_env(cmdbuf, response, ptn);
		else
			return process_cmd_flash_normal(cmdbuf, response, ptn);
	}
	return 0;
}

static int process_cmd_format(void)
{
	char run_cmd[32];
	int status = -1;
	char *part_type = getenv("part_type");
	if (!part_type)
		part_type = "gpt";

	if (!strcmp(part_type, "gpt")) {
		sprintf(run_cmd, "gpt write mmc %d $partitions", emmc_dev);
		status = run_command(run_cmd, 0);
	}
	return status;
}

static int process_cmd_oem(const char *cmdbuf, char *response)
{
	if (!strcmp("format", cmdbuf + 4)) {
		if (process_cmd_format() == -1) {
			sprintf(response, "FAILpartition format");
			fastboot_tx_status(response, strlen(response),
					   FASTBOOT_TX_ASYNC);
			return -1;
		} else {
			sprintf(response, "OKAY");
			fastboot_tx_status(response, strlen(response),
					FASTBOOT_TX_SYNC);
#ifdef CONFIG_FASTBOOT_AUTO_REBOOT
			do_reset_fastboot();
#endif
		}
	} else {
		sprintf(response, "INFOunknown OEM command");
		fastboot_tx_status(response, strlen(response),
				   FASTBOOT_TX_ASYNC);
	}

	sprintf(response, "OKAY");
	fastboot_tx_status(response, strlen(response), FASTBOOT_TX_ASYNC);

	return 0;
}

#ifdef CONFIG_RAMDUMP_MODE
static int process_cmd_ramdump(const char *cmdbuf, char *response)
{
	printf("\nGot ramdump command\n");
	is_ramdump = 1;
	/* save the size */
	download_size = simple_strtoul(cmdbuf + 8, NULL, 16);
	/* Reset the bytes count, now it is safe */
	download_bytes = 0;
	/* Reset error */
	download_error = 0;

	printf("Starting download of %d bytes\n", download_size);

	if (0 == download_size) {
		/* bad user input */
		sprintf(response, "FAILdata invalid size");
	} else if (download_size > interface.transfer_buffer_size) {
		/* set download_size to 0 because this is an error */
		download_size = 0;
		sprintf(response, "FAILdata too large");
	} else {
		/* The default case, the transfer fits
		   completely in the interface buffer */
		sprintf(response, "DATA%08x", download_size);
	}

	return 0;
}
#endif /* CONFIG_RAMDUMP_MODE */

static int rx_handler (const unsigned char *buffer, unsigned int buffer_size)
{
	int ret = 1;

	/* Use 65 instead of 64
	   null gets dropped
	   strcpy's need the extra byte */
	/* A special hardware needs 8 bytes aligned address for dma transfer */
	char *response = (char *)memalign(8, 65);

	if (download_size)
		ret = download_data(buffer, buffer_size, response);
	else {
		/* A command */

		/* Cast to make compiler happy with string functions */
		const char *cmdbuf = (char *) buffer;

		/* Generic failed response */
		sprintf(response, "FAIL");

		/* reboot
		   Reboot the board. */
		if (memcmp(cmdbuf, "reboot", 6) == 0) {
			ret = process_cmd_reboot(cmdbuf, response);
			goto ret_free;
		} else if (memcmp(cmdbuf, "getvar:", 7) == 0)
			ret = process_cmd_getvar(cmdbuf, response);
		else if (memcmp(cmdbuf, "erase:", 6) == 0)
			ret = process_cmd_erase(cmdbuf, response);
		else if (memcmp(cmdbuf, "download:", 9) == 0)
			ret = process_cmd_download(cmdbuf, response);
		else if (memcmp(cmdbuf, "boot", 4) == 0)
			ret = process_cmd_boot(cmdbuf, response);
		else if (memcmp(cmdbuf, "flash:", 6) == 0)
			ret = process_cmd_flash(cmdbuf, response);
		else if (memcmp(cmdbuf, "oem", 3) == 0)
			return process_cmd_oem(cmdbuf, response);
#if defined(CONFIG_RAMDUMP_MODE)
		else if (memcmp(cmdbuf, "ramdump:", 8) == 0)
			ret = process_cmd_ramdump(cmdbuf, response);
#endif
		fastboot_tx_status(response, strlen(response),
				FASTBOOT_TX_ASYNC);

#ifdef CONFIG_USE_LCD
		LCD_setprogress(0);
#endif
	} /* End of command */
ret_free:
	free(response);
	return ret;
}


static int check_against_static_partition(struct fastboot_ptentry *ptn)
{
	int ret = 0;
	struct fastboot_ptentry *c;
	int i;

	for (i = 0; i < static_pcount; i++) {
		c = fastboot_flash_get_ptn((unsigned int) i);

		if (0 == ptn->length)
			break;

		if ((ptn->start >= c->start) &&
		    (ptn->start < c->start + c->length))
			break;

		if ((ptn->start + ptn->length > c->start) &&
		    (ptn->start + ptn->length <= c->start + c->length))
			break;

		if ((0 == strcmp(ptn->name, c->name)) &&
		    (0 == strcmp(c->name, ptn->name)))
			break;
	}

	if (i >= static_pcount)
		ret = 1;
	return ret;
}


static unsigned long long memparse(char *ptr, char **retptr)
{
	char *endptr;	/* local pointer to end of parsed string */

	unsigned long ret = simple_strtoul(ptr, &endptr, 0);

	switch (*endptr) {
	case 'M':
	case 'm':
		ret <<= 10;
	case 'K':
	case 'k':
		ret <<= 10;
		endptr++;
	default:
		break;
	}

	if (retptr)
		*retptr = endptr;

	return ret;
}


static int add_partition_from_environment(char *s, char **retptr)
{
	unsigned long size;
	unsigned long offset = 0;
	char *name;
	int name_len;
	int delim;
	unsigned int flags;
	struct fastboot_ptentry part;

	size = memparse(s, &s);
	if (0 == size) {
		printf("Error:FASTBOOT size of parition is 0\n");
		return 1;
	}

	/* fetch partition name and flags */
	flags = 0; /* this is going to be a regular partition */
	delim = 0;
	/* check for offset */
	if (*s == '@') {
		s++;
		offset = memparse(s, &s);
	} else {
		printf("Error:FASTBOOT offset of parition is not given\n");
		return 1;
	}

	/* now look for name */
	if (*s == '(')
		delim = ')';

	if (delim) {
		char *p;

		name = ++s;
		p = strchr((const char *)name, delim);
		if (!p) {
			printf("Error:FASTBOOT no closing %c found in partition name\n", delim);
			return 1;
		}
		name_len = p - name;
		s = p + 1;
	} else {
		printf("Error:FASTBOOT no partition name for \'%s\'\n", s);
		return 1;
	}

	/* test for options */
	while (1) {
		if (strncmp(s, "i", 1) == 0) {
			flags |= FASTBOOT_PTENTRY_FLAGS_WRITE_I;
			s += 1;
		} else if (strncmp(s, "yaffs", 5) == 0) {
			/* yaffs */
			flags |= FASTBOOT_PTENTRY_FLAGS_WRITE_YAFFS;
			s += 5;
		} else if (strncmp(s, "swecc", 5) == 0) {
			/* swecc */
			flags |= FASTBOOT_PTENTRY_FLAGS_WRITE_SW_ECC;
			s += 5;
		} else if (strncmp(s, "hwecc", 5) == 0) {
			/* hwecc */
			flags |= FASTBOOT_PTENTRY_FLAGS_WRITE_HW_ECC;
			s += 5;
		} else {
			break;
		}
		if (strncmp(s, "|", 1) == 0)
			s += 1;
	}

	/* enter this partition (offset will be calculated later if it is zero at this point) */
	part.length = size;
	part.start = offset;
	part.flags = flags;

	if (name) {
		if (name_len >= sizeof(part.name)) {
			printf("Error:FASTBOOT partition name is too long\n");
			return 1;
		}
		strncpy(&part.name[0], name, name_len);
		/* name is not null terminated */
		part.name[name_len] = '\0';
	} else {
		printf("Error:FASTBOOT no name\n");
		return 1;
	}


	/* Check if this overlaps a static partition */
	if (check_against_static_partition(&part)) {
		printf("Adding: %s, offset 0x%8.8x, size 0x%8.8x, flags 0x%8.8x\n",
		       part.name, part.start, part.length, part.flags);
		fastboot_flash_add_ptn(&part);
	}

	/* return (updated) pointer command line string */
	*retptr = s;

	/* return partition table */
	return 0;
}

#if defined(CONFIG_FASTBOOT)
static int set_partition_table()
{
	char fbparts[4096], *env;

	/*
	 * Place the runtime partitions at the end of the
	 * static paritions.  First save the start off so
	 * it can be saved from run to run.
	 */
	if (static_pcount >= 0)
	{
		/* Reset */
		pcount = static_pcount;
	}
	else
	{
		/* Save */
		static_pcount = pcount;
	}
	env = getenv("fbparts");
	if (env)
	{
		unsigned int len;
		len = strlen(env);
		if (len && len < 4096)
		{
			char *s, *e;

			memcpy(&fbparts[0], env, len + 1);
			printf("Fastboot: Adding partitions from environment\n");
			s = &fbparts[0];
			e = s + len;
			while (s < e)
			{
				if (add_partition_from_environment(s, &s))
				{
					printf("Error:Fastboot: Abort adding partitions\n");
					/* reset back to static */
					pcount = static_pcount;
					break;
				}
				/* Skip a bunch of delimiters */
				while (s < e)
				{
					if ((' ' == *s) ||
					    ('\t' == *s) ||
					    ('\n' == *s) ||
					    ('\r' == *s) ||
					    (',' == *s)) {
						s++;
					}
					else
					{
						break;
					}
				}
			}
		}
	}
	else if (ptable_default_size >= sizeof(fastboot_ptentry))
	{
		printf("Fastboot: employ default partition information\n");
		//memcpy(ptable, ptable_default, ptable_default_size);
		memcpy((void*)ptable, (void*)&ptable_default, ptable_default_size);
		pcount = ptable_default_size / sizeof(fastboot_ptentry);
	}
	else
	{
		printf("No partition informations!");
		return 0;
	}

#if 1 // Debug
	fastboot_flash_dump_ptn();
#endif
#ifdef CONFIG_USE_LCD
	LCD_setleftcolor(0x1024C0);
#endif
	return 0;
}
#endif

#if defined(CFG_FASTBOOT_SDMMCBSP)

#ifdef CONFIG_FASTBOOT_GET_MMC_PARTITIONS
static int get_mmc_partition_tables()
{
	int i, ret;
	char mmc_dev[8];
	block_dev_desc_t *dev_desc;
	disk_partition_t info;

	sprintf(mmc_dev, "%d", emmc_dev);
	ret = get_device("mmc", mmc_dev, &dev_desc);
	if (ret < 0)
		return -1;

	for (i = 1; i < 128; i++) {
		ret = get_partition_info(dev_desc, i, &info);
		if (ret)
			break;

		strcpy(ptable[pcount].name, info.name);
		ptable[pcount].start = (unsigned long long)info.start *
							   info.blksz;
		ptable[pcount].length = (unsigned long long)info.size *
							   info.blksz;
		ptable[pcount].flags = FASTBOOT_PTENTRY_FLAGS_USE_MMC_CMD;
#ifdef CONFIG_FASTBOOT_FLASH_CHUNK
		ptable[pcount].flags |= FASTBOOT_PTENTRY_FLAGS_FLASH_CHUNK;
#endif

		pcount++;
	}

	return pcount;
}
#endif	/* CONFIG_FASTBOOT_GET_MMC_PARTITIONS */

static int set_partition_table_sdmmc()
{
	unsigned long long start, count;
	unsigned char pid;
	char dev_num[2];

	sprintf(dev_num, "%d", emmc_dev);

	pcount = 0;

	/* FW BL1 for fused chip */
	strcpy(ptable[pcount].name, "fwbl1");
	ptable[pcount].start = 0;
	ptable[pcount].length = 0;
	ptable[pcount].flags = FASTBOOT_PTENTRY_FLAGS_USE_MOVI_CMD;
	pcount++;

	/* BL2 */
	strcpy(ptable[pcount].name, "bl2");
	ptable[pcount].start = 0;
	ptable[pcount].length = 0;
	ptable[pcount].flags = FASTBOOT_PTENTRY_FLAGS_USE_MOVI_CMD;
	pcount++;

	/* Bootloader */
	strcpy(ptable[pcount].name, "bootloader");
	ptable[pcount].start = 0;
	ptable[pcount].length = 0;
	ptable[pcount].flags = FASTBOOT_PTENTRY_FLAGS_USE_MOVI_CMD;
	pcount++;

	/* TrustZone S/W */
	strcpy(ptable[pcount].name, "tzsw");
	ptable[pcount].start = 0;
	ptable[pcount].length = 0;
	ptable[pcount].flags = FASTBOOT_PTENTRY_FLAGS_USE_MOVI_CMD;
	pcount++;

	/* Environment */
	strcpy(ptable[pcount].name, "env");
	ptable[pcount].start = 0;
	ptable[pcount].length = 0;
	ptable[pcount].flags = FASTBOOT_PTENTRY_FLAGS_USE_MOVI_CMD;
	pcount++;

	/* Kernel */
	strcpy(ptable[pcount].name, "kernel");
	ptable[pcount].start = 0;
	ptable[pcount].length = 0;
	ptable[pcount].flags = FASTBOOT_PTENTRY_FLAGS_USE_FAT_CMD;
	pcount++;

	/* Ramdisk */
	strcpy(ptable[pcount].name, "ramdisk");
	ptable[pcount].start = 0;
	ptable[pcount].length = PART_SIZE_ROOTFS;
	ptable[pcount].flags = FASTBOOT_PTENTRY_FLAGS_USE_FAT_CMD;
	pcount++;

	/* Devicetree */
	strcpy(ptable[pcount].name, "dtb");
	ptable[pcount].start = 0;
	ptable[pcount].length = 0;
	ptable[pcount].flags = FASTBOOT_PTENTRY_FLAGS_USE_FAT_CMD;
	pcount++;

#ifdef CONFIG_CHARGER_LOGO
	strcpy(ptable[pcount].name, "charger");
	ptable[pcount].start = 0;
	ptable[pcount].length = PART_SIZE_CHARGER_LOGO;
	ptable[pcount].flags = FASTBOOT_PTENTRY_FLAGS_USE_MOVI_CMD;
	pcount++;
#endif
#ifdef CONFIG_BOOT_LOGO
	strcpy(ptable[pcount].name, "bootlogo");
	ptable[pcount].start = 0;
	ptable[pcount].length = PART_SIZE_BOOT_LOGO;
	ptable[pcount].flags = FASTBOOT_PTENTRY_FLAGS_USE_MOVI_CMD;
	pcount++;
#endif
#ifdef CONFIG_ANDROID_PARTITIONS
	/* System */
	get_mmc_part_info(dev_num, 2, &start, &count, &pid);
	if (pid != 0x83)
		goto part_type_error;
	strcpy(ptable[pcount].name, "system");
	ptable[pcount].start = start * CFG_FASTBOOT_SDMMC_BLOCKSIZE;
	ptable[pcount].length = count * CFG_FASTBOOT_SDMMC_BLOCKSIZE;
	ptable[pcount].flags = FASTBOOT_PTENTRY_FLAGS_USE_MMC_CMD;
	pcount++;

	/* Data */
	get_mmc_part_info(dev_num, 3, &start, &count, &pid);
	if (pid != 0x83)
		goto part_type_error;
	strcpy(ptable[pcount].name, "userdata");
	ptable[pcount].start = start * CFG_FASTBOOT_SDMMC_BLOCKSIZE;
	ptable[pcount].length = count * CFG_FASTBOOT_SDMMC_BLOCKSIZE;
	ptable[pcount].flags = FASTBOOT_PTENTRY_FLAGS_USE_MMC_CMD;
	pcount++;

	/* Cache */
	get_mmc_part_info(dev_num, 4, &start, &count, &pid);
	if (pid != 0x83)
		goto part_type_error;
	strcpy(ptable[pcount].name, "cache");
	ptable[pcount].start = start * CFG_FASTBOOT_SDMMC_BLOCKSIZE;
	ptable[pcount].length = count * CFG_FASTBOOT_SDMMC_BLOCKSIZE;
	ptable[pcount].flags = FASTBOOT_PTENTRY_FLAGS_USE_MMC_CMD;
	pcount++;

	/* fat */
	get_mmc_part_info(dev_num, 1, &start, &count, &pid);
	if (pid != 0xc)
		goto part_type_error;
	strcpy(ptable[pcount].name, "fat");
	ptable[pcount].start = start * CFG_FASTBOOT_SDMMC_BLOCKSIZE;
	ptable[pcount].length = count * CFG_FASTBOOT_SDMMC_BLOCKSIZE;
	ptable[pcount].flags = FASTBOOT_PTENTRY_FLAGS_USE_MMC_CMD;
	pcount++;
#endif	/* End of CONFIG_ANDROID_PARTITIONS */

#ifdef CONFIG_FASTBOOT_GET_MMC_PARTITIONS
	get_mmc_partition_tables();
#endif	/* CONFIG_FASTBOOT_GET_MMC_PARTITIONS */

#if 1 // Debug
	fastboot_flash_dump_ptn();
#endif
#ifdef CONFIG_USE_LCD
	LCD_setleftcolor(0x8a2be2);
#endif
	return 0;

part_type_error:
	printf("Error: No MBR is found at SD/MMC.\n");
	printf("Hint: use fdisk command to make partitions.\n");

	return -1;
}
#endif

static int set_partition_table_from_bootmode(void)
{
	/* checking boot mode before to set partition table	*/
	switch(OmPin) {
		case BOOT_ONENAND:
			if (set_partition_table()) {
				return 1;
			}
			break;
		case BOOT_MMCSD:
			emmc_dev = 1;
			sd_dev = 0;
		case BOOT_EMMC_4_4:
		case BOOT_EMMC:
			if (set_partition_table_sdmmc()) {
				return 1;
			}
			break;
		case BOOT_NAND:
			if (set_partition_table()) {
				return 1;
			}
			break;
	}
	return 0;
}

int do_fastboot(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	int ret = 1;
	int check_timeout = 0;
	uint64_t timeout_endtime = 0;
	uint64_t timeout_ticks = 0;
	long timeout_seconds = -1;
	int continue_from_disconnect = 0;
	struct fastboot_ptentry *ptn;
	unsigned int addr, size;
	gflag_reboot = 0;
	force_emmc = 1;

	if (set_partition_table_from_bootmode())
		return -1;

	if ((argc > 1) && (0 == strcmp(argv[1], "flash"))){
		ptn = fastboot_flash_find_ptn(argv[2]);
		if(ptn == NULL) {
			printf("undefined image name !\n");
			return -1;
		}
		size = 0;
		if(ptn->name[0] == 'r')
			size = PART_SIZE_ROOTFS;
		addr = simple_strtoul(argv[3], NULL, 16);
		write_to_ptn_sdmmc(ptn, addr, size);
		return 1;
	}

	/* Time out */
	if (2 == argc)
	{
		long try_seconds;
		char *try_seconds_end;
		/* Check for timeout */
		try_seconds = simple_strtol(argv[1], &try_seconds_end, 10);
		if ((try_seconds_end != argv[1]) && (try_seconds >= 0))
		{
			check_timeout = 1;
			timeout_seconds = try_seconds;
			printf("Fastboot inactivity timeout %ld seconds\n", timeout_seconds);
		}
	}

	if (1 == check_timeout)
	{
		timeout_ticks = (uint64_t) (timeout_seconds * get_tbclk());
	}


	do
	{
		continue_from_disconnect = 0;

		/* Initialize the board specific support */
		if (0 == fastboot_init(&interface))
		{
			int poll_status;

			/* If we got this far, we are a success */
			ret = 0;

			timeout_endtime = get_ticks();
			timeout_endtime += timeout_ticks;

			while (1)
			{
				uint64_t current_time = 0;
				poll_status = fastboot_poll();

				if (1 == check_timeout)
					current_time = get_ticks();

				/* Check if the user wanted to terminate with ^C */
				if ( ((poll_status != FASTBOOT_OK) && (ctrlc())) || gflag_reboot)
				{
					printf("Fastboot ended by user\n");
					continue_from_disconnect = 0;
					break;
				}

				if (FASTBOOT_ERROR == poll_status)
				{
					/* Error */
					printf("Fastboot error \n");
					break;
				}
				else if (FASTBOOT_DISCONNECT == poll_status)
				{
					/* break, cleanup and re-init */
					printf("Fastboot disconnect detected\n");
					continue_from_disconnect = 1;
					break;
				}
				else if ((1 == check_timeout) &&
					   (FASTBOOT_INACTIVE == poll_status))
				{
					/* No activity */
					if (current_time >= timeout_endtime)
					{
						printf("Fastboot inactivity detected\n");
						break;
					}
				}
				else
				{
					/* Something happened */
					/* Actual works of parsing are done by rx_handler */
					if (1 == check_timeout)
					{
						/* Update the timeout endtime */
						timeout_endtime = current_time;
						timeout_endtime += timeout_ticks;
					}
				}
			} /* while (1) */
		}

		/* Reset the board specific support */
		fastboot_shutdown();

#ifdef CONFIG_USE_LCD
		LCD_setfgcolor(0x000010);
		LCD_setleftcolor(0x000010);
		LCD_setprogress(100);
#endif

		/* restart the loop if a disconnect was detected */
	} while (continue_from_disconnect);

	force_emmc = 0;
	return ret;
}

U_BOOT_CMD(
	fastboot,	4,	1,	do_fastboot,
	"fastboot- use USB Fastboot protocol\n",
	"[inactive timeout]\n"
	"    - Run as a fastboot usb device.\n"
	"    - The optional inactive timeout is the decimal seconds before\n"
	"    - the normal console resumes\n"
);


#ifdef CONFIG_FASTBOOT_SDFUSE
/*
 * part : partition name (This should be a defined name at ptable)
 * file : file to read
 */
#include <fs.h>
extern struct ext2fs_node *ext4fs_file;
static int update_from_sd(int part_num, const char *part, const char *file)
{
	int ret = 1;
	long size;
	unsigned long addr;
	unsigned long offset = 0;
	unsigned long count;
	int file_len;
	char command[64];
	char response[64];
	block_dev_desc_t *dev_desc;
	disk_partition_t part_info;

	dev_desc = get_dev("mmc", sd_dev);
	if (dev_desc == NULL) {
		printf("** Invalid boot device **\n");
		return -1;
	}

	ret = get_partition_info(dev_desc, part_num, &part_info);
	if (ret) {
		printf("Invalid partition\n");
		return -1;
	}

	ret = ext4fs_probe(dev_desc, &part_info);
	if (ret) {
		printf("Cannot probe ext4 partition\n");
		return -1;
	}

	reset_handler();
	check_ptn_flash_chunk(part);

	addr = CFG_FASTBOOT_TRANSFER_BUFFER;

	file_len = ext4fs_open(file);
	if (file_len < 0) {
		printf("File not found %s\n", file);
		return -1;
	}

	download_size = file_len;
	if (download_ptn) {
		total_chunk = download_size / CHUNK_SIZE;
		if (download_size % CHUNK_SIZE)
			total_chunk++;
	}

	while (1) {
		count = file_len;
		if (count > CHUNK_SIZE)
			count = CHUNK_SIZE;

		size = ext4fs_read_file(ext4fs_file, offset, count, addr);
		if (size == -1) {
			printf("Failed to read %s\n", file);
			ret = -1;
			goto err_out;
		}

		if (download_ptn) {
			flash_chunk_data(addr, size, response);
			offset += size;
			if (offset >= file_len) {
				ret = 0;
				break;
			}
		} else {
			download_size = 0;
			download_bytes = size;

			printf("%ld (0x%08x) bytes read\n", size, size);
			sprintf(command, "flash:%s", part);
			ret = rx_handler(command, sizeof(command));
			break;
		}
	}

err_out:
	ext4fs_close();
	reset_handler();
	return ret;
}

/* SD Fusing : read images from EXT4 partition of SD Card,
 * and write it to boot device.
 *
 * NOTE
 * - sdfuse is not a original code of fastboot
 * - Fusing image from SD Card is not a original part of Fastboot protocol.
 * - This command implemented at this file
 *   to re-use an existing code of fastboot */
int do_sdfuse (cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	int ret = 1;
	int enable_reset = 0;
	struct mmc *mmc;
	int part_num = 2;
	char run_cmd[64];
	force_emmc = 1;

	interface.transfer_buffer =
		(unsigned char *)CFG_FASTBOOT_TRANSFER_BUFFER;
	interface.transfer_buffer_size = CFG_FASTBOOT_TRANSFER_BUFFER_SIZE;

	printf("[Fusing Image from SD Card.]\n");

	if (set_partition_table_from_bootmode())
		return -1;

	if ((argc == 2) && !strcmp(argv[1], "format")) {
		ret = process_cmd_format();
		sprintf(run_cmd, "mmc dev %d", emmc_dev);
		run_command(run_cmd, 0);
		sprintf(run_cmd, "mmc rescan");
		run_command(run_cmd, 0);
		if (emmc_dev) {
			sprintf(run_cmd, "mmc dev 0");
			run_command(run_cmd, 0);
		}
	} else if ((argc == 3) && !strcmp(argv[1], "flashall")) {
		part_num = simple_strtoul(argv[2], NULL, 16);
		update_from_sd(part_num, "fwbl1", "bl1.bin");
		update_from_sd(part_num, "bl2", "bl2.bin");
		update_from_sd(part_num, "tzsw", "tzsw.bin");
		update_from_sd(part_num, "bootloader", "u-boot.bin");
		update_from_sd(part_num, "env", "params.bin");

		if (update_from_sd(part_num, "boot", "boot.img")) {
			update_from_sd(part_num, "kernel",
					getenv("kernel_file"));
			update_from_sd(part_num, "ramdisk",
					getenv("initrd_file"));
			update_from_sd(part_num, "dtb",
					getenv("fdtfile"));
		}
		update_from_sd(part_num, "rootfs", "rootfs.img");
		update_from_sd(part_num, "data", "data.img");

		ret = 0;
	} else if ((argc == 5) && !strcmp(argv[1], "flash")) {
		if (update_from_sd(part_num, argv[3], argv[4]))
			return -1;

		ret = 0;
	} else {
		printf("Usage:\n%s\n", cmdtp->usage);
		return 1;
	}

	force_emmc = 0;

	return ret;
}

U_BOOT_CMD(
	sdfuse,		5,	1,	do_sdfuse,
	"sdfuse  - read images from EXT4 partition of SD card and write them to booting device.\n",
	"sdfuse part_no flashall                 - flash all images\n"
	"sdfuse format				 - format mmc partition\n"
	"sdfuse flash part_no <partition> [ <filename> ] - write a file to a partition.\n"
);
#endif	/* CONFIG_FASTBOOT_SDFUSE */

/*
 * Android style flash utilties */
void fastboot_flash_add_ptn(fastboot_ptentry *ptn)
{
	if (pcount < MAX_PTN)
	{
		memcpy(ptable + pcount, ptn, sizeof(*ptn));
		pcount++;
	}
}

void fastboot_flash_dump_ptn(void)
{
	unsigned int n;

	printf("[Partition table on ");

	switch(OmPin) {
		case BOOT_ONENAND:
			printf("OneNAND");
			break;
		case BOOT_MMCSD:
		case BOOT_EMMC_4_4:
		case BOOT_EMMC:
			printf("MoviNAND");
			break;
		case BOOT_NAND:
			printf("NAND");
			break;
		}
	printf("]\n");

	for (n = 0; n < pcount; n++)
	{
		fastboot_ptentry *ptn = ptable + n;
#if 0	/* old format - decimal */
		printf("ptn %d name='%s' start=%d len=%d\n",
				n, ptn->name, ptn->start, ptn->length);
#else
		printf("ptn %d name='%s' ", n, ptn->name);
		if (n == 0 || ptn->start)
			printf("start=0x%llx ", ptn->start);
		else
			printf("start=N/A ");
		if (ptn->length)
			printf("len=0x%llx(~%lldKB) ", ptn->length,
					ptn->length >> 10);
		else
			printf("len=N/A ");

		if (ptn->flags & FASTBOOT_PTENTRY_FLAGS_WRITE_YAFFS)
			printf("(Yaffs)\n");
		else if (ptn->flags & FASTBOOT_PTENTRY_FLAGS_USE_MOVI_CMD)
			printf("(use hard-coded info. (cmd: movi))\n");
		else
			printf("\n");
#endif
	}
}


fastboot_ptentry *fastboot_flash_find_ptn(const char *name)
{
	unsigned int n;

	for (n = 0; n < pcount; n++)
	{
		/* Make sure a substring is not accepted */
		if (strlen(name) == strlen(ptable[n].name))
		{
			if (0 == strcmp(ptable[n].name, name))
				return ptable + n;
		}
	}
	return 0;
}

fastboot_ptentry *fastboot_flash_get_ptn(unsigned int n)
{
	if (n < pcount) {
		return ptable + n;
	} else {
		return 0;
	}
}

unsigned int fastboot_flash_get_ptn_count(void)
{
	return pcount;
}



#endif	/* CONFIG_FASTBOOT */


