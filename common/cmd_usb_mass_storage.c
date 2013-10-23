/*
 * Copyright (C) 2011 Samsung Electronics
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
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#include <common.h>
#include <command.h>
#include <g_dnl.h>
#include <usb.h>
#include <usb_mass_storage.h>

int do_usb_mass_storage(cmd_tbl_t *cmdtp, int flag,
			       int argc, char * const argv[])
{
	if (argc < 3)
		return CMD_RET_USAGE;

	const char *usb_controller = argv[1];
	const char *mmc_devstring  = argv[2];

	unsigned int dev_num = simple_strtoul(mmc_devstring, NULL, 0);

	struct ums *ums = ums_init(dev_num);
	if (!ums)
		return CMD_RET_FAILURE;

	unsigned int controller_index = (unsigned int)(simple_strtoul(
					usb_controller,	NULL, 0));
	if (board_usb_init(controller_index, USB_INIT_DEVICE)) {
		error("Couldn't init USB controller.");
		return CMD_RET_FAILURE;
	}

	int rc = fsg_init(ums);
	if (rc) {
		error("fsg_init failed");
		return CMD_RET_FAILURE;
	}

	g_dnl_register("ums");

	while (1) {
		/* Handle control-c and timeouts */
		if (ctrlc()) {
			error("The remote end did not respond in time.");
			goto exit;
		}

		usb_gadget_handle_interrupts();
		/* Check if USB cable has been detached */
		if (fsg_main_thread(NULL) == EIO)
			goto exit;
	}
exit:
	g_dnl_unregister();
	return CMD_RET_SUCCESS;
}

U_BOOT_CMD(ums, CONFIG_SYS_MAXARGS, 1, do_usb_mass_storage,
	"Use the UMS [User Mass Storage]",
	"ums <USB_controller> <mmc_dev>  e.g. ums 0 0"
);
