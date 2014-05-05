/*
 * Copyright (C) 2011 Samsung Electrnoics
 * Lukasz Majewski <l.majewski@samsung.com>
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
 * aloong with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#ifndef __USB_MASS_STORAGE_H__
#define __USB_MASS_STORAGE_H__

#define SECTOR_SIZE		0x200
#include <part.h>
#include <linux/usb/composite.h>

/* Wait at maximum 60 seconds for cable connection */
#define UMS_CABLE_READY_TIMEOUT	60

struct ums {
	int (*read_sector)(struct ums *ums_dev,
			   ulong start, lbaint_t blkcnt, void *buf);
	int (*write_sector)(struct ums *ums_dev,
			    ulong start, lbaint_t blkcnt, const void *buf);
	unsigned int start_sector;
	unsigned int num_sectors;
	const char *name;
	block_dev_desc_t *block_dev;
};

extern struct ums *ums;

int fsg_init(struct ums *);
void fsg_cleanup(void);
int fsg_main_thread(void *);
int fsg_add(struct usb_configuration *c);
#endif /* __USB_MASS_STORAGE_H__ */
