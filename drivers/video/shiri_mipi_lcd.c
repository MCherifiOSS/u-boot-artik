/*
 * Copyright (C) 2014 Samsung Electronics
 *
 * Author: Jaeyong Lee <jaeyong2.lee@samsung.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#include <common.h>
#include <asm/arch/mipi_dsim.h>

#include "exynos_mipi_dsi_lowlevel.h"
#include "exynos_mipi_dsi_common.h"
/* MIPI S6D04D2X01 MCS commands */
enum {
    MIPI_MCS_PASSWD1            = 0xF0,
    MIPI_MCS_PASSWD2            = 0xF1,
    MIPI_MCS_DISPCTL            = 0xF2,
    MIPI_MCS_MAINPWRSEQ     	= 0xF3,
    MIPI_MCS_PWRCTL         	= 0xF4,
    MIPI_MCS_VCMCTL         	= 0xF5,
    MIPI_MCS_SRCCTL         	= 0xF6,
    MIPI_MCS_IFCTL          	= 0xF7,
    MIPI_MCS_PANELCTL           = 0xF8,
    MIPI_MCS_IFCLK_CTL      	= 0xF9,
    MIPI_MCS_GAMMACTL           = 0xFA,
    MIPI_MCS_PASSWD3            = 0xFC,
    MIPI_MCS_MTPMVCTL           = 0xFE,
};

static void shiri_mipi_lcd_sleep_out(struct mipi_dsim_device *dsim_dev)
{
	struct mipi_dsim_master_ops *ops = dsim_dev->master_ops;

	ops->cmd_write(dsim_dev,
		MIPI_DSI_DCS_SHORT_WRITE, 0x11, 0x00);
}

static void shiri_mipi_lcd_sequence_on(struct mipi_dsim_device *dsim_dev)
{
	struct mipi_dsim_master_ops *ops = dsim_dev->master_ops;

	unsigned char MCS_CMD_01[7] = {0xE1, 0xF3, 0x10, 0x1C, 0x17, 0x08, 0x1D};
	unsigned char MCS_CMD_02[18]= {0xF2, 0x00, 0xD7, 0x03, 0x22, 0x23, 0x00, 0x01, 0x01, 0x12, 0x01, 0x08, 0x57, 0x00, 0x00, 0xD7, 0x22, 0x23 };
	unsigned char MCS_CMD_03[15]= {0xF4, 0x0B, 0x00, 0x00, 0x00, 0x21, 0x4F, 0x01, 0x02, 0x2A, 0x7F, 0x03, 0x2A, 0x00, 0x03 };
	unsigned char MCS_CMD_04[11]= {0xF5, 0x00, 0x30, 0x49, 0x00, 0x00, 0x18, 0x00, 0x00, 0x04, 0x04 };
	unsigned char MCS_CMD_05[10]= {0xF6, 0x02, 0x01, 0x06, 0x00, 0x02, 0x04, 0x02, 0x84, 0x06 };
	unsigned char MCS_CMD_06[2]	= {0xF7, 0x40 };
	unsigned char MCS_CMD_07[3]	= {0xF8, 0x33, 0x00 };
	unsigned char MCS_CMD_08[2]	= {0xF9, 0x00 };
	unsigned char MCS_CMD_09[2]	= {0x36, 0x08 };

	unsigned char MCS_CMD_E1[7]	= {0xE1, 0xF3, 0x10, 0x1C, 0x17, 0x08, 0x1D };
	unsigned char MCS_CMD_E2[6]	= {0xE2, 0xC3, 0x87, 0x39, 0x63, 0xD5 };
	unsigned char MCS_CMD_E3[4]	= {0xE3, 0x84, 0x06, 0x52 };
	unsigned char MCS_CMD_E4[4]	= {0xE4, 0x43, 0x00, 0x00 };
	ops->cmd_write(dsim_dev, MIPI_DSI_DCS_LONG_WRITE,
		(unsigned int)MCS_CMD_01, ARRAY_SIZE(MCS_CMD_01));

	ops->cmd_write(dsim_dev, MIPI_DSI_DCS_LONG_WRITE,
		(unsigned int)MCS_CMD_02, ARRAY_SIZE(MCS_CMD_02));

	ops->cmd_write(dsim_dev, MIPI_DSI_DCS_LONG_WRITE,
		(unsigned int)MCS_CMD_03, ARRAY_SIZE(MCS_CMD_03));

	ops->cmd_write(dsim_dev, MIPI_DSI_DCS_LONG_WRITE,
		(unsigned int)MCS_CMD_04, ARRAY_SIZE(MCS_CMD_04));

	ops->cmd_write(dsim_dev, MIPI_DSI_DCS_LONG_WRITE,
		(unsigned int)MCS_CMD_05, ARRAY_SIZE(MCS_CMD_05));

	ops->cmd_write(dsim_dev, MIPI_DSI_DCS_SHORT_WRITE_PARAM,
		MCS_CMD_06[0],MCS_CMD_06[1]);

	ops->cmd_write(dsim_dev, MIPI_DSI_DCS_LONG_WRITE,
		(unsigned int)MCS_CMD_07, ARRAY_SIZE(MCS_CMD_07));

	ops->cmd_write(dsim_dev, MIPI_DSI_DCS_SHORT_WRITE_PARAM,
		MCS_CMD_08[0],MCS_CMD_08[1]);

	ops->cmd_write(dsim_dev, MIPI_DSI_DCS_LONG_WRITE,
		(unsigned int)MCS_CMD_09, ARRAY_SIZE(MCS_CMD_09));

	ops->cmd_write(dsim_dev, MIPI_DSI_DCS_LONG_WRITE,
		(unsigned int)MCS_CMD_E1, ARRAY_SIZE(MCS_CMD_E1));

	ops->cmd_write(dsim_dev, MIPI_DSI_DCS_LONG_WRITE,
		(unsigned int)MCS_CMD_E2, ARRAY_SIZE(MCS_CMD_E2));

	ops->cmd_write(dsim_dev, MIPI_DSI_DCS_LONG_WRITE,
		(unsigned int)MCS_CMD_E3, ARRAY_SIZE(MCS_CMD_E3));

	ops->cmd_write(dsim_dev, MIPI_DSI_DCS_LONG_WRITE,
		(unsigned int)MCS_CMD_E4, ARRAY_SIZE(MCS_CMD_E4));

	ops->cmd_write(dsim_dev, MIPI_DSI_DCS_SHORT_WRITE, 
		0x11, 0x00);
}

