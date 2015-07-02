/*
 * Memory setup for XYREF5430 board based on EXYNOS5430
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
#include <asm/arch/smc.h>
#include <asm/arch/pmu.h>
#include "setup.h"

#define Outp32(addr, data)	(*(volatile u32 *)(addr) = (data))
#define Inp32(addr)		(*(volatile u32 *)(addr))
#define GetBits(uAddr, uBaseBit, uMaskValue) \
    ((Inp32(uAddr)>>(uBaseBit))&(uMaskValue))
#define GetPOPType()		(GetBits(0x10000004, 4, 0x3))
#undef FALSE
#undef TRUE

#define FALSE 0
#define TRUE 1
#define false 0
#define true 1

#define PWMTIMER_BASE			(0x14DD0000)						//0x12D90000

#define rTCFG0					(PWMTIMER_BASE+0x00)
#define rTCFG1					(PWMTIMER_BASE+0x04)
#define rTCON					(PWMTIMER_BASE+0x08)
#define rTCNTB0					(PWMTIMER_BASE+0x0C)
#define rTCMPB0					(PWMTIMER_BASE+0x10)
#define rTCNTO0					(PWMTIMER_BASE+0x14)
#define rTINT_CSTAT				(PWMTIMER_BASE+0x44)





void InitMemClk(u32 uMemclk);

//----------------------------------------------------------------------------
//
//- DMC Initialzation Script for LPDDR3
//
//- Copyright 2013 by Samsung Electronics. All rights reserved.
//
//----------------------------------------------------------------------------

static void StartTimer0(void)
{
	u32 uTimer=0;
	u32 uTemp0,uTemp1;
	u32 uEnInt=0;
	u32 uDivider=0;
	u32 uDzlen=0;
	u32 uPrescaler=66;												//- Silicon : uPrescaler=66   /  FPGA : uPrescaler=24
	u32 uEnDz=0;
	u32 uAutoreload=1;
	u32 uEnInverter=0;
	u32 uTCNTB=0xffffffff;
	u32 uTCMPB=(u32)(0xffffffff/2);

	{
		uTemp0 = Inp32(rTCFG1);						// 1 changed into 0xf; dohyun kim 090211
		uTemp0 = (uTemp0 & (~ (0xf << 4*uTimer) )) | (uDivider<<4*uTimer);
		// TCFG1			init. selected MUX 0~4, 	select 1/1~1/16

		Outp32(rTCFG1,uTemp0);

		uTemp0 = Inp32(rTINT_CSTAT);
		uTemp0 = (uTemp0 & (~(1<<uTimer)))|(uEnInt<<(uTimer));
		//		selected timer disable, 		selected timer enable or diable.
		Outp32(rTINT_CSTAT,uTemp0);
	}

	{
		uTemp0 = Inp32(rTCON);
		uTemp0 = uTemp0 & (0xfffffffe);
		Outp32(rTCON, uTemp0);								// Timer0 stop

		uTemp0 = Inp32(rTCFG0);
		uTemp0 = (uTemp0 & (~(0xff00ff))) | ((uPrescaler-1)<<0) |(uDzlen<<16);
		//			init. except prescaler 1 value, 	set the Prescaler 0 value,	set the dead zone length.
		Outp32(rTCFG0, uTemp0);

		Outp32(rTCNTB0, uTCNTB);
		Outp32(rTCMPB0, uTCMPB);


		uTemp1 = Inp32(rTCON);
		uTemp1 = (uTemp1 & (~(0x1f))) |(uEnDz<<4)|(uAutoreload<<3)|(uEnInverter<<2)|(1<<1)|(0<<0);
		//		set all zero in the Tiemr 0 con., Deadzone En or dis, set autoload, 	set invert,  manual uptate, timer stop
		Outp32(rTCON, uTemp1);									//timer0 manual update
		uTemp1 = (uTemp1 & (~(0x1f))) |(uEnDz<<4)|(uAutoreload<<3)|(uEnInverter<<2)|(0<<1)|(1<<0);
		//		set all zero in the Tiemr 0 con., Deadzone En or dis, set autoload, 	set invert,  manual uptate disable, timer start
		Outp32(rTCON, uTemp1);									// timer0 start
	}
}

static void PWM_stop(u32 uNum)
{
    u32 uTemp;

    uTemp = Inp32(rTCON);

    if(uNum == 0)
		uTemp &= ~(0x1);
    else
		uTemp &= ~((0x10)<<(uNum*4));

    Outp32(rTCON,uTemp);
}

static u32 StopTimer0(void)
{
	u32 uVal = 0;

	PWM_stop(0);

	uVal = Inp32(rTCNTO0);
	uVal = 0xffffffff - uVal;

	return uVal;
}

#if 0
void DMC_Delay(int msec)
{
	volatile u32 i;
	for(;msec > 0; msec--);
		for(i=0; i<1000; i++) ;
}
#else
void DMC_Delay(u32 usec)
{
	u32 uElapsedTime, uVal;

	StartTimer0();

	while(1){
		uElapsedTime = Inp32(rTCNTO0);
		uElapsedTime = 0xffffffff - uElapsedTime;

		if(uElapsedTime > usec){			
			StopTimer0();
			break;
		}
	}
}
#endif



void Gate_CLKM_PHY(int gate_en)
{
	u32 temp;

	temp=Inp32(0x105B0800);

	if(gate_en){
		temp=temp&~(0x3<<16);
		Outp32(0x105B0800,temp);
	}
	else{
		temp=temp&~(0x3<<16);
		temp|=(0x3<<16);
		Outp32(0x105B0800,temp);
	}
}

int Check_MRStatus(u32 ch, u32 cs)
{
	u32 mr0;
	u32 resp;
	u32 loop_cnt=1000;
	int re=true;

	if(cs==0)				mr0=0x09000000;
	else					mr0=0x09100000;

	if(ch==0){
		do{
			Outp32(0x10400010, mr0);
			resp=Inp32(0x10400054)&0x1;
			loop_cnt--;

			if(loop_cnt==0){
				DMC_Delay(10);							//- Although DAI is not completed during polling it, end the loop when 10us delay is time over.
				re=false;
				break;
			}
		}while(resp);
	}
	else{
		do{
			Outp32(0x10440010, mr0);
			resp=Inp32(0x10440054)&0x1;
			loop_cnt--;

			if(loop_cnt==0){
				DMC_Delay(10);							//- Although DAI is not completed during polling it, end the loop when 10us delay is time over.
				re=false;
				break;
			}
		}while(resp);
	}

	return re;
}

int Issuing_Basic_DirectCMD(void)
{
	int re[]={true,true,true,true};
	int result=true;
	u32 ch_offset;
	u32 cs_offset;
	u32 ch,cs;
	u32 sw_n_warm_reset;

	sw_n_warm_reset=(Inp32(0x105C0404)>>28)&0x3;				//- RESETSTATE(0x105C_0404) : [29]=SWRST,  [28]=WRESET

	for(ch=0;ch<2;ch++){
		if(ch==0)			ch_offset=0;
		else				ch_offset=0x40000;

		for(cs=0;cs<2;cs++){
			if(cs==0)			cs_offset=0;
			else				cs_offset=0x100000;

			if(sw_n_warm_reset){
				Outp32( 0x10400010 + ch_offset, (0x08000000)|(cs_offset));					 //- Self Refresh Exit
				Outp32( 0x10400010 + ch_offset, (0x00000870)|(cs_offset));					 //- MR2 (MA[7:0]=0x02), RL/WL	 , RL&WL=OP[3:0]=0xC(RL/WL=14/8,for 825MHz), nWRE=OP[4]=1 (WR > 9(667MHz))
				Outp32( 0x10400010 + ch_offset, (0x0000060C)|(cs_offset));					 //- MR1 (MA[7:0]=0x01), nWR		 , BL=OP[2:0]=0x3, nWR=OP[7:5]=4 (for 825MHz)
			}
			else{
				Outp32( 0x10400010 + ch_offset, (0x07000000)|(cs_offset));					 //- NOP
				DMC_Delay(200); 															 //- Wait for 200us
				Outp32( 0x10400010 + ch_offset, (0x00071C00)|(cs_offset));					 //- MR63(MA[7:0]=0x3F), Reset
				DMC_Delay(1);																 //- Wait for 1us
				re[(ch<<1)|(cs)]=Check_MRStatus(ch,cs); 													 //- Check if DAI is complete.
				Outp32( 0x10400010 + ch_offset, (0x00010BFC)|(cs_offset));					 //- MR10(MA[7:0]=0x0A), Calibration, OP[7:0]=0xFF(Calibration command after initialization)
				DMC_Delay(1);																 //- Wait for 1us
				Outp32( 0x10400010 + ch_offset, (0x00000870)|(cs_offset));					 //- MR2 (MA[7:0]=0x02), RL/WL	 , RL&WL=OP[3:0]=0xC(RL/WL=14/8,for 825MHz), nWRE=OP[4]=1 (WR > 9(667MHz))
				Outp32( 0x10400010 + ch_offset, (0x0000060C)|(cs_offset));					 //- MR1 (MA[7:0]=0x01), nWR		 , BL=OP[2:0]=0x3, nWR=OP[7:5]=4 (for 825MHz)
			}

		}
	}

	result=(re[3])|(re[2])|(re[1])|(re[0]);

	return result;
}

void Issuing_Optinal_DirectCMD(u32 mem_ds, u32 odt)
{
	u32 ch_offset;
	u32 cs_offset;
	u32 ch,cs;
	u32 mr3,mr11;

	//*** Setting Memory Driver Strength & ODT Termination ***
	//---------------------------------------------------------------
	//- DS : 0x1 : 34.3 ohm typical pull-down/pull-up
	//- DS : 0x2 : 40 ohm typical pull-down/pull-up (default)
	//- DS : 0x3 : 48 ohm typical pull-down/pull-up
	//- DS : 0x9 : 34.3 ohm typical pull-down/40 ohm typical pull-up
	//- DS : 0xA : 40 ohm typical pull-down/48 ohm typical pull-up
	//- DS : 0xB : 34.3 ohm typical pull-down/48 ohm typical pull-up

	//- ODT Term : 0x0 : Disable (disable)
	//- ODT Term : 0x1 : RZQ/4(60 ohm, for 933MHz / 1066MHz)
	//- ODT Term : 0x2 : RZQ/2(120 ohm)
	//- ODT Term : 0x3 : RZQ/1(240 ohm)
	//---------------------------------------------------------------
	//- Selection of Memory Option : DS -> 0x2,   ODT Termination -> 0x0

	//* mr3 : I/O configuration
	if (mem_ds==0x1)				mr3=0x00000C04; 	// 34.3 ohm typical pull-down/pull-up
	else if (mem_ds==0x2)			mr3=0x00000C08; 	// 40 ohm typical pull-down/pull-up (default)
	else if (mem_ds==0x3)			mr3=0x00000C0C; 	// 48 ohm typical pull-down/pull-up
	else if (mem_ds==0x9)			mr3=0x00000C24; 	// 34.3 ohm typical pull-down/40 ohm typical pull-up
	else if (mem_ds==0xA)			mr3=0x00000C28; 	// 40 ohm typical pull-down/48 ohm typical pull-up
	else if (mem_ds==0xB)			mr3=0x00000C2C; 	// 34.3 ohm typical pull-down/48 ohm typical pull-up
	else							mr3=0x00000C08; 	// 40 ohm typical pull-down/pull-up (default)

	//* on die termination
	if (odt==0)						mr11=0x00010C00;		// ODT Disable
	else if (odt==1)				mr11=0x00010C04;		// RZQ/4
	else if (odt==2)				mr11=0x00010C08;		// RZQ/2
	else if (odt==3)				mr11=0x00010C0C;		// RZQ/1
	else							mr11=0x00010C00;		// ODT Disable

	for(ch=0;ch<2;ch++){
		if(ch==0)				ch_offset=0;
		else					ch_offset=0x40000;

		for(cs=0;cs<2;cs++){
			if(cs==0)			cs_offset=0;
			else				cs_offset=0x100000;

			Outp32(0x10400010 + ch_offset, (mr3)|(cs_offset) );						//- CHx, CSx mr3 command
			Outp32(0x10400010 + ch_offset, (mr11)|(cs_offset) ); 				    //- CHx, CSx mr11 command
		}
	}
}

int Init_Mem(u32 mem_ds, u32 odt)
{
	int re;
	u32 sw_n_warm_reset;

	sw_n_warm_reset=(Inp32(0x105C0404)>>28)&0x3;				//- RESETSTATE(0x105C_0404) : [29]=SWRST,  [28]=WRESET

	//*** Setting Memory Driver Strength & ODT Termination ***
	//---------------------------------------------------------------
	//- DS : 0x1 : 34.3 ohm typical pull-down/pull-up
	//- DS : 0x2 : 40 ohm typical pull-down/pull-up (default)
	//- DS : 0x3 : 48 ohm typical pull-down/pull-up
	//- DS : 0x9 : 34.3 ohm typical pull-down/40 ohm typical pull-up
	//- DS : 0xA : 40 ohm typical pull-down/48 ohm typical pull-up
	//- DS : 0xB : 34.3 ohm typical pull-down/48 ohm typical pull-up

	//- ODT Term : 0x0 : Disable (disable)
	//- ODT Term : 0x1 : RZQ/4(60 ohm, for 933MHz / 1066MHz)
	//- ODT Term : 0x2 : RZQ/2(120 ohm)
	//- ODT Term : 0x3 : RZQ/1(240 ohm)
	//---------------------------------------------------------------
	//- Selection of Memory Option : DS -> 0x2,   ODT Termination -> 0x0

	re=Issuing_Basic_DirectCMD();							//- Issuing DirectCMD for CHx, CSx
	if (!sw_n_warm_reset)
		Issuing_Optinal_DirectCMD(mem_ds,odt);					//- Setting Memory Driver Strength(MR3) & ODT Termination(MR11)

	return re;
}

int DMCInit_For_LPDDR3(int initial, int nEnableForce, u32 uPopOption)
{
	u32 lock_val;
	int re=true;
#if defined(CONFIG_SMC_CMD)
       reg_arr_t reg_arr;
#endif
	//u32 sw_n_warm_reset;
	u32 loop_cnt=0;

	//Assert(uPopOption<=1);										//- MCP Size : 0 --> 2GB,                  1 --> : 3GB

	//sw_n_warm_reset=(Inp32(0x105C0404)>>28)&0x3;				//- RESETSTATE(0x105C_0404) : [29]=SWRST,  [28]=WRESET

	if(initial == true)
	{
		InitMemClk(50);
	}

	//-----------------------------------------------------------------------
	//--- PHASE 1 : Initializing DREX                                  			                 ---
	//-----------------------------------------------------------------------
	Outp32( 0x10400310, 0x00000021 );					 		//- DREX CH0 : ¡ÚPIPELINE_CONFIG reg, dq_pipeline[6:4]=2, ca_pipeline[6:4]=1
	Outp32( 0x10440310, 0x00000021 );							//- DREX CH1 : ¡ÚPIPELINE_CONFIG reg, dq_pipeline[6:4]=2, ca_pipeline[6:4]=1

	Outp32( 0x10400000+0x0014, 0x0 );					 		//- DMC.PrechConfig0.tp_en=0x0, port_policy=0x0
	Outp32( 0x10400000+0x001C, 0xFFFFFFFF );					//- DMC.PrechConfig1 : tp_cnt3=0xFF, tp_cnt2=0xFF, tp_cnt1=0xFF, tp_cnt0=0xFF
	Outp32( 0x10440000+0x0014, 0x0 );					 		//- DMC.PrechConfig0.tp_en=0x0, port_policy=0x0
	Outp32( 0x10440000+0x001C, 0xFFFFFFFF );					//- DMC.PrechConfig1 : tp_cnt3=0xFF, tp_cnt2=0xFF, tp_cnt1=0xFF, tp_cnt0=0xFF

	Outp32( 0x10400000+0x0004, 0x202600 );						//- set dynamic powe down to OFF mode for CH0.
	Outp32( 0x10440000+0x0004, 0x202600 );						//- set dynamic powe down to OFF mode for CH1.

	Outp32( 0x10400000+0x0018, 0x3000 );						//- CH0. DMC.PHYCONTROL0.sl_dll_dyn_con=0x0
	Outp32( 0x10440000+0x0018, 0x3000 );						//- CH1. DMC.PHYCONTROL0.sl_dll_dyn_con=0x0

	Outp32( 0x10400000+0x0000, 0xFFF1100 ); 					//- CH0. DMC.PHYCONTROL.io_pd_con=0x0, DMC.CONCONTROL.update_mode[3]=0, aref disabled
	Outp32( 0x10440000+0x0000, 0xFFF1100 ); 					//- CH1. DMC.PHYCONTROL.io_pd_con=0x0, DMC.CONCONTROL.update_mode[3]=0, aref disabled

	if(initial == true)
	{
		#if 0
		Outp32( 0x10420020, 0x70707070 );					 	//-  PHY0 : OFFSETR_CON0 : offsetr3=0x70,offsetr2=0x70,offsetr1=0x70,offsetr0=0x70
		Outp32( 0x10420030, 0x70707070 );					 	//-  PHY0 : OFFSETW_CON0 : offsetw3=0x7F,offsetw2=0x7F,offsetw1=0x7F,offsetw0=0x7F
		Outp32( 0x10420050, 0x00000070 );					 	//-  PHY0 OFFSETD_CON0(0x50).upd_mode=0(PHY-Initiated Update Mode), offsetd=0x18
		Outp32( 0x10460020, 0x70707070 );					 	//-  PHY1 : OFFSETR_CON0 : offsetr3=0x70,offsetr2=0x70,offsetr1=0x70,offsetr0=0x70
		Outp32( 0x10460030, 0x70707070 );					 	//-  PHY1 : OFFSETW_CON0 : offsetw3=0x7F,offsetw2=0x7F,offsetw1=0x7F,offsetw0=0x7F
		Outp32( 0x10460050, 0x00000070 );					 	//-  PHY1 OFFSETD_CON0(0x50).upd_mode=0(PHY-Initiated Update Mode), offsetd=0x18

		Outp32( 0x10400000, 0xFFF1100 );						//- DREX0 : CONCONTROL.update_mode[3]=0
		Outp32( 0x10440000, 0xFFF1100 );						//- DREX1 : CONCONTROL.update_mode[3]=0

		//- Update DLL Information for CH0
		Outp32( 0x10420000+0x0050, 0x1000070 ); 				//- PHY0 ctrl_resync=1
		Outp32( 0x10420000+0x0050, 0x70 );					  	//- PHY0 ctrl_resync=0
		//- Update DLL Information for CH1
		Outp32( 0x10460000+0x0050, 0x1000070 ); 				//- PHY1 ctrl_resync=1
		Outp32( 0x10460000+0x0050, 0x70 );					  	//- PHY1 ctrl_resync=0
		#endif
	}

	//-- PHY CH0,1 : GNR_CON0 reg, ctrl_upd_mode[31:30]=0x1, ctrl_upd_range[29:28]=0x0, ctrl_dfdqs[26]=0x1, ctrl_ddr_mode[25:24]=0x3, ctrl_fnc_fb[23:21]=0x0
	//- 						   ctrl_wrlat[20:16]=0x7(WL+1), ctrl_bstlen[12:8]=0x8, ctrl_ckdis[6]=0x0, ctrl_cmosrcv[5]=0x0, ctrl_rdlat[4:0]=0xC
	Outp32( 0x10420000, 0x4709080E );                  		 	//- PHY0 : GNR_CON0, for 825MHz on SILICON
	Outp32( 0x10460000, 0x4709080E );                   	 	//- PHY1 : GNR_CON0, for 825MHz on SILICON

	Outp32( 0x10420018, 0x0000000f );					 		//- PHY0 : LP_CON0(0x18), Active HIGH signal to pull-up or down PDQS/NDQS signals.
	Outp32( 0x10460018, 0x0000000f );					 		//- PHY1 : LP_CON0(0x18), Active HIGH signal to pull-up or down PDQS/NDQS signals.

	Outp32( 0x10420064, 0x0000000E );							//- PHY0 : LP_DDR_CON3.cmd_read[12:0]=0x000E (LPDDR2/LPDDR3)
	Outp32( 0x10460064, 0x0000000E );							//- PHY1 : LP_DDR_CON3.cmd_read[12:0]=0x000E (LPDDR2/LPDDR3)

	Outp32( 0x10420068, 0x0000000F );							//- PHY0 : LP_DDR_CON4.cmd_default[12:0]=0x000F (LPDDR2/LPDDR3)
	Outp32( 0x10460068, 0x0000000F );							//- PHY1 : LP_DDR_CON4.cmd_default[12:0]=0x000F (LPDDR2/LPDDR3)

	//- ¡Ú Execute ZQ calibration
	// ¡Ø Note : Don't execute ZQ calibration for PHY0
	// ¡Ø Note : It needs to execute ZQ calibration for only PHY1
	Outp32( 0x10420000+0x00C0, 0xE0C7304 );                     //- Set PHY0.ZQ_CON0.zq_mode_dds[26:24]=0x6, PHY0.ZQ_CON0.zq_manual_mode[3:2]=1

	Outp32( 0x10460000+0x00C0, 0xE0C7304 );                     //- Set PHY1.ZQ_CON0.zq_mode_dds[26:24]=0x6, PHY1.ZQ_CON0.zq_manual_mode[3:2]=1
	Outp32( 0x10460000+0x00C0, 0xE0C7306 );                     //- Set PHY1.ZQ_CON0.zq_manual_str[1]=1
	do{
		if(loop_cnt==100000){													//- Just Delay
			loop_cnt=0;
			break;
		}
		else					loop_cnt++;
	}while( ( Inp32( 0x10460000+0x00C4 ) & 0x1 ) != 0x1 );	        //- Wait for PHY1.ZQ_CON1.zq_done[0]
	Outp32( 0x10460000+0x00C0, 0xE0C7304 );                     //- Set PHY1.ZQ_CON0.zq_manual_str[1]=0

	//- Update DLL Information for CH0
	Outp32( 0x10420000+0x0050, 0x1000000 );                     //- PHY0 ctrl_resync=1
	Outp32( 0x10420000+0x0050, 0x0 );                    		//- PHY0 ctrl_resync=0
	//- Update DLL Information for CH1
	Outp32( 0x10460000+0x0050, 0x1000000 );                     //- PHY1 ctrl_resync=1
	Outp32( 0x10460000+0x0050, 0x0 );                    		//- PHY1 ctrl_resync=0

	Outp32( 0x10420000+0x00C4, 0x3f0000 );                    	//- Set PHY0.ZQ_CON1.zq_vref[21:16]=0x20
	Outp32( 0x10460000+0x00C4, 0x3f0000 );                    	//- Set PHY1.ZQ_CON1.zq_vref[21:16]=0x20
	Outp32( 0x1042000c, 0x42101000 );                    		//- PHY0 : CAL_CON2(0x0C), ctrl_shgate=1
	Outp32( 0x1046000c, 0x42101000 );                   		//- PHY1 : CAL_CON2(0x0C), ctrl_shgate=1


	//-Setting ODT feature in DREX side for Memory ODT(LPDDR3)

	if(initial == true)
	{
		#if 0
		//- Disable Off after forcing max lock_value(0x1FF)
		Outp32( 0x10420000+0x00B0, 0x1010FFF0 );				//- PHY0. ctrl_force[15:7]=0x1FF
		Outp32( 0x10420000+0x00B0, 0x1010FFD0 );				//- disable ctrl_dll_on & ctrl_start for PHY0.
		Outp32( 0x10460000+0x00B0, 0x1010FFF0 );				//- PHY1. ctrl_force[15:7]=0x1FF
		Outp32( 0x10460000+0x00B0, 0x1010FFD0 );				//- disable ctrl_dll_on & ctrl_start for PHY1.

		//- Update DLL Information for CH0
		Outp32( 0x10420000+0x0050, 0x1000070 ); 				//- PHY0 ctrl_resync=1
		Outp32( 0x10420000+0x0050, 0x70 );					  	//- PHY0 ctrl_resync=0
		//- Update DLL Information for CH1
		Outp32( 0x10460000+0x0050, 0x1000070 ); 				//- PHY1 ctrl_resync=1
		Outp32( 0x10460000+0x0050, 0x70 );					  	//- PHY1 ctrl_resync=0
		#endif
	}
	Outp32( 0x10400000+0x0000, 0xFFF1100 ); 				   	//- set CA Swap mode for CH0.
	Outp32( 0x10440000+0x0000, 0xFFF1100 ); 				   	//- set CA Swap mode for CH1.

	Outp32( 0x10400000+0x0018, 0x3080 );						//- Set Resync Enable During PAUSE handshaking for CH0.
	Outp32( 0x10440000+0x0018, 0x3080 );						//- Set Resync Enable During PAUSE handshaking for CH1.

	Outp32( 0x10400000+0x0000, 0x1FFF1100 );					//- assert dfi_init_start for CH0
	loop_cnt=0;
	do{
		if(loop_cnt==100000){													//- Just Delay
			loop_cnt=0;
			break;
		}
		else					loop_cnt++;
	}while( ( Inp32( 0x10400000+0x0040 ) & 0x8 ) != 0x8 );		//- Wait for DMC.dfi_init_complete_ch0
	Outp32( 0x10400000+0x0000, 0xFFF1100 ); 				   	//- deassert dfi_init_start for CH0

	Outp32( 0x10440000+0x0000, 0x1FFF1100 );					//- assert dfi_init_start for CH1
	loop_cnt=0;
	do{
		if(loop_cnt==100000){													//- Just Delay
			loop_cnt=0;
			break;
		}
		else					loop_cnt++;
	}while( ( Inp32( 0x10440000+0x0040 ) & 0x8 ) != 0x8 );		//- Wait for DMC.dfi_init_complete_ch1
	Outp32( 0x10440000+0x0000, 0xFFF1100 ); 				   	//- deassert dfi_init_start for CH1

	//- Update DLL Information for CH0
	Outp32( 0x10420000+0x0050, 0x1000000 );                    	//- PHY0 ctrl_resync=1
	Outp32( 0x10420000+0x0050, 0x0 );                    		//- PHY0 ctrl_resync=0
	//- Update DLL Information for CH1
	Outp32( 0x10460000+0x0050, 0x1000000 );                    	//- PHY1 ctrl_resync=1
	Outp32( 0x10460000+0x0050, 0x0 );                    		//- PHY1 ctrl_resync=0

	Outp32( 0x10400000+0x0018, 0x5080 );						//- Set Recovery Time From PHY clock Gate for CH0.
	Outp32( 0x10440000+0x0018, 0x5080 );						//- Set Recovery Time From PHY clock Gate for CH1.

	//Outp32( 0x10400000+0x0018, 0x5084 );							//- Drive Memory DQ Signals Enable for CH0.
	//Outp32( 0x10440000+0x0018, 0x5084 );							//- Drive Memory DQ Signals Enable for CH1.

	Outp32( 0x10400004, 0x28312700 );					 		//- DREX0 : num_chunk[19:18]=0x0 (1 chunk enable)
	Outp32( 0x10440004, 0x28312700 );					 		//- DREX1 : num_chunk[19:18]=0x0 (1 chunk enable)

	//*** Setting MemConfig0/1 : Interleaving/CHIP_MAP/COL/ROW/BANK bits ***
	//-------------------------------------------------------------------------
	//- £ªAddress Map : Split Column Interleaving(default)
	//- £ªbank_lsb : LSB of Bank Bit Position in Complex Interleaved Mapping
	//- 0x0 = bit position [8](column low size = 256B)
	//- 0x1 = bit position [9](column low size = 512B)
	//- 0x2 = bit position [10](column low size = 1KB)
	//- 0x3 = bit position [11](column low size = 2KB)
	//- 0x4 = bit position [12](column low size = 4KB)
	//- 0x5 = bit position [13](column low size = 8KB)

	//- £ªIf bit_sel_en=1 (Randomized Interleaving Enable)
	//- £ªbit_sel : Bit Selection for Randomized Interleaved Address mapping
	//- 0x0: bit position = [14:12] (if rank_inter_en is enabled, [15:12])
	//- 0x1: bit position = [20:18] (if rank_inter_en is enabled, [21:18])
	//- 0x2: bit position = [24:22] (if rank_inter_en is enabled, [25:22])
	//- 0x3: bit position = [26:24] (if rank_inter_en is enabled, [27:24])
	//-------------------------------------------------------------------------
	#if 1
	if(uPopOption==1)
	{
#if defined(CONFIG_SMC_CMD)
	       reg_arr.set0.addr = 0x10410f10;
	       reg_arr.set0.val = 0x462333;
	       reg_arr.set1.addr = 0x10410f14;
	       reg_arr.set1.val = 0x462333;
	       reg_arr.set4.addr = 0x10450f10;
	       reg_arr.set4.val = 0x462333;
	       reg_arr.set5.addr = 0x10450f14;
	       reg_arr.set5.val = 0x462333;
	       reg_arr.set2.addr = 0x10410f00;
	       reg_arr.set2.val = 0x00010006;
	       reg_arr.set6.addr = 0x10450f00;
	       reg_arr.set6.val = 0x00010006;
	       reg_arr.set3.addr = 0x10410f20;
	       reg_arr.set3.val = 0x00000002;
	       reg_arr.set7.addr = 0x10450f20;
	       reg_arr.set7.val = 0x00000002;
	       set_secure_reg((u32)&reg_arr, 8);
#else
		Outp32( 0x10410000+0x0f10, 0x462333 );					  //- CH0. MemConfig0 : bank_lsb=0x4, rank_inter_en=0x0, bit_sel_en=0x0, bit_sel=0x2
		Outp32( 0x10410000+0x0f14, 0x462333 );					  //- CH0. MemConfig1 : bank_lsb=0x4, rank_inter_en=0x0, bit_sel_en=0x0, bit_sel=0x2
		Outp32( 0x10450000+0x0f10, 0x462333 );					  //- CH1. MemConfig0 : bank_lsb=0x4, rank_inter_en=0x0, bit_sel_en=0x0, bit_sel=0x2
		Outp32( 0x10450000+0x0f14, 0x462333 );					  //- CH1. MemConfig1 : bank_lsb=0x4, rank_inter_en=0x0, bit_sel_en=0x0, bit_sel=0x2

		Outp32( 0x10410f00, 0x00010006 );                    //-  DREX0 chunk_start[23:16]=0x1 means that chunk base is 0x2(0x20000000), chunk_end[7:0]=0x6 means that chunk base is 0xD(0xD0000000)
		Outp32( 0x10450f00, 0x00010006 );                    //-  DREX1 chunk_start[23:16]=0x1 means that chunk base is 0x2(0x20000000), chunk_end[7:0]=0x6 means that chunk base is 0xD(0xD0000000)

		Outp32( 0x10410f20, 0x00000002 );                    //-  DREX0 chip0_size[7:0]=0x2 (chip size is 768MB)
		Outp32( 0x10450f20, 0x00000002 );                    //-  DREX1 chip0_size[7:0]=0x2 (chip size is 768MB)
#endif
		Outp32( 0x10400020, 0x00002626 );                    //-  DREX0 : ACLK/CCLK=400MHz, CLK2X=825MHz(on SILICON), t_rfcpb1[13:8]=60ns/2.5ns=24(0x18)(for timing set #1), t_rfcpb0[5:0]=60ns/2.5ns=24(0x18)(for timing set #0)
		Outp32( 0x10440020, 0x00002626 );                    //-  DREX1 : ACLK/CCLK=400MHz, CLK2X=825MHz(on SILICON), t_rfcpb1[13:8]=60ns/2.5ns=24(0x18)(for timing set #1), t_rfcpb0[5:0]=60ns/2.5ns=24(0x18)(for timing set #0)
	}
	else
	{
#if defined(CONFIG_SMC_CMD)
	       reg_arr.set0.addr = 0x10410f10;
	       reg_arr.set0.val = 0x462323;
	       reg_arr.set1.addr = 0x10410f14;
	       reg_arr.set1.val = 0x462323;
	       reg_arr.set4.addr = 0x10450f10;
	       reg_arr.set4.val = 0x462323;
	       reg_arr.set5.addr = 0x10450f14;
	       reg_arr.set5.val = 0x462323;
	       reg_arr.set2.addr = 0x10410f00;
	       reg_arr.set2.val = 0x00010004;
	       reg_arr.set6.addr = 0x10450f00;
	       reg_arr.set6.val = 0x00010004;
	       reg_arr.set3.addr = 0x10410f20;
	       reg_arr.set3.val = 0x00000001;
	       reg_arr.set7.addr = 0x10450f20;
	       reg_arr.set7.val = 0x00000001;
	       set_secure_reg((u32)&reg_arr, 8);
#else
		Outp32( 0x10410000+0x0f10, 0x462323 );                    //- CH0. MemConfig0 : bank_lsb=0x4, rank_inter_en=0x0, bit_sel_en=0x0, bit_sel=0x2
		Outp32( 0x10410000+0x0f14, 0x462323 );                    //- CH0. MemConfig1 : bank_lsb=0x4, rank_inter_en=0x0, bit_sel_en=0x0, bit_sel=0x2
		Outp32( 0x10450000+0x0f10, 0x462323 );                    //- CH1. MemConfig0 : bank_lsb=0x4, rank_inter_en=0x0, bit_sel_en=0x0, bit_sel=0x2
		Outp32( 0x10450000+0x0f14, 0x462323 );                    //- CH1. MemConfig1 : bank_lsb=0x4, rank_inter_en=0x0, bit_sel_en=0x0, bit_sel=0x2

		Outp32( 0x10410f00, 0x00010004 );                    //-  DREX0 chunk_start[23:16]=0x1 means that chunk base is 0x2(0x20000000), chunk_end[7:0]=0x4 means that chunk base is 0x9(0x90000000)
		Outp32( 0x10450f00, 0x00010004 );                    //-  DREX1 chunk_start[23:16]=0x1 means that chunk base is 0x2(0x20000000), chunk_end[7:0]=0x6 means that chunk base is 0x9(0x90000000)

		Outp32( 0x10410f20, 0x00000001 );                    //-  DREX0 chip0_size[7:0]=0x1 (chip size is 512MB)
		Outp32( 0x10450f20, 0x00000001 );                    //-  DREX1 chip0_size[7:0]=0x1 (chip size is 512MB)
#endif
		Outp32( 0x10400020, 0x00001919 );                    //-  DREX0 : ACLK/CCLK=400MHz, CLK2X=825MHz(on SILICON), t_rfcpb1[13:8]=60ns/2.5ns=24(0x18)(for timing set #1), t_rfcpb0[5:0]=60ns/2.5ns=24(0x18)(for timing set #0)
		Outp32( 0x10440020, 0x00001919 );                    //-  DREX1 : ACLK/CCLK=400MHz, CLK2X=825MHz(on SILICON), t_rfcpb1[13:8]=60ns/2.5ns=24(0x18)(for timing set #1), t_rfcpb0[5:0]=60ns/2.5ns=24(0x18)(for timing set #0)
	}
	#endif

	Outp32( 0x104000E0, 0x00000000 );					 		//-  DREX0 Timing Parameter Set #0 Enable
	Outp32( 0x104400E0, 0x00000000 );					 		//-  DREX1 Timing Parameter Set #0 Enable

	Outp32( 0x10400030, 0x0005002E );						//-  DREX0 : rclk=24MHz, t_refipb=0x5, t_refi=0x2E
	Outp32( 0x10440030, 0x0005002E );						//-  DREX1 : rclk=24MHz, t_refipb=0x5, t_refi=0x2E

	Outp32( 0x10400044, 0x00002270 );					 		//-  DREX0, ETCTIMING, t_mrr[13:12]=0x2(for LPDDR3), t_srr[11:10]=0x2(for WideIO), t_src[7:4]=0x7(for WideIO)
	Outp32( 0x10440044, 0x00002270 );					 		//-  DREX1, ETCTIMING, t_mrr[13:12]=0x2(for LPDDR3), t_srr[11:10]=0x2(for WideIO), t_src[7:4]=0x7(for WideIO)

	Outp32( 0x1040004C, 0x00000003 );					 		//-  DREX0, RDFETCH0(timing parameter set #0)
	Outp32( 0x1044004C, 0x00000003 );					 		//-  DREX1, RDFETCH0(timing parameter set #0)

	Outp32( 0x10400034, 0x575A9713 );                    		//-  TIMINGROW0 : DREX0, CLK2X=825MHz
	Outp32( 0x10440034, 0x575A9713 );                    		//-  TIMINGROW0 : DREX1, CLK2X=825MHz
	Outp32( 0x10400038, 0x4740085E );                    		//-  TIMINGDATA0 : DREX0, CLK2X=825MHz
	Outp32( 0x10440038, 0x4740085E );                   		//-  TIMINGDATA0 : DREX1, CLK2X=825MHz
	Outp32( 0x1040003C, 0x545B0446 );                    		//-  TIMINGPOWER0 : DREX0, CLK2X=825MHz
	Outp32( 0x1044003C, 0x545B0446 );                    		//-  TIMINGPOWER0 : DREX1, CLK2X=825MHz

	Outp32( 0x10400058, 0x50000000 );                    		//-  TermControl0 : DREX0, CLK2X=800MHz
	Outp32( 0x10440058, 0x50000000 );                    		//-  TermControl0 : DREX1, CLK2X=800MHz

	Outp32( 0x10400050, 0x00000003 );                    		//-  DREX0, RDFETCH1(timing parameter set #1)
	Outp32( 0x10440050, 0x00000003 );                    		//-  DREX1, RDFETCH1(timing parameter set #1)

	Outp32( 0x104000E4, 0x575A9713 );                    		//-  TIMINGROW1 : DREX0, CLK2X=825MHz
	Outp32( 0x104400E4, 0x575A9713 );                    		//-  TIMINGROW1 : DREX1, CLK2X=825MHz
	Outp32( 0x104000E8, 0x4740085E );                    		//-  TIMINGDATA1 : DREX0, CLK2X=825MHz
	Outp32( 0x104400E8, 0x4740085E );                    		//-  TIMINGDATA1 : DREX1, CLK2X=825MHz
	Outp32( 0x104000EC, 0x545B0446 );                    		//-  TIMINGPOWER1 : DREX0, CLK2X=825MHz
	Outp32( 0x104400EC, 0x545B0446 );                    		//-  TIMINGPOWER1 : DREX1, CLK2X=825MHz

	Outp32( 0x1040005C, 0x50000000 );                    		//-  TermControl1 : DREX0, CLK2X=800MHz
	Outp32( 0x1044005C, 0x50000000 );                    		//-  TermControl1 : DREX1, CLK2X=800MHz

	#if 0
	Outp32( 0x10400060, 0x00ff4b20 );					 		//-  DREX0 : Setting QOSCONTROL0 : [27:16]=cfg_qos_th, [11:0]=cfg_qos
	Outp32( 0x10400068, 0x00ffc0c0 );					 		//-  DREX0 : Setting QOSCONTROL1 : [27:16]=cfg_qos_th, [11:0]=cfg_qos
	Outp32( 0x10400070, 0x00ff5d10 );					 		//-  DREX0 : Setting QOSCONTROL2 : [27:16]=cfg_qos_th, [11:0]=cfg_qos
	Outp32( 0x10400078, 0x00ffb130 );					 		//-  DREX0 : Setting QOSCONTROL3 : [27:16]=cfg_qos_th, [11:0]=cfg_qos
	Outp32( 0x10400080, 0x00ff61f0 );					 		//-  DREX0 : Setting QOSCONTROL4 : [27:16]=cfg_qos_th, [11:0]=cfg_qos
	Outp32( 0x10400088, 0x00ff0fc0 );					 		//-  DREX0 : Setting QOSCONTROL5 : [27:16]=cfg_qos_th, [11:0]=cfg_qos
	Outp32( 0x10400090, 0x00fff680 );					 		//-  DREX0 : Setting QOSCONTROL6 : [27:16]=cfg_qos_th, [11:0]=cfg_qos
	Outp32( 0x10400098, 0x00ff2160 );					 		//-  DREX0 : Setting QOSCONTROL7 : [27:16]=cfg_qos_th, [11:0]=cfg_qos
	Outp32( 0x104000a0, 0x00ff6dd0 );					 		//-  DREX0 : Setting QOSCONTROL8 : [27:16]=cfg_qos_th, [11:0]=cfg_qos
	Outp32( 0x104000a8, 0x00ff93c0 );					 		//-  DREX0 : Setting QOSCONTROL9 : [27:16]=cfg_qos_th, [11:0]=cfg_qos
	Outp32( 0x104000b0, 0x00ff2330 );					 		//-  DREX0 : Setting QOSCONTROL10 : [27:16]=cfg_qos_th, [11:0]=cfg_qos
	Outp32( 0x104000b8, 0x00fffd60 );					 		//-  DREX0 : Setting QOSCONTROL11 : [27:16]=cfg_qos_th, [11:0]=cfg_qos
	Outp32( 0x104000c0, 0x00ff48f0 );					 		//-  DREX0 : Setting QOSCONTROL12 : [27:16]=cfg_qos_th, [11:0]=cfg_qos
	Outp32( 0x104000c8, 0x00ffd750 );					 		//-  DREX0 : Setting QOSCONTROL13 : [27:16]=cfg_qos_th, [11:0]=cfg_qos
	Outp32( 0x104000d0, 0x00ff4580 );					 		//-  DREX0 : Setting QOSCONTROL14 : [27:16]=cfg_qos_th, [11:0]=cfg_qos
	Outp32( 0x104000d8, 0x000f6920 );					 		//-  DREX0 : Setting QOSCONTROL15 : [27:16]=cfg_qos_th, [11:0]=cfg_qos
	Outp32( 0x10440060, 0x00ff4b20 );					 		//-  DREX1 : Setting QOSCONTROL0 : [27:16]=cfg_qos_th, [11:0]=cfg_qos
	Outp32( 0x10440068, 0x00ffc0c0 );					 		//-  DREX1 : Setting QOSCONTROL1 : [27:16]=cfg_qos_th, [11:0]=cfg_qos
	Outp32( 0x10440070, 0x00ff5d10 );					 		//-  DREX1 : Setting QOSCONTROL2 : [27:16]=cfg_qos_th, [11:0]=cfg_qos
	Outp32( 0x10440078, 0x00ffb130 );					 		//-  DREX1 : Setting QOSCONTROL3 : [27:16]=cfg_qos_th, [11:0]=cfg_qos
	Outp32( 0x10440080, 0x00ff61f0 );					 		//-  DREX1 : Setting QOSCONTROL4 : [27:16]=cfg_qos_th, [11:0]=cfg_qos
	Outp32( 0x10440088, 0x00ff0fc0 );					 		//-  DREX1 : Setting QOSCONTROL5 : [27:16]=cfg_qos_th, [11:0]=cfg_qos
	Outp32( 0x10440090, 0x00fff680 );					 		//-  DREX1 : Setting QOSCONTROL6 : [27:16]=cfg_qos_th, [11:0]=cfg_qos
	Outp32( 0x10440098, 0x00ff2160 );					 		//-  DREX1 : Setting QOSCONTROL7 : [27:16]=cfg_qos_th, [11:0]=cfg_qos
	Outp32( 0x104400a0, 0x00ff6dd0 );					 		//-  DREX1 : Setting QOSCONTROL8 : [27:16]=cfg_qos_th, [11:0]=cfg_qos
	Outp32( 0x104400a8, 0x00ff93c0 );					 		//-  DREX1 : Setting QOSCONTROL9 : [27:16]=cfg_qos_th, [11:0]=cfg_qos
	Outp32( 0x104400b0, 0x00ff2330 );					 		//-  DREX1 : Setting QOSCONTROL10 : [27:16]=cfg_qos_th, [11:0]=cfg_qos
	Outp32( 0x104400b8, 0x00fffd60 );					 		//-  DREX1 : Setting QOSCONTROL11 : [27:16]=cfg_qos_th, [11:0]=cfg_qos
	Outp32( 0x104400c0, 0x00ff48f0 );					 		//-  DREX1 : Setting QOSCONTROL12 : [27:16]=cfg_qos_th, [11:0]=cfg_qos
	Outp32( 0x104400c8, 0x00ffd750 );					 		//-  DREX1 : Setting QOSCONTROL13 : [27:16]=cfg_qos_th, [11:0]=cfg_qos
	Outp32( 0x104400d0, 0x00ff4580 );					 		//-  DREX1 : Setting QOSCONTROL14 : [27:16]=cfg_qos_th, [11:0]=cfg_qos
	Outp32( 0x104400d8, 0x000f6920 );					 		//-  DREX1 : Setting QOSCONTROL15 : [27:16]=cfg_qos_th, [11:0]=cfg_qos
	#endif

	Outp32( 0x10400000+0x0100, 0x0 );					 		//- CH0 brbrsvcontrol
	Outp32( 0x10440000+0x0100, 0x0 );					 		//- CH1 brbrsvcontrol
	Outp32( 0x10400000+0x0104, 0x88888888 );					//- CH0 brbrsvconfig
	Outp32( 0x10440000+0x0104, 0x88888888 );					//- CH1 brbrsvconfig

	//-----------------------------------------------------------------------
	//--- PHASE 2 : Issuing DIRECTCMD, for LPDDR3 @ 0x800			                 ---
	//-----------------------------------------------------------------------
	//if((initial == true)&&(!sw_n_warm_reset))			// That means that sleep & wakeup is not , or This is for all reset state
	if(initial == true)
	{
		//*** Setting Memory Driver Strength & ODT Termination ***
		//---------------------------------------------------------------
		//- DS : 0x1 : 34.3 ohm typical pull-down/pull-up
		//- DS : 0x2 : 40 ohm typical pull-down/pull-up (default)
		//- DS : 0x3 : 48 ohm typical pull-down/pull-up
		//- DS : 0x9 : 34.3 ohm typical pull-down/40 ohm typical pull-up
		//- DS : 0xA : 40 ohm typical pull-down/48 ohm typical pull-up
		//- DS : 0xB : 34.3 ohm typical pull-down/48 ohm typical pull-up

		//- ODT Term : 0x0 : Disable (disable)
		//- ODT Term : 0x1 : RZQ/4(60 ohm, for 933MHz / 1066MHz)
		//- ODT Term : 0x2 : RZQ/2(120 ohm)
		//- ODT Term : 0x3 : RZQ/1(240 ohm)
		//---------------------------------------------------------------
		//- Selection of Memory Option : DS -> 0x2,   ODT Termination -> 0x0

		#if 1
			re=Init_Mem(2,0);
		#else
		//- [DREX0]
		//-------------------------------------------
		//- [CS0]
		Outp32( 0x10400010, 0x07000000 );					 	//- NOP
		DMC_Delay(200); 									 	//- Wait for 200us
		Outp32( 0x10400010, 0x00071C00 );					 	//- MR63(MA[7:0]=0x3F), Reset
		DMC_Delay(1); 										 	//- Wait for 1us
		re=Check_MRStatus(0,0);								 	//- Check if DAI is complete.
		Outp32( 0x10400010, 0x00010BFC );					 	//- MR10(MA[7:0]=0x0A), Calibration, OP[7:0]=0xFF(Calibration command after initialization)
		DMC_Delay(1); 										 	//- Wait for 1us
		Outp32( 0x10400010, 0x00000870 );					 	//- MR2 (MA[7:0]=0x02), RL/WL 	 , RL&WL=OP[3:0]=0xC(RL/WL=14/8,for 825MHz), nWRE=OP[4]=1 (WR > 9(667MHz))
		Outp32( 0x10400010, 0x0000060C );					 	//- MR1 (MA[7:0]=0x01), nWR		 , BL=OP[2:0]=0x3, nWR=OP[7:5]=4 (for 825MHz)

		//- [CS1]
		Outp32( 0x10400010, 0x07100000 );					 	//- NOP
		DMC_Delay(200); 									 	//- Wait for 200us
		Outp32( 0x10400010, 0x00171C00 );					 	//- MR63(MA[7:0]=0x3F), Reset
		DMC_Delay(1); 										 	//- Wait for 1us
		re=Check_MRStatus(0,1);								 	//- Check if DAI is complete.
		Outp32( 0x10400010, 0x00110BFC );					 	//- MR10(MA[7:0]=0x0A), Calibration, OP[7:0]=0xFF(Calibration command after initialization)
		DMC_Delay(1); 										 	//- Wait for 1us
		Outp32( 0x10400010, 0x00100870 );					 	//- MR2 (MA[7:0]=0x02), RL/WL 	 , RL&WL=OP[3:0]=0xC(RL/WL=14/8,for 825MHz), nWRE=OP[4]=1 (WR > 9(667MHz))
		Outp32( 0x10400010, 0x0010060C );					 	//- MR1 (MA[7:0]=0x01), nWR		 , BL=OP[2:0]=0x3, nWR=OP[7:5]=4 (for 825MHz)
		//-------------------------------------------

		//- [DREX1]
		//-------------------------------------------
		//- [CS0]
		Outp32( 0x10440010, 0x07000000 );					 	//- NOP
		DMC_Delay(200); 									 	//- Wait for 200us
		Outp32( 0x10440010, 0x00071C00 );					 	//- MR63(MA[7:0]=0x3F), Reset
		DMC_Delay(1); 										 	//- Wait for 1us
		re=Check_MRStatus(1,0);								 	//- Check if DAI is complete.
		Outp32( 0x10440010, 0x00010BFC );					 	//- MR10(MA[7:0]=0x0A), Calibration, OP[7:0]=0xFF(Calibration command after initialization)
		DMC_Delay(1); 										 	//- Wait for 1us
		Outp32( 0x10440010, 0x00000870 );					 	//- MR2 (MA[7:0]=0x02), RL/WL 	 , RL&WL=OP[3:0]=0xA(RL/WL=12/6,for 800MHz), nWRE=OP[4]=1 (WR > 9(600MHz))
		Outp32( 0x10440010, 0x0000060C );					 	//- MR1 (MA[7:0]=0x01), nWR		 , BL=OP[2:0]=0x3, nWR=OP[7:5]=2 (for 800MHz)

		//- [CS1]
		Outp32( 0x10440010, 0x07100000 );					 	//- NOP
		DMC_Delay(200); 									 	//- Wait for 200us
		Outp32( 0x10440010, 0x00171C00 );					 	//- MR63(MA[7:0]=0x3F), Reset
		DMC_Delay(1); 										 	//- Wait for 1us
		re=Check_MRStatus(1,1);								 	//- Check if DAI is complete.
		Outp32( 0x10440010, 0x00110BFC );					 	//- MR10(MA[7:0]=0x0A), Calibration, OP[7:0]=0xFF(Calibration command after initialization)
		DMC_Delay(1); 										 	//- Wait for 1us
		Outp32( 0x10440010, 0x00100870 );					 	//- MR2 (MA[7:0]=0x02), RL/WL 	 , RL&WL=OP[3:0]=0xA(RL/WL=12/6,for 800MHz), nWRE=OP[4]=1 (WR > 9(600MHz))
		Outp32( 0x10440010, 0x0010060C );					 	//- MR1 (MA[7:0]=0x01), nWR		 , BL=OP[2:0]=0x3, nWR=OP[7:5]=2 (for 800MHz)
		//-------------------------------------------


		//*** Setting Memory Driver Strength & ODT Termination ***
		//---------------------------------------------------------------
		//- DS : 0x1 : 34.3 ohm typical pull-down/pull-up
		//- DS : 0x2 : 40 ohm typical pull-down/pull-up (default)
		//- DS : 0x3 : 48 ohm typical pull-down/pull-up
		//- DS : 0x9 : 34.3 ohm typical pull-down/40 ohm typical pull-up
		//- DS : 0xA : 40 ohm typical pull-down/48 ohm typical pull-up
		//- DS : 0xB : 34.3 ohm typical pull-down/48 ohm typical pull-up

		//- ODT Term : 0x0 : Disable (disable)
		//- ODT Term : 0x1 : RZQ/4(60 ohm, for 933MHz / 1066MHz)
		//- ODT Term : 0x2 : RZQ/2(120 ohm)
		//- ODT Term : 0x3 : RZQ/1(240 ohm)
		//---------------------------------------------------------------
		//- Selection of Memory Option : DS -> 0x2,   ODT Termination -> 0x0
		Outp32( (0x10400000+0x0010), 0xC08 );					//- CH0, CS0. mr3 command
		DMC_Delay(0x100); //- wait 1ms
		Outp32( (0x10400000+0x0010), 0x10C00 ); 				//- CH0, CS0. mr11 command
		DMC_Delay(0x10000); //- wait 100ms
		Outp32( (0x10400000+0x0010), 0x100C08 );				//- CH0, CS1. mr3 command
		DMC_Delay(0x100); //- wait 1ms
		Outp32( (0x10400000+0x0010), 0x110C00 );				//- CH0, CS1. mr11 command
		DMC_Delay(0x10000); //- wait 100ms
		Outp32( (0x10440000+0x0010), 0xC08 );					//- CH1, CS0. mr3 command
		DMC_Delay(0x100); //- wait 1ms
		Outp32( (0x10440000+0x0010), 0x10C00 ); 				//- CH1, CS0. mr11 command
		DMC_Delay(0x10000); //- wait 100ms
		Outp32( (0x10440000+0x0010), 0x100C08 );				//- CH1, CS1. mr3 command
		DMC_Delay(0x100); //- wait 1ms
		Outp32( (0x10440000+0x0010), 0x110C00 );				//- CH1, CS1. mr11 command
		DMC_Delay(0x10000); //- wait 100ms
		#endif
	}
	else{				//- Sleep & Wakeup, SW reset, Warm Reset
		Outp32( (0x10400000+0x0010), 0x08000000 ); 				//- CH0, CS0. Self Refresh Exit Command for only sleep & wakeup
		Outp32( (0x10400000+0x0010), 0x08100000 ); 				//- CH0, CS0. Self Refresh Exit Command for only sleep & wakeup
		Outp32( (0x10440000+0x0010), 0x08000000 ); 				//- CH0, CS0. Self Refresh Exit Command for only sleep & wakeup
		Outp32( (0x10440000+0x0010), 0x08100000 ); 				//- CH0, CS0. Self Refresh Exit Command for only sleep & wakeup
	}

	Outp32( 0x10420020, 0x0 );					  				//-  PHY0 : OFFSETR_CON0 : offsetr3=0x08,offsetr2=0x08,offsetr1=0x08,offsetr0=0x08
	Outp32( 0x10420030, 0x0 );					  				//-  PHY0 : OFFSETW_CON0 : offsetw3=0x08,offsetw2=0x08,offsetw1=0x08,offsetw0=0x08
	Outp32( 0x10420050, 0x0 );					  				//-  PHY0 OFFSETD_CON0(0x50).upd_mode=0(PHY-Initiated Update Mode), offsetd=0x08
	Outp32( 0x10460020, 0x0 );					  				//-  PHY1 : OFFSETR_CON0 : offsetr3=0x08,offsetr2=0x08,offsetr1=0x08,offsetr0=0x08
	Outp32( 0x10460030, 0x0 );					  				//-  PHY1 : OFFSETW_CON0 : offsetw3=0x08,offsetw2=0x08,offsetw1=0x08,offsetw0=0x08
	Outp32( 0x10460050, 0x0 );					  				//-  PHY1 OFFSETD_CON0(0x50).upd_mode=0(PHY-Initiated Update Mode), offsetd=0x08

	//- Update DLL Information for CH0
	Outp32( 0x10420000+0x0050, 0x1000000 ); 				   	//- PHY0 ctrl_resync=1
	Outp32( 0x10420000+0x0050, 0x0 );					 		//- PHY0 ctrl_resync=0
	//- Update DLL Information for CH1
	Outp32( 0x10460000+0x0050, 0x1000000 ); 				   	//- PHY1 ctrl_resync=1
	Outp32( 0x10460000+0x0050, 0x0 );					 		//- PHY1 ctrl_resync=0


	//-----------------------------------------------------------------------
	//--- Executing DLL locking Process 								  ---
	//--- And Then, 													  ---
	//--- If training is required, Please execute it.					  ---
	//-----------------------------------------------------------------------
	if(initial == true)
	{
		InitMemClk(825);
	}

	Outp32( 0x10420000+0x00B0, 0x10100010|(0xF<<1) );                    //-  PHY0 : ctrl_dll_on[5] = 0, ctrl_start = 0, ctrl_ref=0xF
	Outp32( 0x10420000+0x00B0, 0x10100030|(0xF<<1) );                    //-  PHY0 : ctrl_dll_on[5] = 1, ctrl_ref=0xF
	Outp32( 0x10420000+0x00B0, 0x10100070|(0xF<<1) );                    //-  PHY0 : ctrl_start[6] = 1, ctrl_ref=0xF
	loop_cnt=0;
	do{
		if(loop_cnt==100000){													//- Just Delay
			loop_cnt=0;
			break;
		}
		else					loop_cnt++;
	}while( ( Inp32( 0x10420000+0x00B4 ) & 0x5 ) != 0x5 );	    //- Wait until PHY0. DLL is locked!

	if (nEnableForce == TRUE)
	{
		//¡Ú¡Ú¡Ú Open for only EVT0 Silicon, the code below is to force lock value for System Power Mode
		Outp32(0x10420000+0x00B0, Inp32(0x10420000+0x00B0)&~(0x1<<5));							//- PHY0 : ctrl_dll_on=0
		lock_val=(Inp32(0x10420000+0x00B4)>>8)&0x1FF;
		Outp32(0x10420000+0x00B0, (Inp32(0x10420000+0x00B0)&~(0x1FF<<7))|(lock_val<<7));		//- PHY0 : forcing lock value
	}

	Outp32( 0x10460000+0x00B0, 0x10100010|(0xF<<1) );                    //-  PHY1 : ctrl_dll_on[5] = 0, ctrl_start = 0
	Outp32( 0x10460000+0x00B0, 0x10100030|(0xF<<1) );                    //-  PHY1 : ctrl_dll_on[5] = 1
	Outp32( 0x10460000+0x00B0, 0x10100070|(0xF<<1) );                    //-  PHY1 : ctrl_start[6] = 1
	loop_cnt=0;
	do{
		if(loop_cnt==100000){													//- Just Delay
			loop_cnt=0;
			break;
		}
		else					loop_cnt++;
	}while( ( Inp32( 0x10460000+0x00B4 ) & 0x5 ) != 0x5 );	    //- Wait until PHY1. DLL is locked!

	if (nEnableForce == TRUE)
	{
		//¡Ú¡Ú¡Ú Open for only EVT0 Silicon, the code below is to force lock value for System Power Mode
		Outp32(0x10460000+0x00B0, Inp32(0x10460000+0x00B0)&~(0x1<<5));							//- PHY1 : ctrl_dll_on=0
		lock_val=(Inp32(0x10460000+0x00B4)>>8)&0x1FF;
		Outp32(0x10460000+0x00B0, (Inp32(0x10460000+0x00B0)&~(0x1FF<<7))|(lock_val<<7));		//- PHY1 : forcing lock value
	}

	//- Update DLL Information for CH0
	Outp32( 0x10420000+0x0050, 0x1000000 ); 				   	//- PHY0 ctrl_resync=1
	Outp32( 0x10420000+0x0050, 0x0 );					 		//- PHY0 ctrl_resync=0
	//- Update DLL Information for CH1
	Outp32( 0x10460000+0x0050, 0x1000000 ); 				   	//- PHY1 ctrl_resync=1
	Outp32( 0x10460000+0x0050, 0x0 );						 	//- PHY1 ctrl_resync=0


	Outp32( 0x10400000+0x0014, 0x0 );					 		//- DMC.PrechConfig0.tp_en=0x0, port_policy=0x0
	Outp32( 0x10400000+0x001C, 0xFFFFFFFF );					//- DMC.PrechConfig1 : tp_cnt3=0xFF, tp_cnt2=0xFF, tp_cnt1=0xFF, tp_cnt0=0xFF
	Outp32( 0x10440000+0x0014, 0x0 );					 		//- DMC.PrechConfig0.tp_en=0x0, port_policy=0x0
	Outp32( 0x10440000+0x001C, 0xFFFFFFFF );					//- DMC.PrechConfig1 : tp_cnt3=0xFF, tp_cnt2=0xFF, tp_cnt1=0xFF, tp_cnt0=0xFF

	Outp32( 0x10400000+0x0028, 0x1FFF000D );					//- DMC.PWRDNCONFIG.dsref_cnt=0x1FFF000D
	Outp32( 0x10400000+0x0004, 0x28312720 );					//- Set DMC.MEMCONTROL.dsref_en=0x20
	Outp32( 0x10440000+0x0028, 0x1FFF000D );					//- DMC.PWRDNCONFIG.dsref_cnt=0x1FFF000D
	Outp32( 0x10440000+0x0004, 0x28312720 );					//- Set DMC.MEMCONTROL.dsref_en=0x20

	Outp32( 0x10400000+0x0028, 0x1FFF000D );					//- Set Channel0. DMC.PWRDNCONFIG.dpwrdn_cyc=0xD
	Outp32( 0x10400000+0x0004, 0x28312722 );					//- Set Channel0. DMC.MEMCONTROL.dpwrdn_en=0x2, dpwrdn_type=0x0
	Outp32( 0x10440000+0x0028, 0x1FFF000D );					//- Set Channel1. DMC.PWRDNCONFIG.dpwrdn_cyc=0xD
	Outp32( 0x10440000+0x0004, 0x28312722 );					//- Set Channel1. DMC.MEMCONTROL.dpwrdn_en=0x2, dpwrdn_type=0x0

	Outp32( 0x10400000+0x0004, 0x20312723 );					//- Channel0. DMC.MEMCONTROL.clk_stop_en=0x1
	Outp32( 0x10440000+0x0004, 0x20312723 );					//- Channel1. DMC.MEMCONTROL.clk_stop_en=0x1

	Outp32( 0x10400000+0x0018, 0x5082 );						//- CH0. DMC.PHYCONTROL0.sl_dll_dyn_con=0x1
	Outp32( 0x10440000+0x0018, 0x5082 );						//- CH1. DMC.PHYCONTROL0.sl_dll_dyn_con=0x1

	Outp32( 0x10400000+0x0000, 0xFFF1180 ); 				   	//- Set CH0. DMC.PHYCONTROL.io_pd_con=0x2
	Outp32( 0x10440000+0x0000, 0xFFF1180 ); 				   	//- Set CH1. DMC.PHYCONTROL.io_pd_con=0x2

	Outp32( 0x10400000+0x0000, 0xFFF11A0 ); 				   	//- CH0. aref enabled
	Outp32( 0x10440000+0x0000, 0xFFF11A0 ); 				   	//- CH1. aref enabled

	Outp32( 0x10400000+0x0400, 0x1 );					 		//- CH0. all init complete!
	Outp32( 0x10440000+0x0400, 0x1 );					 		//- CH1. all init complete!

	Outp32( 0x10400000+0x0008, 0x3F );					  		//- CH0 Clock Gating Setting!
	Outp32( 0x10400000+0x0004, 0x28312722 );					//- Channel0. DMC.MEMCONTROL.clk_stop_en=0x0
	Outp32( 0x10440000+0x0008, 0x3F );					  		//- CH1 Clock Gating Setting!
	Outp32( 0x10440000+0x0004, 0x28312722 );					//- Channel1. DMC.MEMCONTROL.clk_stop_en=0x0

	//- Gating PHY0,1 clock source for SFR and logic not changing after DRAM initialization
	//- The function above is possible to reduce dynamic power consumption of PHY0,1
	//- Actually, Clock Source fof idle logic & SFR during dynamic operation is gated.
	//- When DRAM operation is idle, CG gating function of DREX operate to reduce MIF Power during idle.
	Outp32( 0x10420000+0x0018, 0x100F );                    	//- PHY0. Wrapper Power Mode Setting : LP_CON0[12] = wrapper_pd = 0x1
	Outp32( 0x10460000+0x0018, 0x100F );                    	//- PHY1. Wrapper Power Mode Setting : LP_CON0[12] = wrapper_pd = 0x1

	return re;
}

void mif_dvfs_init(void)
{
	unsigned int tmp;

	/* enable drex pause */
	tmp = Inp32(0x105B0000 + 0x1008);
	tmp |= 0x1;
	Outp32(0x105B0000 + 0x1008, tmp);

	/* Configuring Automatic Direct Command Operation */
	tmp = 0x00130010;
	Outp32(0x105E0000 + 0x1020, tmp);

	/* Decision for Timing Parameter Set Switch Control */
	tmp = 0x0;
	Outp32(0x10400000 + 0x00E0, tmp);
	Outp32(0x10440000 + 0x00E0, tmp);

	/* Setting DVFS Mode Control of PHY */
	tmp = 0x0C0C2121;
	Outp32(0x10420000 + 0x00BC, tmp);
	tmp = 0x0C0C2121;
	Outp32(0x10460000 + 0x00BC, tmp);
}

void mem_ctrl_init(void)
{
	unsigned int wakeup_stat;

	wakeup_stat = readl(EXYNOS5430_POWER_WAKEUP_STAT);
	wakeup_stat &= WAKEUP_MASK;

	if (wakeup_stat) {
		DMCInit_For_LPDDR3(0, FALSE, GetPOPType());
	} else {
		DMCInit_For_LPDDR3(1, FALSE, GetPOPType());
	}

	/* prepare mif dvfs */
	mif_dvfs_init();
}
