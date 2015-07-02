/*
 * (C) Copyright 2012 Samsung Electronics Co. Ltd
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
#include <asm/io.h>
#include <lcd.h>
#include <asm/arch-exynos/movi_partition.h>

#ifdef CONFIG_FB_ADDR
#define CFG_LCD_FBUFFER		CONFIG_FB_ADDR
#else
#define CFG_LCD_FBUFFER		(0x46000000)
#endif
#define LCD_WIDTH	240
#define LCD_HEIGHT	240
#define LCD_BGCOLOR	0x1428A0

void LCD_Backlight_Off(void);
void LCD_Backlight_On(void);
void LCD_Stop(void);
void LCD_turnoff(void);
void LCD_turnon(void);
#ifdef CONFIG_MACH_SHIRI
static void SWTrigger_for_te(void);
#endif

// Use Just test purpose to fill dummy pattern in Lcd_read_bootlogo()
#define USE_LCD_TEST_PATTERN (0)
#define USE_RAW_IMAGE (0)

#define BAT_CHG_BITMAP_WIDTH 260
#define BAT_CHG_BITMAP_HEIGHT 124
#define BAT_CHG_BITMAP_LEN (BAT_CHG_BITMAP_WIDTH*BAT_CHG_BITMAP_HEIGHT*4)

#define LOGO_RESERVED_ADDR  (0x47000000)

unsigned char *boot_logo[4];	// 4frame animation logo set. currently not used.
unsigned char *pboot_logo_data;
unsigned char *pboot_logo_data2;

unsigned char *charger_logo[4];
unsigned char *pcharger_logo_data;

static unsigned int gFgColor = 0xFF;
static unsigned int gLeftColor = LCD_BGCOLOR;

static int lcd_init(void *lcdbase)
{
	lcd_ctrl_init(lcdbase);

	lcd_enable();

	return 0;
}

void LCD_turnon(void)
{
	pmic_turnon_vdd_lcd(); 	//ldo poower on

	lcd_base = CONFIG_FB_ADDR;

	lcd_init(lcd_base);		/* LCD initialization */
}

void LCD_Stop(void)
{
	exynos_mipi_dsi_stop();

	exynos_fimd_lcd_off();
}

void LCD_turnoff(void)
{
	pmic_turnoff_vdd_lcd();
}

void Display_Turnon(void)
{
	Backlight_Turnoff();
	LCD_turnon();
	printf("Turn on Display\n");
}

void Backlight_Turnon(void)
{
	backlight_en(1);
}

void Backlight_Turnoff(void)
{
	backlight_en(0);
}

void Display_Turnoff(void)
{
	LCD_Stop();
	Backlight_Turnoff();
	LCD_turnoff();

	printf("Turn off Display\n");
}

void LCD_setfgcolor(unsigned int color)
{
	gFgColor = color;
}

void LCD_setleftcolor(unsigned int color)
{
	gLeftColor = color;
}

void LCD_clear_content(void)
{
	memset((void*)CFG_LCD_FBUFFER, 0x00, LCD_WIDTH*LCD_HEIGHT*4);	
#ifdef CONFIG_MACH_SHIRI
	SWTrigger_for_te();
#endif
}

void LCD_setprogress(int percentage)
{

	u32 i, j;
	u32* pBuffer = (u32*)CFG_LCD_FBUFFER;
	u32 BAR_WIDTH = 30;
	u32 BAR_LENGTH = (LCD_WIDTH/100)*80;
	u32 sy = (LCD_HEIGHT-BAR_WIDTH)*2/3;
	u32 ey = (LCD_HEIGHT+BAR_WIDTH)*2/3;
	u32 sx = (LCD_WIDTH-BAR_LENGTH)/2;
	u32 ex = (LCD_WIDTH+BAR_LENGTH)/2;
	u32 syy = sy-10;
	u32 eyy = ey+10;
	u32 sxx = sx-10;
	u32 exx = ex+10;

	for(i = sxx; i < exx; i++)
	{
		for (j = syy; j < eyy; j++)
		{
			*(pBuffer+(j*LCD_WIDTH+i)) = LCD_BGCOLOR;
		}
	}

	for (i = sx; i < (ex/100)*percentage; i++)
	{
		for (j = sy; j < ey; j++)
		{
			*(pBuffer+(j*LCD_WIDTH+i)) = gFgColor;
		}
	}

	for (; i < ex; i++)
	{
		for (j=sy; j < ey; j++)
		{
			*(pBuffer+(j*LCD_WIDTH+i)) = gLeftColor;
		}
	}
#ifdef CONFIG_MACH_SHIRI
	SWTrigger_for_te();
#endif
}

