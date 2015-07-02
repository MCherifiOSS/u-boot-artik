/*
 * (C) Copyright 2014 Samsung Electronics Co. Ltd
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
#include <command.h>

#include <video_font.h>
#include <video_font_data.h>

#define DEBUG_CMD_LCDTEXT
#undef DEBUG_CMD_LCDTEXT

#ifdef DEBUG_CMD_LCDTEXT
#define dbg(x...)       printf(x)
#else
#define dbg(x...)       do { } while (0)
#endif

#ifdef CONFIG_FB_ADDR
#define CFG_LCD_FBUFFER		CONFIG_FB_ADDR
#else
#define CFG_LCD_FBUFFER		(0x59000000)
#endif
#define LCD_WIDTH	240
#define LCD_HEIGHT	240

#define RGB2888(r, g, b)	(unsigned int)((r) << 16 | (g) << 8 | (b))

#define RGB2PIXEL(r, g, b) 	RGB2888(r, g, b)

#define COLOR_INVALID		(RGB2PIXEL(9, 1, 1))
static unsigned int s_text_color = RGB2PIXEL(58, 110, 165);
static unsigned int s_text_color_bg = COLOR_INVALID;

static int s_last_x = 0;
static int s_last_y = 0;

static const unsigned char * getGlyph(unsigned char ascii)
{
	return video_fontdata + ((int)ascii * 16);		//size 8x16
}

int do_draw_text (cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	int x, y, w, h, strike, i, j, k;

	unsigned int * data, * base;
	unsigned char str[256];		//256 characters buffer!
	unsigned char * glyph;

	base = CFG_LCD_FBUFFER;
	w = LCD_WIDTH;
	h = LCD_HEIGHT;
	strike = w;

	if(argc == 2)
	{
		x = s_last_x;
		y = s_last_y + 20;
		str[0] = '\0';
		for(i = 1; i < argc; i++)
		{
			if(argv[i])
			{
				strcat(str, argv[i]);
				strcat(str, " ");
			}
		}

	}
	else if(argc == 4)
	{
		x = simple_strtoul(argv[1], NULL, 10);
		y = simple_strtoul(argv[2], NULL, 10);

		str[0] = '\0';
		for(i = 3; i < argc; i++)
		{
			if(argv[i])
			{
				strcat(str, argv[i]);
				strcat(str, " ");
			}
		}
	}
	else
	{
		printf ("Usage:\n%s\n", cmdtp->usage);
		return 1;
	}

	s_last_x = x;
	s_last_y = y;


	printf ("lcdtext draw: %s\n", str);

	if(x > w || y > h) 
	{	
		printf ("out of range, x=%d, y=%d, width=%d, height=%d\n", x, y, w, h);
		return 1;
	}
	
	if(!argv[3]) 
	{
		printf ("no string input\n");
		return 1;
	}

	for(i = 0; str[i] != '\0'; i++)
	{
		x += VIDEO_FONT_WIDTH;
		x += 2;
		if(x > w - 32)
			break;	//draw end

		data = base + y * strike + x;
		glyph = getGlyph(str[i]);
		for(j = 0; j < VIDEO_FONT_HEIGHT; j++)
		{	
			unsigned char mask = 0x80;
			for(k = 0; k < VIDEO_FONT_WIDTH; k++)
			{
				if(glyph[j] & mask)
				{	
					data[k] = s_text_color;
					dbg("#");
				}
				else
				{
					if(s_text_color_bg != COLOR_INVALID)
						data[k] = s_text_color_bg;
					dbg(" ");
				}
				mask >>= 1;
			}
			data += strike;
			dbg("\n");
		}
	}
#ifdef CONFIG_MACH_SHIRI
	SWTrigger_for_te();
#endif
	return 0;
}

int do_set_text_color (cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	int r, g, b;
	if (argc < 4) 
	{
		printf ("Usage:\n%s\n", cmdtp->usage);
		return 1;
	}

	r = simple_strtoul(argv[1], NULL, 10);
	g = simple_strtoul(argv[2], NULL, 10);
	b = simple_strtoul(argv[3], NULL, 10);
	s_text_color = RGB2PIXEL(r, g, b);
	printf ("set text color to [%d, %d, %d]\n", r, g, b);
	return 0;
}

int do_set_text_bg_color (cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	int r, g, b;
	if (argc < 4) 
	{
		printf ("Usage:\n%s\n", cmdtp->usage);
		return 1;
	}

	r = simple_strtoul(argv[1], NULL, 10);
	g = simple_strtoul(argv[2], NULL, 10);
	b = simple_strtoul(argv[3], NULL, 10);
	s_text_color_bg = RGB2PIXEL(r, g, b);
	printf ("set text background color to [%d, %d, %d]\n", r, g, b);
	return 0;
}

U_BOOT_CMD(
	lcdtext, CONFIG_SYS_MAXARGS, 1,	do_draw_text,
	"lcdtext    - lcdtext [your text]\nlcdtext    - lcdtext [x] [y] [your text]\n",
	"draw text on screen, default font 8x8 ascii, color[58, 110, 165]\n"
);

U_BOOT_CMD(
	lcdtextcolor, CONFIG_SYS_MAXARGS, 1,	do_set_text_color,
	"lcdtextcolor   - lcdtextcolor [R] [G] [B]\n",
	"set text color in RGB\n"
);

U_BOOT_CMD(
	lcdtextcolorbg, CONFIG_SYS_MAXARGS, 1,	do_set_text_bg_color,
	"lcdtextcolorbg   - lcdtextcolorbg [R] [G] [B]\n",
	"set text background color in RGB\n"
);
