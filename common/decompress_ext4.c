/* copyright (c) 2010 Samsung Electronics Co., Ltd.
 *              http://www.samsung.com/
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
*/

#include <common.h>
#include <asm/io.h>
#include <asm/types.h>
#include <environment.h>
#include <command.h>
#include <fastboot.h>

#include <asm/errno.h>
#include <decompress_ext4.h>
#include <mmc.h>
#include <div64.h>

static ext4_chunk_header chunk_header;
static ext4_file_header *file_header;
static int total_chunks;
static int remain_chunk_sectors;
static unsigned int sector_base;

static int residue_bytes;
static char residue_buf[512];

#define SECTOR_BITS		9	/* 512B */
#define SECTOR_SIZE		(1 << SECTOR_BITS)

#define ext4_printf(args, ...)

static int write_raw_chunk(char* data, unsigned int sector, unsigned int sector_size);


int check_compress_ext4(char *img_base, unsigned long long parti_size) {
	ext4_file_header *file_header;

	file_header = (ext4_file_header*)img_base;

	if (file_header->magic != EXT4_FILE_HEADER_MAGIC) {
		return -1;
	}

	if (file_header->major != EXT4_FILE_HEADER_MAJOR) {
		printf("Invalid Version Info! 0x%2x\n", file_header->major);
		return -1;
	}

	if (file_header->file_header_size != EXT4_FILE_HEADER_SIZE) {
		printf("Invalid File Header Size! 0x%8x\n",
								file_header->file_header_size);
		return -1;
	}

	if (file_header->chunk_header_size != EXT4_CHUNK_HEADER_SIZE) {
		printf("Invalid Chunk Header Size! 0x%8x\n",
								file_header->chunk_header_size);
		return -1;
	}

	if (file_header->block_size != EXT4_FILE_BLOCK_SIZE) {
		printf("Invalid Block Size! 0x%8x\n", file_header->block_size);
		return -1;
	}

	if ((parti_size/file_header->block_size)  < file_header->total_blocks) {
		printf("Invalid Volume Size! Image is bigger than partition size!\n");
		printf("partion size %ld , image size %d \n",
			(do_div(parti_size, file_header->block_size)),
			file_header->total_blocks);
		while(1);
	}

	/* image is compressed ext4 */
	return 0;
}

int write_raw_chunk(char* data, unsigned int sector, unsigned int sector_size) {
	char run_cmd[64];
#if defined(CONFIG_MMC_64BIT_BUS) || defined(CONFIG_CPU_EXYNOS5410_EVT2)
	unsigned char *tmp_align;

	if (((unsigned int)data % 8) != 0) {
		tmp_align = (char *)CFG_FASTBOOT_MMC_BUFFER;
		memcpy((unsigned char *)tmp_align, (unsigned char *)data, sector_size * 512);
		data = tmp_align;
	}
#endif
	ext4_printf("write raw data in %d size %d \n", sector, sector_size);
	sprintf(run_cmd,"mmc write 0 0x%x 0x%x 0x%x", (int)data, sector, sector_size);
	run_command(run_cmd, 0);

	return 0;
}

ext4_file_header *init_compressed_ext4(char *img_base)
{
	ext4_file_header *f_hdr;

	f_hdr = (ext4_file_header *)malloc(sizeof(ext4_file_header));
	if (!f_hdr)
		return NULL;

	memcpy(f_hdr, img_base, sizeof(ext4_file_header));
	total_chunks = f_hdr->total_chunks;
	ext4_printf("total chunk = %d\n", total_chunks);

	return f_hdr;
}

void exit_compressed_ext4(void)
{
	if (file_header)
		free(file_header);
	file_header = NULL;
	total_chunks = 0;
	remain_chunk_sectors = 0;
	residue_bytes = 0;
	sector_base = 0;
}

int write_compressed_ext4(char *img_base, unsigned int buflen,
			  unsigned int base, unsigned int length)
{
	unsigned int sector_size;
	int remain_bytes = buflen;

	if (!file_header) {
		/* If there is no file_header, it means the first download
		 * stage. First of all, we need to parse the file header */
		file_header = init_compressed_ext4(img_base);
		if (!file_header)
			return -1;
		img_base += file_header->file_header_size;
		remain_bytes -= file_header->file_header_size;
		sector_base = base;
	}

	/* Flash buffer until there is no remain buffer */
	while (remain_bytes > 0) {
		if (!remain_chunk_sectors) {
			if (remain_bytes < sizeof(ext4_chunk_header)) {
				printf("Cannot parse chunk_header\n");
				return -1;
			}

			memcpy(&chunk_header, img_base,
					sizeof(ext4_chunk_header));
			img_base += file_header->chunk_header_size;
			remain_bytes -= file_header->chunk_header_size;

			sector_size = (chunk_header.chunk_size *
				file_header->block_size) >> SECTOR_BITS;
			remain_chunk_sectors = sector_size;
		} else
			sector_size = remain_chunk_sectors;

		switch (chunk_header.type) {
		case EXT4_CHUNK_TYPE_RAW:
			ext4_printf("raw_chunk\n");
			if (residue_bytes) {
				/* If there is residue buffer, fill a sector
				 * and flash it */
				memcpy(residue_buf + residue_bytes, img_base,
						SECTOR_SIZE - residue_bytes);
				write_raw_chunk(residue_buf, sector_base, 1);
				/* Move a location of sector base */
				sector_base++;
				img_base += (SECTOR_SIZE - residue_bytes);
				sector_size--;
				remain_chunk_sectors--;
				remain_bytes -= (SECTOR_SIZE - residue_bytes);
				residue_bytes = 0;
			}

			if (sector_size > (remain_bytes >> SECTOR_BITS)) {
				/* If we need to keep residue buffer, we store
				 * them into residue_buf and flash them during
				 * next download */
				residue_bytes = remain_bytes -
					((remain_bytes >> SECTOR_BITS)
					 << SECTOR_BITS);
				sector_size = remain_bytes >> SECTOR_BITS;
				if (residue_bytes) {
					memcpy(residue_buf, img_base +
						(sector_size << SECTOR_BITS),
						residue_bytes);
				}
			}

			if (sector_size) {
				write_raw_chunk(img_base, sector_base,
						sector_size);
				sector_base += sector_size;
				img_base += (sector_size << SECTOR_BITS);
				remain_bytes -= ((sector_size << SECTOR_BITS)
						+ residue_bytes);
			}
			break;

		case EXT4_CHUNK_TYPE_FILL:
			sector_base += sector_size;
			img_base += (chunk_header.total_size -
					file_header->chunk_header_size);
			remain_bytes -= (chunk_header.total_size -
					file_header->chunk_header_size);
			break;

		case EXT4_CHUNK_TYPE_NONE:
			ext4_printf("none chunk\n");
			sector_base += sector_size;
			img_base += (chunk_header.total_size -
					file_header->chunk_header_size);
			remain_bytes -= (chunk_header.total_size -
					file_header->chunk_header_size);
			break;

		default:
			ext4_printf("*** unknown chunk type ***\n");
			/* If unknown chunk type is founded, return error */
			exit_compressed_ext4();
			return -1;
		}
		remain_chunk_sectors -= sector_size;
		if (remain_chunk_sectors == 0)
			total_chunks--;

		if (total_chunks == 0) {
			ext4_printf("write done\n");
			exit_compressed_ext4();
			return 0;
		}
		ext4_printf("remain chunks = %d\n", total_chunks);
	};

	return 0;
}
