/*
 * (C) Copyright 2005
 * Wolfgang Denk, DENX Software Engineering, wd@denx.de.
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
#include <command.h>
#include <led-display.h>

#undef DEBUG_LCD

#ifdef CONFIG_USE_LCD
extern void Lcd_read_bootlogo(void);
extern void Backlight_Turnon(void);
extern void Display_Turnon(void);
extern void Display_Turnoff(void);
extern void s5p_lcd_draw_bootlogo(void);
extern void LCD_setprogress(int percentage);
extern void LCD_clear_content(void);
#endif

int do_lcd (cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	char cmd;

	if (argc < 2)
		return (0);

	cmd=argv[1][0];

	switch(cmd-'0')
	{
		case 0:
			printf("turnon\n");
			Display_Turnon();
			Lcd_read_bootlogo();
			s5p_lcd_draw_bootlogo();
			Backlight_Turnon();
			break;
		case 1:
			printf("turnoff\n");
			Display_Turnoff();
			break;
		case 2:
			printf("clear with black color\n");
			LCD_clear_content();
			break;
		case 3:
			printf("showprog with 100%\n");
			LCD_setprogress(100);
			break;
		case 4:
			printf("turnon_clear_black\n");
			Display_Turnon();
			LCD_clear_content();
			Backlight_Turnon();
			break;
		default:
			switch(cmd-'a')
			{
				default:
					printf("unknown command\n");
					return (0);
			}
	}

	return (0);
}

/***************************************************/

U_BOOT_CMD(
	lcd,	CONFIG_SYS_MAXARGS,	2,	do_lcd,
	"control lcd for MIPI",
	"<command number> [argument]\n"
	"    - turnon : 0\n"
	"    - turnoff : 1\n"
	"    - clear : 2, clear with 0x00000000 color\n"
	"    - showprog : 3, arguments is percentage, decimal\n"
	"    - clear_turnon : 4, turnon and clear with black color\n"
);
