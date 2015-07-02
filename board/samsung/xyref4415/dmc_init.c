/*
 * Memory setup for XYREF4415 board based on EXYNOS4
 *
 * Copyright (C) 2013 Samsung Electronics
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

#include <config.h>
#include <asm/io.h>
#include <asm/arch/dmc.h>
#include <asm/arch/clock.h>
#include <asm/arch/power.h>
#include <asm/arch/cpu.h>
#include "setup.h"
#include "cmu.h"

typedef int BOOT_STAT;
#define nRESET 1
#define true 1
#define DMC_BASE 0x106F0000
#define PHY0_BASE 0x10600000
#define PHY1_BASE 0x10610000
#define Outp32(addr, data) (*(volatile u32 *)(addr) = (data))
#define Inp32(addr) ((*(volatile u32 *)(addr)))
#define SetBits(uAddr, uBaseBit, uMaskValue, uSetValue) \
    Outp32(uAddr, (Inp32(uAddr) & ~((uMaskValue)<<(uBaseBit))) | (((uMaskValue)&(uSetValue))<<(uBaseBit)))

//----------------------------------------------------------------------------
//
//- DMC Initialzation Script for LPDDR3
//
//- Copyright 2013 by Samsung Electronics. All rights reserved.
//
//----------------------------------------------------------------------------
void DMC_Delay(int msec)
{
	volatile u32 i;
	for(;msec > 0; msec--);
		for(i=0; i<1000; i++) ;
}

void InitDMC(u32 MemClk, BOOT_STAT eRstStat)
{
	u32 utemp;

	//Outp32(0x105c0504, 546867200.);
	Outp32(0x106F0000+0x0024, 0x00000003);
	Outp32(0x10600000+0x0000, 0x17023A00);
	Outp32(0x10610000+0x0000, 0x17023A00);
	Outp32(0x10600000+0x00AC, 0x809);
	Outp32(0x10610000+0x00AC, 0x809);
	Outp32(0x10600000+0x0004, 0x09210001);
	Outp32(0x10610000+0x0004, 0x09210001);
	Outp32(0x10600000+0x000C, 0x00210842);
	Outp32(0x10610000+0x000C, 0x00210842);
	Outp32(0x10600000+0x0038, 0x001F000F);
	Outp32(0x10610000+0x0038, 0x001F000F);
	Outp32(0x10600000+0x0040, 0x0F0C0308);
	Outp32(0x10610000+0x0040, 0x0F0C0308);
	Outp32(0x10600000+0x0040, 0x0F0C030a);
	Outp32(0x10610000+0x0040, 0x0F0C030a);
	Outp32(0x10600000+0x0048, 0x1E9);
	Outp32(0x10610000+0x0048, 0x1E9);
	Outp32(0x10600000+0x0040, 0x0F0C0308);
	Outp32(0x10610000+0x0040, 0x0F0C0308);
	Outp32(0x106F0000+0x0018, 0x00003000);
	Outp32(0x106F0000+0x0000, 0x1FFF1008);
		DMC_Delay(0x10000); //- GOSUB Waiting 100ms
	Outp32(0x106F0000+0x0000, 0x0FFF1000);
		DMC_Delay(0x10000); //- GOSUB Waiting 100ms
	Outp32(0x10600000+0x0010, 0x7f7f7f7f);
	Outp32(0x10610000+0x0010, 0x7f7f7f7f);
	Outp32(0x10600000+0x0018, 0x7f7f7f7f);
	Outp32(0x10610000+0x0018, 0x7f7f7f7f);
	Outp32(0x10600000+0x0028, 0x0000007f);
	Outp32(0x10610000+0x0028, 0x0000007f);
		DMC_Delay(0x10000); //- GOSUB Waiting 100ms
	Outp32(0x106F0000+0x0018, 0x00003008);
	Outp32(0x106F0000+0x0018, 0x00003000);
	Outp32(0x106F0000+0x0004, 0x00302740);
	Outp32(0x106F0000+0x0008, 0x00001323);
	Outp32(0x106F0000+0x000C, 0x00001323);
	Outp32(0x106F0000+0x010C, 0x004007c0);
	Outp32(0x106F0000+0x0014, 0xFF000000);
	Outp32(0x106F0000+0x0028, 0xFFFF00FF);

	Outp32(0x106F0000+0x0030, 0x5d); // normal = 0x5d, half = 0x2e

	Outp32(0x106F0000+0x0034, 0x2C46450C);
	Outp32(0x106F0000+0x0038, 0x3530460A);
	Outp32(0x106F0000+0x003C, 0x442F0335);
	Outp32(0x106F0000+0x0034, 0x2446648D);
	Outp32(0x106F0000+0x0038, 0x35302509);
	Outp32(0x106F0000+0x003C, 0x38270335);
	Outp32(0x106F0000+0x00F0, 0x00000007);

	if (eRstStat ==nRESET)
	{
		Outp32(0x106F0000+0x0010, 0x07000000);
			DMC_Delay(0x10000); //- GOSUB Waiting 100ms
		Outp32(0x106F0000+0x0010, 0x00071C00);
			DMC_Delay(0x10000); //- GOSUB Waiting 100ms
		Outp32(0x106F0000+0x0010, 0x00010BFC);
			DMC_Delay(0x10000); //- GOSUB Waiting 100ms
		Outp32(0x106F0000+0x0010, 0x0000050C);
		Outp32(0x106F0000+0x0010, 0x0000085C);
		Outp32(0x106F0000+0x0010, 0x00000C04);
			DMC_Delay(0x10000); //- GOSUB Waiting 100ms
		Outp32(0x106F0000+0x0010, 0x17000000);
			DMC_Delay(0x10000); //- GOSUB Waiting 100ms
		Outp32(0x106F0000+0x0010, 0x10071C00);
			DMC_Delay(0x10000); //- GOSUB Waiting 100ms
		Outp32(0x106F0000+0x0010, 0x10010BFC);
			DMC_Delay(0x10000); //- GOSUB Waiting 100ms
		Outp32(0x106F0000+0x0010, 0x1000050C);
		Outp32(0x106F0000+0x0010, 0x1000085C);
		Outp32(0x106F0000+0x0010, 0x10000C04);
			DMC_Delay(0x10000); //- GOSUB Waiting 100ms
	}

	Outp32(0x10600000+0x0030, 0x10107f10);
	Outp32(0x10610000+0x0030, 0x10107f10);
	Outp32(0x10600000+0x0030, 0x10107f30);
	Outp32(0x10610000+0x0030, 0x10107f30);
	Outp32(0x10600000+0x0030, 0x10107f70);
	Outp32(0x10610000+0x0030, 0x10107f70);

	while(1)
	{
        if(Inp32(0x10600034)&0x5==0x5)
		break;
	}

	#if 0
		utemp = (Inp32(0x10600000+0x0034)>>10)&0x7f; //forcing value
		Outp32(0x10600000+0x0030, Inp32(0x10600000+0x0030)&(~(0x7f<<8))|(utemp<<8));
		Outp32(0x10600000+0x0030, Inp32(0x10600000+0x0030)&(~(0x1<<5))); //forcing value
	#endif

	Outp32(0x10610000+0x0030, 0x10107f10);
	Outp32(0x10610000+0x0030, 0x10107f30);
	Outp32(0x10610000+0x0030, 0x10107f70);

	while(1)
	{
        if(Inp32(0x10610034)&0x5==0x5)
		break;
	}

	#if 0
		utemp = (Inp32(0x10610000+0x0034)>>10)&0x7f; //forcing value
		Outp32(0x10610000+0x0030, Inp32(0x10610000+0x0030)&(~(0x7f<<8))|(utemp<<8));
		Outp32(0x10610000+0x0030, Inp32(0x10610000+0x0030)&(~(0x1<<5))); //forcing value
	#endif

	//---------------- clock change
	//Outp32(0x105C0504, 142641152.);
	//Outp32(0x105C0800, 3.);
	//Outp32(0x105C0300, 0x1510);
	//Outp32(0x105C0218, 2171208705.);
	//	DMC_Delay(0x10000); //- GOSUB Waiting 0.3s
	//Outp32(0x105C0300, 0x1510);
	CMU_InitForMif(MemClk);


	Outp32(0x10600000+0x0010, 0x08080808);
	Outp32(0x10610000+0x0010, 0x08080808);
	Outp32(0x10600000+0x0018, 0x08080808);
	Outp32(0x10610000+0x0018, 0x08080808);
	Outp32(0x10600000+0x0028, 0x00000008);
	Outp32(0x10610000+0x0028, 0x00000008);

	while(1)
	{
        if(Inp32(0x10600000+0x0034)&0x1==0x1)
		break;
	}
	while(1)
	{
        if(Inp32(0x10610000+0x0034)&0x1==0x1)
		break;
	}

	Outp32(0x106F0000+0x0000, 0x1FFF1008);
		DMC_Delay(0x10000); //- GOSUB Waiting 100ms
		DMC_Delay(0x10000); //- GOSUB Waiting 0.1s
	Outp32(0x106F0000+0x0018, 0x5008);
	Outp32(0x106F0000+0x0018, 0x5000);
	Outp32(0x10600000+0x0000, 0x17023a00);
	Outp32(0x10610000+0x0000, 0x17023a00);
	Outp32(0x106F0000+0x0010, 0x01000000);
		DMC_Delay(0x10000); //- GOSUB Waiting 100ms
	Outp32(0x106F0000+0x0010, 0x01100000);
		DMC_Delay(0x10000); //- GOSUB Waiting 100ms
	Outp32(0x106F0000+0x0010, 0x11000000);
		DMC_Delay(0x10000); //- GOSUB Waiting 100ms
	Outp32(0x106F0000+0x0010, 0x11100000);
		DMC_Delay(0x10000); //- GOSUB Waiting 100ms

	#if 0
	Outp32(0x106F0000+0x0004, 0x00302763);
	Outp32(0x106F0000+0x0000, 0x0FFF1028);
	#endif
	Outp32(0x106F0000+0x001C, 0x17); // MEMIF_CG_EN off
	Outp32(0x106F0000+0x0400, 0x00000001);	// ALL_INIT_INDI[0]=all_init_done=0x1

	// LPI handshaking w/a
	SetBits(0x105C0700, 0, 0x1, 0);
	// Checking Self-Refresh Status
	#if 1				//- 1GB size
		while(((Inp32(0x106F0048)>>8)&0x1)!=0x1);					//- CH0 ChipStatus, chip_sref_state[8] (CS0)
		while(((Inp32(0x106F004C)>>8)&0x1)!=0x1);					//- CH1 ChipStatus, chip_sref_state[8] (CS0)
	#else				//- 2GB size
		while(((Inp32(0x106F0048)>>8)&0x3)!=0x3);					//- CH0 ChipStatus, chip_sref_state[9:8] (CS1,CS0)
		while(((Inp32(0x106F004C)>>8)&0x3)!=0x3);					//- CH1 ChipStatus, chip_sref_state[9:8] (CS1,CS0)
	#endif

	//DMC_Delay(10000);
	SetBits(0x105C0700, 0, 0x1, 1);

	// MEMIF_CG_EN off
	Outp32(0x106F0000+0x001C, 0x1F);

	//check clockout setting
	Outp32(0x10020A00, 0x00000000);
	Outp32(0x105C0A00, 0x10004);

	#if 1
		Outp32(0x106F0000+0x0004, 0x00302762);							//- Setting Power Down Mode, dynamic self refresh, dynamic power down : Enable
		Outp32(0x106F0000+0x0018, 0x5002);									//- SDLL dynamic power gating : Enable
		Outp32(0x106F0000+0x0000, 0x0FFF1028);							//- Auto Refresh : Enable
	#endif

}

void InitDMC_temp(u32 MemClk, BOOT_STAT eRstStat)
{
	u32 IvSize=0x7;		//Interleaved( 128-byte:0x7),Linear(512-Mbyte:0x1D)
	u32 DrexCGEn=1;
	u32 PhyCgEn=1;
	u32 uBits;
	u32 uCLK_Pause =true;
	u32 uDLL_Lock0,uDLL_Lock1,uTemp;
	u32 uIsWakeup;
	u32 channel,chip;


//;-------------------------------------------------------------------------------
//; 0-0 Clock change to low (about 50Mhz)
//;-------------------------------------------------------------------------------
//; DMC(1/2:27), DPHY(1/2:23), DMC_PRE(1/8:19), COREP(1/2:15), CORED(1/2:11), MPLL_PRE(1/1:8)
//&uBits=(4.<<27.)|(1.<<23.)|(3.<<19.)|(1.<<15.)|(1.<<11.)|(0.<<8.);For DMC 40Mhz@800Mhz(BPLL), 53.3Mhz@1066Mhz(BPLL)
//d.s SD:0x105c0504 %LE %LONG &uBits //"rCLK_DIV_CORE_1"

	// DMC(1/2:27), DPHY(1/2:23), DMC_PRE(1/8:19), COREP(1/2:15), CORED(1/2:11), MPLL_PRE(1/1:8)
	//uBits=(1<<27)|(1<<23)|(0<<19)|(1<<15)|(1<<11)|(0<<8)//For DMC 400Mhz
	//uBits=(1<<27)|(1<<23)|(2<<19)|(1<<15)|(1<<11)|(0<<8)//For DMC 266Mhz
	uBits=(4<<27)|(1<<23)|(3<<19)|(1<<15)|(1<<11)|(0<<8);//For DMC 40Mhz@BPLL800Mhz, 53.3Mhz@BPLL 1066Mhz
	Outp32(0x105c0504, uBits); //"rCLK_DIV_CORE_1"
	//Outp32(0x105C0300, 0x1510); //Phy BPLL 400Mhz Sel


//;-------------------------------------------------------------------------------
//; 0. Set PLControl to 2'b11 for reducing pipeline cycle
//;-------------------------------------------------------------------------------
//GOSUB write &DMC_BASE+0x0024 0x00000003
	Outp32(DMC_BASE+0x0024 ,0x00000003);


//;-------------------------------------------------------------------------------
//; 1. To provide stable power for controller and memory device,
//;    the controller must assert and hold CKE to a logic low level.
//;-------------------------------------------------------------------------------

//;-------------------------------------------------------------------------------
//; 2. Set the right value to PHY control register0 for LPDDR2/3. If read
//; leveling is needed, check LPDDR2/3 IO calibration MRR data and match it
//; to PHY control register1's ctrl_rlvl_rdata_adj field.
//;-------------------------------------------------------------------------------
//;PHY_CON0 reset value 0x17020A40
//;    w_data[13]		= 'h1;		// [13] byte_rdlvl_en
//;    w_data[12:11]	= 'h3;		// [12:11] 00:DDR2/LPDDR2, 01:DDR3, 10:LPDDR2, 11:LPDDR3
//;    w_data[5]		= 'h1;		// [5] drive ctrl_read_p* 1: LPDDR2/LPDDR3, 0:normal
//GOSUB write &PHY0_BASE+0x0000 0x17023A60
//GOSUB write &PHY1_BASE+0x0000 0x17023A60

	Outp32(PHY0_BASE+0x0000 ,0x17023A60);
	Outp32(PHY1_BASE+0x0000 ,0x17023A60);

//;PHY_CON1 reset value 0x09210100
//;     w_data[15:0]	= 'h1;		// [15:0] rdlvl_rddata_adj 16'h0100:DDR3(@byte_rdlvl_en=1), 16'h0001:LPDDR3(@byte_rdlvl_en=1)
//GOSUB write &PHY0_BASE+0x0004 0x09210101
//GOSUB write &PHY1_BASE+0x0004 0x09210101
	Outp32(PHY0_BASE+0x0004 ,0x09210101);
	Outp32(PHY1_BASE+0x0004 ,0x09210101);

//;PHY_CON3 reset value 0x00210842
//GOSUB write &PHY0_BASE+0x000C 0x00210842
//GOSUB write &PHY1_BASE+0x000C 0x00210842
	Outp32(PHY0_BASE+0x000C ,0x00210842);
	Outp32(PHY1_BASE+0x000C ,0x00210842);

//;PHY_CON14 reset value 0x001F0000
//;w_data[3:0]		= 'hF;		// [3:0] F:LPDDR2/LPDDR3, 0:Gate Leveing in DDR3(not normal)
//GOSUB write &PHY0_BASE+0x0038 0x001F000F
//GOSUB write &PHY1_BASE+0x0038 0x001F000F
	Outp32(PHY0_BASE+0x0038 ,0x001F000F);
	Outp32(PHY1_BASE+0x0038 ,0x001F000F);


//;PHY_CON16 reset value 0x08000304
//;w_data[19]		= 'h1;		// [19] zq_mode_noterm. 1:termination disable, 0:termination enable/
//GOSUB write &PHY0_BASE+0x0040 0x08080304
//GOSUB write &PHY1_BASE+0x0040 0x08080304
	Outp32(PHY0_BASE+0x0040 ,0x08080304);
	Outp32(PHY1_BASE+0x0040 ,0x08080304);


//; PHYCONTROL0 reset value 0x00000000
//; w_data[28:24]	= 'h1F;		// [28:24] ctrl_pd.
//; w_data[15:12]	= 'h9;		// [15:12] Recovery Time From PHY Clock Gate. minimum value is 0x3
//; w_data[0]		= 'h1;		// [0] Memory Termination between chips. 0:disable, 1:Enable
//GOSUB write &DMC_BASE+0x0018 0x01F09001
//; fpga - GOSUB write &DMC_BASE+0x0018 0x1f749a05
	Outp32(DMC_BASE+0x0018 ,0x01F09001);


//;-------------------------------------------------------------------------------
//; 3. Assert dfi_init_start field to high but the aref_en should be off
//;-------------------------------------------------------------------------------
//;-------------------------------------------------------------------------------
//; 4. Wait dfi_init_complete
//;-------------------------------------------------------------------------------
//; CONCONTROL reset value 0x0FFF1100
//;  w_data[28]    = 1'b1;      // dfi_init_start
//;  w_data[27:16] = 12'hFFF;   // timeout_cnt
//;  w_data[15]    = 1'b0;      // Reserved
//;  w_data[14:12] = 4'h1;      // rd_fetch. 0x2:LPDDR3 800MHz/ 0x1:Other cases
//;  w_data[11:9]  = 1'b0;      // Reserved
//;  w_data[8]     = 1'b0;      // chip_empty, read-only
//;  w_data[7:6]   = 2'b0;      // Reserved
//;  w_data[5]     = 1'b0;      // Auto-Refresh cnt En.  ** Auto refresh cont. On
//;  w_data[4]     = 1'b0;      // Reserved
//;  w_data[3]     = 1'b1;      // IO Powerdown Control in Low Power Mode(through LPI)
//;			   // 0x0 = Use Programmed ctrl_pd and pulldown control
//;			   // 0x1 = Automatic control for ctrl_pd and pulldown control
//;  w_data[2:1]   = 2'b0;      // clock ratio between aclk and cclk (0 - 1:1,  1 - 1:2)
//;  w_data[0]     = 1'b0;      // Reserved

//GOSUB write &DMC_BASE+0x0000 0x1FFF1008 ; DFI_INIT_START '1' & aref'0'   step 4
//Wait 100ms                              ; check DFI_INIT_COMPLETE 	     step 5
	Outp32(DMC_BASE+0x0000 ,0x1FFF1008);
	DMC_Delay(100); //

//;-------------------------------------------------------------------------------
//; 5. Set ConControl. Deassert dfi_init_start field to low
//;-------------------------------------------------------------------------------
//GOSUB write &DMC_BASE+0x0000 0x0FFF1000 ; DFI_INIT_START '0' & aref'0'   step 6
//Wait 100ms
	Outp32(DMC_BASE+0x0000 ,0x0FFF1000);
	DMC_Delay(100); //


//;-------------------------------------------------------------------------------
//; 6. Set PhyControl0.fp_resync to '1' to DLL information
//; 7. Set PhyControl0.fp_resync to '0'
//;-------------------------------------------------------------------------------
//GOSUB write &DMC_BASE+0x0018 0x00000008 ; [3] 1'b1 Force DLL resync      step 7
//GOSUB write &DMC_BASE+0x0018 0x00000000 ; [3] 1'b0
	Outp32(DMC_BASE+0x0018 ,0x00000008);
	Outp32(DMC_BASE+0x0018 ,0x00000000);

//;-------------------------------------------------------------------------------
//; 8. Set MemControl. All power down modes should be off
//;-------------------------------------------------------------------------------
//;MEMCONTROL reset value 0x00202601
//GOSUB write &DMC_BASE+0x0004 0x00312700  ; Memory Control Register
//					 ; [22:20] 3'b011 Memory Burst length 8
//					 ;	[19:16] 4'b0001	 2chips
//					 ; [15:12] 4'b0010 Width of Memory Data Bus 32bit
//					 ; [11:8]4'b0111 LPDDR3
//					 ; [7:6]	2'b00 Additional Latency for PALL in cclk cycle
//					 ; [5] 1'b0 dynamic self refresh disable
//					 ; [4] 1'b0 timeout precharge
//					 ; [3:2] 1'b0 type of dynamic power down , active/precharge power down
//					 ; [1] 1'b0 dynamic power down disable
//					 ; [0] 1'b0 always running
 	Outp32(DMC_BASE+0x0004 ,0x00312700);

//;-------------------------------------------------------------------------------
//; 9. Set MemConfig0/1 : ADDR/ROW/BANK bits																																			 step 10 MembaseConfig0/1 setting (0x010c)
//;-------------------------------------------------------------------------------
//;MEMCONFIG0,1 reset value 0x00001312
//GOSUB write &DMC_BASE+0x0008 0x00001323   ; MemConfig0                     step 11
//					  ; [15:12]	4'b0001 address Mapping Method (AXI to Memory) interleaved ({row, bank, column, width}),
//					  ; [11:8]4'b0011  Number of Column Address Bits 10bit
//					  ; [7:4]	4'b0010  Number of Row Address Bits 14bit
//					  ; [3:0] 4'b0011 Number of Banks 8 banks
//GOSUB write &DMC_BASE+0x000C 0x00001323
 	Outp32(DMC_BASE+0x0008 ,0x00001323);
 	Outp32(DMC_BASE+0x000C ,0x00001323);


//;-------------------------------------------------------------------------------
//; 10. Set MemBaseConfig0/1
//;-------------------------------------------------------------------------------
//GOSUB write &DMC_BASE+0x010C 0x004007C0   ; MemBaseConfig0                  step 10
//					  ; [26:16] AXI Base Address for Chip0 0x40000000
//					  ; [10:0] 1GB, then chip_mask is 0x7C0.

//GOSUB write &DMC_BASE+0x0110 0x008007C0   ; MemBaseConfig1
//					  ; [26:16] AXI Base Address for Chip1 0x80000000
//					  ; [10:0] 1GB, then chip_mask is 0x7C0.
	Outp32(DMC_BASE+0x010C ,0x004007C0);
	Outp32(DMC_BASE+0x0110 ,0x008007C0);

//;-------------------------------------------------------------------------------
//; 11. Set PrechConfig and PwrdnConfig
//;-------------------------------------------------------------------------------
//GOSUB write &DMC_BASE+0x0014 0xFF000000   ; PrechConfig                     step 12
//					  ; [31:24] 0xff Timeout Precharge Cycles
//					  ; [15:8] Memory Chip1 Precharge Bank Selective Policy
//					  ; [7:0]  Memory Chip0 Precharge Bank Selective Policy
//GOSUB write &DMC_BASE+0x0028 0xFFFF00FF   ; PwrdnConfig
//					  ; [31:16] Number of Cycles for dynamic self refresh entry
//					  ; [7:0]   Number of Cycles for dynamic power down entry
	Outp32(DMC_BASE+0x0014 ,0xFF000000);
	Outp32(DMC_BASE+0x0028 ,0xFFFF00FF);

//;-------------------------------------------------------------------------------
//;12. Set the TimingAref, TimingRow, TimingData and TimingPower registers
//;    according to memory AC parameters. At this moment, TimingData.wl and TimingData.rl
//;    registers must be programmed according to initial clock frequency
//;-------------------------------------------------------------------------------

//GOSUB write &DMC_BASE+0x0030 0x0000005d   ; TimingAref                      step 13
//					  ; [15:0] Average Periodic Refresh Interval
//					  ; t_refi * T(rclk) should be less than or equal to the minimum value of memory tREFI (all bank),
//					  ; for example, for the all bank refresh period of 7.8us, and an rclk frequency of 12MHz,
//					  ; the following value should be programmed into it: 7.8 us * 12 MHz = 93
	Outp32(DMC_BASE+0x0030 ,0x0000005d);

//if &MCLK==667
//(
//; TIMINGROW reset value 0x1F233286
//;      w_data[31:24] = 8'h57;  //tRFCab                  130 /1.5 = 86.7  t_rfc = timing_row[31:24];
//;      w_data[23:20] = 4'h7;   //tRRD   max(2Tck,10ns) = 10  /1.5 = 6.7   t_rrd = timing_row[23:20];
//;      w_data[19:16] = 4'hE;   //tRP                     21  /1.5 = 14    t_rp = timing_row[19:16];
//;      w_data[15:12] = 4'hC;   //tRCD                    18  /1.5 = 12    t_rcd = timing_row[15:12];
//;      w_data[11: 6] = 6'h28;  //tRC                     60  /1.5 = 40    t_rc = timing_row[11:6];
//;      w_data[ 5: 0] = 6'h1C;  //tRAS                    42  /1.5 = 28    t_ras = timing_row[5:0];
//	GOSUB write &DMC_BASE+0x0034 0x577ECA1C

if (MemClk==667)
{
	Outp32(DMC_BASE+0x0034 ,0x577ECA1C);
	Outp32(DMC_BASE+0x0038 ,0x5A53360C);
	Outp32(DMC_BASE+0x003C ,0x885E05AA);

}
else if(MemClk==667)
{
	Outp32(DMC_BASE+0x0034 ,0x2336544C);
	Outp32(DMC_BASE+0x0038 ,0x24202408);
	Outp32(DMC_BASE+0x003C ,0x38260235);

}
else
{
	Outp32(DMC_BASE+0x0034 ,0x03598691);
	Outp32(DMC_BASE+0x0038 ,0x3833060c);
	Outp32(DMC_BASE+0x003C ,0x50c80365);
}


//; TIMINGDATA reset value 0x1230360C
//;      w_data[31:28] = 4'h5;   //tWTR  max(4Tck,7.5ns) = 7.5 /1.5 = 5    t_wtr = timing_data[31:28];
//;      w_data[27:24] = 4'hA;   //tWR   max(4Tck,15ns)  = 15  /1.5 = 10   t_wr = timing_data[27:24];
//;      w_data[23:20] = 4'h5;   //tRTP  max(4Tck,7.5ns) = 7.5 /1.5 = 5    t_rtp = timing_data[23:20];
//;      w_data[19:16] = 4'h3;   //CL    Reset Value.                      cl = timing_data[19:16];
//;      w_data[11: 8] = 4'h6;   //WLmin 6 tCK                             wl = timing_data[11:8];
//;      w_data[ 3: 0] = 4'hC;   //RLmin 12 tCK                            rl = timing_data[3:0];
//	GOSUB write &DMC_BASE+0x0038 0x5A53360C

//; TIMINGPOWER reset value 0x381B0422
//;      w_data[31:26] = 6'h22;  //tFAW   max(8Tck,50ns)   = 50  /1.5 = 33.4  t_faw = timing_power[31:26];
//;      w_data[25:16] = 10'h5E; //tXSRD  max(2Tck,tRFC+10)= 140 /1.5 = 93.3  t_xsr = timing_power[25:16];
//;			      // = tXSDLL @DDR3
//;      w_data[15: 8] = 8'h5;   //tXP    max(2Tck,7.5ns)  = 7.5 /1.5 = 5     t_xp = timing_power[15:8];
//;			      // =tXPDLL @DDR3
//;      w_data[ 7: 4] = 4'hA;   //tCKESR max(3Tck,15ns)   = 15  /1.5 = 10    t_cke = timing_power[7:4];
//;      w_data[ 3: 0] = 4'hA;   //       max(6Tck,15ns)   = 15  /1.5 = 10    t_mrd = timing_power[3:0];
//			      // = tMOD @DDR3
//	GOSUB write &DMC_BASE+0x003C 0x885E05AA
//)
//else if &MCLK==533
//(
//	GOSUB write &DMC_BASE+0x0034 0x2336544C
//	GOSUB write &DMC_BASE+0x0038 0x24202408
//	GOSUB write &DMC_BASE+0x003C 0x38260235
//)
//else	;&MCLK==400
//(
//	GOSUB write &DMC_BASE+0x0034 0x03598691 ;0x1A354349
//	GOSUB write &DMC_BASE+0x0038 0x3833060c ;0x23202306
//	GOSUB write &DMC_BASE+0x003C 0x50c80365
//)

//GOSUB write &DMC_BASE+0x00F0 0x000000007 ; IVCONTROL                            step ???
	Outp32(DMC_BASE+0x00F0 ,0x000000007);

//;-------------------------------------------------------------------------------  																				 ; [7:0] Memory Channel Interleaving Size 0x07 = 128-byte,
//; 13. If QoS scheme is required...
//;-------------------------------------------------------------------------------
//GOSUB write &DMC_BASE+0x0060 0x00ff_4b20
//GOSUB write &DMC_BASE+0x0068 0x00ff_c0c0
//GOSUB write &DMC_BASE+0x0070 0x00ff_5d10
//GOSUB write &DMC_BASE+0x0078 0x00ff_b130
//GOSUB write &DMC_BASE+0x0080 0x00ff_61f0
//GOSUB write &DMC_BASE+0x0088 0x00ff_0fc0
//GOSUB write &DMC_BASE+0x0090 0x00ff_f680
//GOSUB write &DMC_BASE+0x0098 0x00ff_2160
//GOSUB write &DMC_BASE+0x00A0 0x00ff_6dd0
//GOSUB write &DMC_BASE+0x00A8 0x00ff_93c0
//GOSUB write &DMC_BASE+0x00B0 0x00ff_2330
//GOSUB write &DMC_BASE+0x00B8 0x00ff_fd60
//GOSUB write &DMC_BASE+0x00C0 0x00ff_48f0
//GOSUB write &DMC_BASE+0x00C8 0x00ff_d750
//GOSUB write &DMC_BASE+0x00D0 0x00ff_4580
//GOSUB write &DMC_BASE+0x00D8 0x000f_6920
	Outp32(DMC_BASE+0x0060 ,0x00ff4b20);
	Outp32(DMC_BASE+0x0068 ,0x00ffc0c0);
	Outp32(DMC_BASE+0x0070 ,0x00ff5d10);
	Outp32(DMC_BASE+0x0078 ,0x00ffb130);
	Outp32(DMC_BASE+0x0080 ,0x00ff61f0);
	Outp32(DMC_BASE+0x0088 ,0x00ff0fc0);
	Outp32(DMC_BASE+0x0090 ,0x00fff680);
	Outp32(DMC_BASE+0x0098 ,0x00ff4b20);
	Outp32(DMC_BASE+0x00A0 ,0x00ff4b20);
	Outp32(DMC_BASE+0x00A8 ,0x00ff4b20);
	Outp32(DMC_BASE+0x00B0 ,0x00ff4b20);
	Outp32(DMC_BASE+0x00B8 ,0x00ff4b20);
	Outp32(DMC_BASE+0x00C0 ,0x00ff4b20);
	Outp32(DMC_BASE+0x00C8 ,0x00ff4b20);
	Outp32(DMC_BASE+0x00D0 ,0x00ff4b20);
	Outp32(DMC_BASE+0x00D8 ,0x00ff4b20);


#if 1
//;-------------------------------------------------------------------------------
//; 14. Set the PHY for LPDDR2/3 intialization
//;     If LPDDR2/3 initialization frequency is 20ns, then follow 15 ~21
//;-------------------------------------------------------------------------------

//;-------------------------------------------------------------------------------
//; 15. Set ctrl_offsetr0~3 and ctrl_offsetw0~3 to 0x7F
//;-------------------------------------------------------------------------------
//;GOSUB write &PHY0_BASE+0x0010 0x7f7f7f7f ; PHY_CON4                             step 16
// 					 ; ctrl_offset_r=0x7f
//;GOSUB write &PHY1_BASE+0x0010 0x7f7f7f7f ; ctrl_offset_r=0x7f
	Outp32(PHY0_BASE+0x0010 ,0x7f7f7f7f);
	Outp32(PHY1_BASE+0x0010 ,0x7f7f7f7f);
//;-------------------------------------------------------------------------------
//; 16. Set ctrl_offsetd0~3 to 0x7F
//;-------------------------------------------------------------------------------
//;GOSUB write &PHY0_BASE+0x0018 0x7f7f7f7f ; PHY_CON6
//					 ; ctrl_offset_w=0x7f
//;GOSUB write &PHY1_BASE+0x0018 0x7f7f7f7f ; ctrl_offset_w=0x7f
	Outp32(PHY0_BASE+0x0018 ,0x7f7f7f7f);
	Outp32(PHY1_BASE+0x0018 ,0x7f7f7f7f);
//;-------------------------------------------------------------------------------
//; 17. Set ctrl_force to 0x7F
//;-------------------------------------------------------------------------------
//;GOSUB write &PHY0_BASE+0x0028 0x0000007f ; PHY_CON10                            step 17
//    				   ; ctrl_offset_d=0x7f
//;GOSUB write &PHY1_BASE+0x0028 0x0000007f ; ctrl_offset_d=0x7f
	Outp32(PHY0_BASE+0x0028 ,0x0000007f);
	Outp32(PHY1_BASE+0x0028 ,0x0000007f);
//;-------------------------------------------------------------------------------
//; 18. Set ctrl_dll_on to low
//;-------------------------------------------------------------------------------
//;GOSUB write &PHY0_BASE+0x0030 0x10107f70 ; PHY_CON12                            step 18/step19
//					 ; ctrl_force[14:8]=0x7f, ctrl_dll_on[5]=0
//;GOSUB write &PHY1_BASE+0x0030 0x10107f70 ; ctrl_force[14:8]=0x7f, ctrl_dll_on[5]=0
//;GOSUB write &PHY0_BASE+0x0030 0x10107f50 ; PHY_CON12                            step 18/step19
//					 ; ctrl_force[14:8]=0x7f, ctrl_dll_on[5]=0
//;GOSUB write &PHY1_BASE+0x0030 0x10107f50 ; ctrl_force[14:8]=0x7f, ctrl_dll_on[5]=0
	Outp32(PHY0_BASE+0x0030 ,0x10107f70);
	Outp32(PHY1_BASE+0x0030 ,0x10107f70);
	Outp32(PHY0_BASE+0x0030 ,0x10107f50);
	Outp32(PHY1_BASE+0x0030 ,0x10107f50);

//;-------------------------------------------------------------------------------
//; 19. Wait for 10 PCLK
//;-------------------------------------------------------------------------------
//;Wait 100ms                                                                      ; step 20
	DMC_Delay(100);
//;-------------------------------------------------------------------------------
//;20. Set the Phy Control.fp_resync bit '1'
//;-------------------------------------------------------------------------------
//;GOSUB write &DMC_BASE+0x0018 0x00000008 ; PHYCONTROL0 resync                      step 21
	Outp32(DMC_BASE+0x0018 ,0x00000008);

//;-------------------------------------------------------------------------------
//;21. Set the Phy Control.fp_resync bit '0'
//;-------------------------------------------------------------------------------
//;GOSUB write &DMC_BASE+0x0018 0x00000000 ; PHYCONTROL0 resync                      step 22
	Outp32(DMC_BASE+0x0018 ,0x00000000);
#endif

if ( eRstStat == nRESET )
{
	for (channel=0; channel<2; channel++)
	{
		for (chip=0; chip<2; chip++)
		{
			 //;-------------------------------------------------------------------------------
			//; 22. Confirm that CKE has been as a logic low level at least 100ns after power on.
			//;-------------------------------------------------------------------------------
			//;-------------------------------------------------------------------------------
			//; 23.Issue a NOP command using the DirectCmd register to assert and to hold CKE to a logic high level.
			//;-------------------------------------------------------------------------------
			//&mr1_400=0x0000050c
			//&mr1_800=0x0000070c

			//; ---------------------
			//; Direct Command P0 CH0
			//; ---------------------
			//GOSUB write &DMC_BASE+0x0010 0x07000000             ;  Direct Command NOP        step 24
				Outp32(DMC_BASE+0x0010 ,(channel<<28)|(chip<<20)|0x07000000);
			//;-------------------------------------------------------------------------------
			//; 24. Wait for minimum 200us.
			//;-------------------------------------------------------------------------------
			//Wait 100ms                                          ;                            step 25
				DMC_Delay(100);
			//;-------------------------------------------------------------------------------
			//;25. Issue a MRS command using the DirectCmd register to reset memory devices and program the
			//;     operating parameters.
			//;-------------------------------------------------------------------------------
			//GOSUB write &DMC_BASE+0x0010 0x00071C00             ;  Direct Command MRS        step 26
			//                                                    ;  [18:16] ?? Related Bank Address when issuing a direct command
			//                                                    ;  [15:0] ?? Related Address value when issuing a direct command
				Outp32(DMC_BASE+0x0010 ,(channel<<28)|(chip<<20)|0x00071C00);
			//;-------------------------------------------------------------------------------
			//; 26. Wait for minimum 10us.
			//; 27.
			//;-------------------------------------------------------------------------------


			//Wait 100ms                                          ;                            step 27
			//GOSUB write &DMC_BASE+0x0010 0x00010BFC             ;  Direct Command MRS        step ??
			//																										;  [18:16] ?? Related Bank Address when issuing a direct command
			//													;  [15:0] ?? Related Address value when issuing a direct command
				DMC_Delay(100);
				Outp32(DMC_BASE+0x0010 ,(channel<<28)|(chip<<20)|0x00010BFC);

			//Wait 100ms
				DMC_Delay(100);
				if (MemClk==400)
				{
					//if &MCLK==400
				//(
				//	;- nWR=6, Wrap, Sequential, BL4
				//	GOSUB write &DMC_BASE+0x0010 0x00000000+&mr1_400
					Outp32(DMC_BASE+0x0010 ,(channel<<28)|(chip<<20)|0x0000050c);
				//	Wait 100ms
					DMC_Delay(100);
				//	;- RL=6, WL=3
				//	GOSUB write &DMC_BASE+0x0010 0x00000810
					Outp32(DMC_BASE+0x0010 ,(channel<<28)|(chip<<20)|0x00000810);
				//	Wait 100ms
					DMC_Delay(100);
				//)

				}
				else if(MemClk==667)
				{
				//else if &MCLK==667
				//{
				//;       // [27:24] cmd_type
				//;       // [20]    cmd_chip
				//;       // [18:16] cmd_bank
				//;       // [15:0]  cmd_addr
				//;       // MA[7:0] = {cmd_addr[1:0], cmd_bank[2:0], cmd_addr[12:10]},
				//;       // OP[7:0] = cmd_addr[9:2]
				//;       // MR1 : nWR, WC, BT, BL
				//;       //   MA=0x01 = {cmd_addr[1:0], cmd_bank[2:0], cmd_addr[12:10]} = 00_000_001,
				//;       //   OP=100_0_0_010 (nWR=12, Sequential, BL8)
				//;       //   0000_0000_0000_0_000_000_001_100_0_0_010_00
				//;       //   0000_0000_0000_0000_0000_0110_0000_1000
				//	GOSUB write &DMC_BASE+0x0010 0x0000050c
					Outp32(DMC_BASE+0x0010 ,(channel<<28)|(chip<<20)|0x0000050c);

				//;	// [27:24] cmd_type
				//;       // [20]    cmd_chip
				//;       // [18:16] cmd_bank
				//;       // [15:0]  cmd_addr
				//;       // MA[7:0] = {cmd_addr[1:0], cmd_bank[2:0], cmd_addr[12:10]},
				//;       // OP[7:0] = cmd_addr[9:2]
				//;       // MR2 : RL&WL
				//;       //   MA=0x02 = {cmd_addr[1:0], cmd_bank[2:0], cmd_addr[12:10]} = 00_000_010,
				//;       //   OP=0x04 (RL=12, WL=6)
				//;       //   0000_0000_0000_0_000_000_010_00000100_00
				//;       //   0000_0000_0000_0000_0000_1000_0001_0000
				//	GOSUB write &DMC_BASE+0x0010 0x00000828
					Outp32(DMC_BASE+0x0010 ,(channel<<28)|(chip<<20)|0x00000828);
				}
				else
				{
					Outp32(DMC_BASE+0x0010 ,(channel<<28)|(chip<<20)|0x0000070c);
					Outp32(DMC_BASE+0x0010 ,(channel<<28)|(chip<<20)|0x00000818);


				}
			Outp32(DMC_BASE+0x0010 ,(channel<<28)|(chip<<20)|0x00000C08);
			//else
			//(
			//	;- nWR=8, Wrap, Sequential, BL4
			//	GOSUB write &DMC_BASE+0x0010 0x00000000+&mr1_800
			//	Wait 100ms
			//	;- RL=?, WL=?
			//	GOSUB write &DMC_BASE+0x0010 0x00000818
			//	Wait 100ms
			//	GOSUB write &DMC_BASE+0x0010 0x00000C08	;DirectCmd	chip0 MRS, MA03 40-ohm
			//	Wait 100ms
			//)
		}

	}
}


//;-------------------------------------------------------------------------------
//; 29. Set ctrl_offsetr0~3 and ctrl_offsetw0~3 to 0x0
//;-------------------------------------------------------------------------------
//GOSUB write &PHY0_BASE+0x0010 0x00000000
//GOSUB write &PHY1_BASE+0x0010 0x00000000
	Outp32(PHY0_BASE+0x0010 ,0x00000000);
	Outp32(PHY1_BASE+0x0010 ,0x00000000);

//;-------------------------------------------------------------------------------
//; 30. Set ctrl_offsetd to 0x0
//;-------------------------------------------------------------------------------
//GOSUB write &PHY0_BASE+0x0028 0x00000000
//GOSUB write &PHY1_BASE+0x0028 0x00000000
	Outp32(PHY0_BASE+0x0028 ,0x00000000);
	Outp32(PHY1_BASE+0x0028 ,0x00000000);

//;-------------------------------------------------------------------------------
//; 31. Set ctrl_dll_on enable
//;-------------------------------------------------------------------------------
//GOSUB write &PHY0_BASE+0x0030 0x10107f70
//GOSUB write &PHY1_BASE+0x0030 0x10107f70
	Outp32(PHY0_BASE+0x0030 ,0x10107f70);
	Outp32(PHY1_BASE+0x0030 ,0x10107f70);

//;-------------------------------------------------------------------------------
//; 32. Set ctrl_start to 0
//;-------------------------------------------------------------------------------
//GOSUB write &PHY0_BASE+0x0030 0x10107f30
//GOSUB write &PHY1_BASE+0x0030 0x10107f30
	Outp32(PHY0_BASE+0x0030 ,0x10107f30);
	Outp32(PHY1_BASE+0x0030 ,0x10107f30);
//;-------------------------------------------------------------------------------
//; 33. Set ctrl_start to 1
//;-------------------------------------------------------------------------------
//GOSUB write &PHY0_BASE+0x0030 0x10107f70
//GOSUB write &PHY1_BASE+0x0030 0x10107f70
	Outp32(PHY0_BASE+0x0030 ,0x10107f70);
	Outp32(PHY1_BASE+0x0030 ,0x10107f70);

//;-------------------------------------------------------------------------------
//; 34. Wait for 10 PCLK
//;-------------------------------------------------------------------------------
//; 35. Wait DFI_init complete
//;-------------------------------------------------------------------------------
//print "===  Waiting for 2nd DFI INIT COMPLETE (LPDDR3PHY)    ==="
//wait 0.1s
	DMC_Delay(100);
//;-------------------------------------------------------------------------------
//; 36. Set the Phy Control.fp_resync bit '1'
//;-------------------------------------------------------------------------------
//GOSUB write &DMC_BASE+0x0018 0x1f749a0c
	Outp32(DMC_BASE+0x0018 ,0x1f749a0c);
//;-------------------------------------------------------------------------------
//; 37. Set the Phy Control.fp_resync bit '0'
//;-------------------------------------------------------------------------------
//GOSUB write &DMC_BASE+0x0018 0x1f749a04
	Outp32(DMC_BASE+0x0018 ,0x1f749a04);

//;-------------------------------------------------------------------------------
//; Skip 38 ~ 42
//;-------------------------------------------------------------------------------

//;------------------------------------------------------------------------------
//; 43. Disable ctrl_atgate
//;------------------------------------------------------------------------------
//GOSUB write &PHY0_BASE+0x0000 0x00023a00
//GOSUB write &PHY1_BASE+0x0000 0x00023a00
	Outp32(PHY0_BASE+0x0000 ,0x00023a00);
	Outp32(PHY1_BASE+0x0000 ,0x00023a00);

//;------------------------------------------------------------------------------
//; 43.-1 PALL command to memory using DirectCmd
//;------------------------------------------------------------------------------
//GOSUB write &DMC_BASE+0x0010 0x01000000
//Wait 100ms
	Outp32(DMC_BASE+0x0010 ,0x01000000);
	DMC_Delay(100);

//GOSUB write &DMC_BASE+0x0010 0x01100000
//Wait 100ms
	Outp32(DMC_BASE+0x0010 ,0x01100000);
	DMC_Delay(100);
//GOSUB write &DMC_BASE+0x0010 0x11000000
//Wait 100ms
	Outp32(DMC_BASE+0x0010 ,0x11000000);
	DMC_Delay(100);
//GOSUB write &DMC_BASE+0x0010 0x11100000
//Wait 100ms
	Outp32(DMC_BASE+0x0010 ,0x11100000);
	DMC_Delay(100);

//;------------------------------------------------------------------------------
//; 43.-2 Clock change to High
//;------------------------------------------------------------------------------
//;;; DMC CLK Setting ;;;
//; DMC(1/2:27), DPHY(1/2:23), DMC_PRE(1/8:19), COREP(1/2:15), CORED(1/2:11), MPLL_PRE(1/1:8)
//&uBits=(0.<<27.)|(1.<<23.)|(1.<<19.)|(1.<<15.)|(1.<<11.)|(0.<<8.);For DMC 400Mhz~667Mhz
//d.s SD:0x105c0504 %LE %LONG &uBits //"rCLK_DIV_CORE_1"
	uBits =(0<<27)|(1<<23)|(1<<19)|(1<<15)|(1<<11)|(0<<8);
	Outp32(0x105c0504 ,uBits);

//;------------------------------------------------------------------------------
//; 44. If power_down modes are required, set the MemControl register.
//;------------------------------------------------------------------------------

//                             ;0x00312700
//GOSUB write &DMC_BASE+0x0004 0x00312723   ; Memory Control Register      step 9	//Phycon0 는 언제 setting 하나?
//					; [22:20] 3'b011 Memory Burst length 8
//					;	[19:16] 4'b0001	 2chips
//					; [15:12] 4'b0010 Width of Memory Data Bus 32bit
//					; [11:8]4'b0111 LPDDR3
//					; [7:6]	2'b00 Additional Latency for PALL in cclk cycle
//					; [5] 1'b1 dynamic self refresh eanble
//					; [4] 1'b0 timeout precharge
//					; [3:2] 1'b0 type of dynamic power down , active/precharge power down
//					; [1] 1'b1 dynamic power down enable
//					; [0] 1'b1 clk stop during idle
	Outp32(DMC_BASE+0x0004 ,0x00312723);

//; Start Auto-refresh
//;just before setting value   0x0FFF1000 ; DFI_INIT_START '0' & aref'0'   step 6
//;gaia setting                0x0FFF10E4 ; [7:6] 0x3 PHY Driving type ->static pull down -> spec out

//;------------------------------------------------------------------------------
//; 45. Set ConControl to turn on an auto refresh
//;------------------------------------------------------------------------------
//																		; [2:1] 0x2 ????
//GOSUB write &DMC_BASE+0x0000 0x0FFF1028 ; [5] 1b'1 Auto Refresh Counter enable
//                                        ; [3] 1b'1 Automatic control for ctrl_pd and pulldown control
	Outp32(DMC_BASE+0x0000 ,0x0FFF1028);
//;------------------------------------------------------------------------------
//; 46. Set the ALL_INIT_INDI register.
//;------------------------------------------------------------------------------
//GOSUB write &DMC_BASE+0x0400 0x00000001
	Outp32(DMC_BASE+0x0400 ,0x00000001 );


}

void BTS_DMC_init(void)
{
	Outp32(0x106F0000+0x00B8, 0x00000080);
	Outp32(0x106F0000+0x00B0, 0x00000080);
	Outp32(0x106F0000+0x00A8, 0x00000080);
	Outp32(0x106F0000+0x00A0, 0x00000080);
}

void mem_ctrl_init (void)
{
	InitDMC(543, 1);
	BTS_DMC_init();
}