void s5p_lcd_draw_bitmap32(const unsigned char *buf)
{
	unsigned long i, j, sx, sy;
	unsigned long* pBuffer = (unsigned long*)CFG_LCD_FBUFFER;
	unsigned long* pBitmap = (unsigned long*)buf;
	unsigned long iBitmapData;
	printf("logobuffer:0x%x\n", buf);

	// at default middle
	sx = (LCD_WIDTH-BAT_CHG_BITMAP_WIDTH)>>1;
	sy = (LCD_HEIGHT-BAT_CHG_BITMAP_HEIGHT)>>1;
	sy -= 20;

	for (i = sy; i < sy+BAT_CHG_BITMAP_HEIGHT; i++)
	{
		for (j = sx; j < sx+BAT_CHG_BITMAP_WIDTH; j++)
		{
			iBitmapData = *pBitmap | 0xFF<<24;
			*(pBuffer+(i*LCD_WIDTH)+j) = iBitmapData;
			pBitmap++;				
		}
	}
#ifdef CONFIG_MACH_SHIRI
	SWTrigger_for_te();
#endif
}

void s5p_lcd_draw_bitmap16(const unsigned char *buf)
{
	unsigned long i, j, sx, sy;
	unsigned long* pBuffer = (unsigned long*)CFG_LCD_FBUFFER;
	unsigned long* pBitmap = (unsigned long*)buf;
	unsigned long iBitmapData;
	printf("logobuffer:0x%x\n", buf);

	sx = (LCD_WIDTH-BAT_CHG_BITMAP_WIDTH)>>1;
	sy = (LCD_HEIGHT-BAT_CHG_BITMAP_HEIGHT)>>1;
	sy -= 20;

	for (i = sy; i < sy+BAT_CHG_BITMAP_HEIGHT; i++)
	{
		for (j = sx; j < sx+BAT_CHG_BITMAP_WIDTH*2; j+=2)
		{
			iBitmapData = 0xFF<<24;
			iBitmapData |= ((*pBitmap>>8)&0xF8)<<16;
			iBitmapData |= ((*pBitmap>>5)&0x3F)<<10;
			iBitmapData |= ((*pBitmap<<3)&0xF8);

			*(pBuffer+(i*LCD_WIDTH)+j) = iBitmapData;
			*(pBuffer+(i*LCD_WIDTH)+(j+1)) = iBitmapData;
			pBitmap++;
		}
	}
}

unsigned int s5p_lcd_search_start_blk(const unsigned char *partition_name)
{
	int i = 0;
	for(i = 0; i < 16; i++)
	{
		if(!strncmp(partition_name, raw_area_control.image[i].description, 16))
		{
			printf("found %s partition : startblk=0x%x\n", raw_area_control.image[i].description, raw_area_control.image[i].start_blk);
			return raw_area_control.image[i].start_blk;
		}
	}
	printf("cannot find partition %s\n", partition_name);
	return 0;
}

void Lcd_read_charger_logo(void)
{
	char run_cmd[100];
	ulong rfs_size;
	uint start_blk, blkcnt;

	rfs_size = (512 * 1024);

	pcharger_logo_data = LOGO_RESERVED_ADDR;

	start_blk = s5p_lcd_search_start_blk("charger");
	blkcnt = rfs_size/512 + ((rfs_size&(512-1)) ? 1 : 0);
	printf("%s bootlogo.. %ld, %ld \n", "reading",start_blk, blkcnt);
	sprintf(run_cmd,"mmc %s 0 %lx %lx %lx","read",pcharger_logo_data, start_blk, blkcnt);

	run_command(run_cmd,0);

	charger_logo[0] = pcharger_logo_data;
	charger_logo[1] = (unsigned char *)(pcharger_logo_data + BAT_CHG_BITMAP_LEN);
	charger_logo[2] = (unsigned char *)(pcharger_logo_data + BAT_CHG_BITMAP_LEN*2);
	charger_logo[3] = (unsigned char *)(pcharger_logo_data + BAT_CHG_BITMAP_LEN*3);
}

#define BMP_FILE_TYPE 0x4d42
typedef struct tag_bmp_file_header
{ // bmfh
        unsigned short bfType;
        unsigned long bfSize;
        unsigned long bfReserved;
        unsigned long bfOffBits;

        unsigned long biSize;
        long int biWidth;
        long int biHeight;
        unsigned short biPlanes;
        unsigned short biBitCount;
        unsigned long biCompression;
        unsigned long biSizeImage;
        long int biXPelsPerMeter;
        long int biYPelsPerMeter;
        unsigned long biClrUsed;
        unsigned long biClrImportant;
}__attribute__ ((packed)) bmp_info_header;

bmp_info_header logo_bmp_info_head;