static void shiri_mipi_lcd_display_on(struct mipi_dsim_device *dsim_dev)
{
	struct mipi_dsim_master_ops *ops = dsim_dev->master_ops;

	ops->cmd_write(dsim_dev, MIPI_DSI_DCS_SHORT_WRITE,
		0x29, 0);
}

static int shiri_mipi_lcd_suspend(struct mipi_dsim_device *dsim_dev)
{
	unsigned char displayoff[] = {0x28};
	unsigned char sleepin[] = {0x10};
	unsigned char MCS_CMD_F1[3] = {MIPI_MCS_PASSWD2, 0x5A, 0x5A};
	unsigned char MCS_CMD_F1_2[3] = {MIPI_MCS_PASSWD2, 0xA5, 0xA5};
	unsigned char MCS_CMD_F4[15] = {MIPI_MCS_PWRCTL, 0x07, 0x00, 0x00, 0x00, 0x21, 0x4F, 0x01, 0x0E, 0x2A, 0x66, 
									0x02, 0x2A, 0x00, 0x02};
	int ret = 0;
	struct mipi_dsim_master_ops *ops = dsim_dev->master_ops;

	ops->cmd_write(dsim_dev, MIPI_DSI_DCS_LONG_WRITE,
		(unsigned int)displayoff, ARRAY_SIZE(displayoff));

	mdelay(10);

	ops->cmd_write(dsim_dev, MIPI_DSI_DCS_LONG_WRITE,
		(unsigned int)MCS_CMD_F1, ARRAY_SIZE(MCS_CMD_F1));

	ops->cmd_write(dsim_dev, MIPI_DSI_DCS_LONG_WRITE,
		(unsigned int)MCS_CMD_F4, ARRAY_SIZE(MCS_CMD_F4));

	ops->cmd_write(dsim_dev, MIPI_DSI_DCS_LONG_WRITE,
		(unsigned int)MCS_CMD_F1_2, ARRAY_SIZE(MCS_CMD_F1_2));

	mdelay(120);

	ops->cmd_write(dsim_dev, MIPI_DSI_DCS_LONG_WRITE,
		(unsigned int)sleepin, ARRAY_SIZE(sleepin));
		
	return 1;
}

static void shiri_mipi_lcd_panel_init(struct mipi_dsim_device *dsim_dev)
{
	/* power setting */
	shiri_mipi_lcd_sequence_on(dsim_dev);
	mdelay(5);
}

static int shiri_mipi_lcd_panel_set(struct mipi_dsim_device *dsim_dev)
{
	shiri_mipi_lcd_panel_init(dsim_dev);

	return 0;
}

static void shiri_mipi_lcd_display_enable(struct mipi_dsim_device *dsim_dev)
{
	shiri_mipi_lcd_display_on(dsim_dev);
}

static struct mipi_dsim_lcd_driver shiri_mipi_lcd_dsim_ddi_driver = {
	.name = "shiri_mipi_lcd",
	.id = -1,

	.mipi_panel_init = shiri_mipi_lcd_panel_set,
	.mipi_display_on = shiri_mipi_lcd_display_enable,
};

void shiri_mipi_lcd_init(void)
{
	exynos_mipi_dsi_register_lcd_driver(&shiri_mipi_lcd_dsim_ddi_driver);
}