void s5p_lcd_draw_bootlogo(void)
{
#if USE_LCD_TEST_PATTERN
	int i, j;
	unsigned height = LCD_HEIGHT / 3;
	printf("height:%d\n",height);

	for (i = 0; i < height; i++){
		for(j = 0; j < LCD_WIDTH; j++){
			*((unsigned int *)(CFG_LCD_FBUFFER + (i * LCD_WIDTH + j) * 4)) = 0x0000ff00;
		}
	}
	printf("i:%d, j:%d\n", i, j);
	for (i = height; i < height*2; i++){
		for(j = 0; j < LCD_WIDTH; j++){
			*((unsigned int *)(CFG_LCD_FBUFFER + (i * LCD_WIDTH + j) * 4)) = 0x00ff0000;		
		}
	}
	for (i = height*2; i < height*3; i++){
		for(j = 0; j < LCD_WIDTH; j++){
			*((unsigned int *)(CFG_LCD_FBUFFER + (i * LCD_WIDTH + j) * 4)) = 0x000000ff;
		}
	}
#else
#if USE_RAW_IMAGE
#else
	unsigned long i, j, bmp_width, bmp_height, bmp_offset;
	unsigned long* pBuffer = (unsigned long*)CFG_LCD_FBUFFER;
	unsigned long* pBufferEnd = pBuffer + (LCD_HEIGHT*LCD_WIDTH);
	unsigned char* pBitmap;
	unsigned long* pBitmapLong = (unsigned long*)pBitmap;
	unsigned long iBitmapData;

	memcpy(&logo_bmp_info_head, (const void*)pboot_logo_data, sizeof(bmp_info_header));
	if(BMP_FILE_TYPE!=logo_bmp_info_head.bfType || !logo_bmp_info_head.bfSize) {
		printf("Err: Check logo format not match !\n");
		memcpy(CFG_LCD_FBUFFER, pboot_logo_data, LCD_WIDTH * LCD_HEIGHT * 4);
		return;
	}

	bmp_width = logo_bmp_info_head.biWidth;
	bmp_height = logo_bmp_info_head.biHeight;
	bmp_offset = logo_bmp_info_head.bfOffBits;

	pBitmap = (unsigned char*)pboot_logo_data + bmp_offset;

	for (i = 0; i < bmp_height; i++)
	{
		for (j = 0; j < bmp_width; j++)
		{
			iBitmapData = *pBitmap | *(pBitmap+1)<<8 | *(pBitmap+2)<<16 | 0xFF<<24;
			*(pBuffer+(LCD_HEIGHT*LCD_WIDTH)-(i*LCD_WIDTH)+j) = iBitmapData;
			pBitmap+=3;
		}
	}
#endif
#endif
#ifdef CONFIG_MACH_SHIRI
	SWTrigger_for_te();
#endif
}

void Lcd_read_bootlogo(void)
{
#if USE_LCD_TEST_PATTERN 
	// 3 Color bar Pattern for LCD test does not require reading logo
#else   
	char run_cmd[100];
	uint start_blk, blkcnt;

#if USE_RAW_IMAGE
	pboot_logo_data = CFG_LCD_FBUFFER;
#else
	pboot_logo_data = LOGO_RESERVED_ADDR;
#endif

	start_blk = s5p_lcd_search_start_blk("bootlogo");
	blkcnt = LCD_WIDTH*LCD_HEIGHT*4/512;	// for RAW 
	printf("%s bootlogo.. %ld, %ld, %08x  %08x\n", "reading",start_blk, blkcnt, pboot_logo_data, pboot_logo_data2);


	// Reading Logo area does not require emmc Open/Close
#if USE_RAW_IMAGE
	sprintf(run_cmd,"mmc read 0 %lx %lx %lx ", CFG_LCD_FBUFFER, start_blk, blkcnt);
#else
	sprintf(run_cmd,"movi r z l 0 %lx ", pboot_logo_data);
#endif
	run_command(run_cmd,0);	
#ifdef CONFIG_SHIRI_LCD
#if USE_RAW_IMAGE
// RGB SWAP (ABGR -> ARGB)
	int i, j;
	unsigned int tmpB, tmpR, pixel;

	for (i = 0; i < LCD_HEIGHT; i++){
		for(j = 0; j < LCD_WIDTH; j++){
			pixel = *((unsigned int *)(CFG_LCD_FBUFFER + (i * LCD_WIDTH + j) *4 ));
			tmpB = pixel & 0x00ff0000;
			tmpR = pixel & 0x000000ff;
			tmpB = tmpB >> 16;
			*((unsigned int *)(CFG_LCD_FBUFFER + (i * LCD_WIDTH + j) *4 )) &= 0xff00ff00;
			*((unsigned int *)(CFG_LCD_FBUFFER + (i * LCD_WIDTH + j) *4 )) |= tmpR << 16 | tmpB;;
		}
	}
#endif
#endif
#endif
#ifdef CONFIG_MACH_SHIRI
	SWTrigger_for_te();
#endif
}
