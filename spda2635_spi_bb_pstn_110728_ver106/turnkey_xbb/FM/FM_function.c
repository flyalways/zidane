#include "..\Header\SPDA2K.h"
#include "..\DSP\dspuser.h"
#include "..\header\variables.h"
#include "FM_receiveH.h"

#define DSP_ADPCM_OUT_SZ_PER_CH	((((505-1)/4)+2)*2)

extern void FM_MUTE(void);
extern void FM_UnMUTE(void);
extern void FM_FMVolumeSet(U16 vol);
extern void FM_FMTune();//设台，tc_mute=1时设台后无声音（用作搜台时设台）
extern void FM_FMSeekFromCurrentCh(U8 seekUp);//seekUp=0向上搜台，seekUp=1向下搜台
extern void FM_Auto_SearchAllFreq(U16 x_FreqTemp);//全部搜台
extern void FM_Powerdown(void);//关掉FM
extern void FM_OPEN(void);//打开fm

void AccessKeyEvent_FM_PAUSE(void)//录音暂停时 ---sunzhk add
{
	switch(gc_KeyEvent)
	{
		case 0x02:	// Play
			L2_DSP_SendCommandSet(DCMD_Play);
			gs_System_State.c_Phase=TASK_PHASE_REC;
		break;

		case 0x06:	// M
		case 0x16:	// L-M
			REC_Stop();
			gs_System_State.c_Phase=TASK_PHASE_STOP;
	   	break;
	}
}

void AccessKeyEvent_FM_REC(void)//录音中 ---sunzhk add
{
	switch(gc_KeyEvent)
	{
		case 0x02:	// Play
			DSP_PauseCmd();
			gs_System_State.c_Phase=TASK_PHASE_PAUSE;
		break;

		case 0x06:	// M
		case 0x16:	// L-M
			REC_Stop();
			gs_System_State.c_Phase=TASK_PHASE_STOP;
	   	break;
	}
}

void AccessKeyEvent_FM_STOP(void)//未录音状态---sunzhk add 
{
	data	U8	tc_temp;
	xdata	U8	i;

	switch(gc_KeyEvent)
	{
		case 0x02:	// Play
			if(gb_FM_Mute==1)
			{
				gb_FM_Mute=0;
				FM_UnMUTE();
			}else
			{
				gb_FM_Mute=1;
				FM_MUTE();
			}
		break;

		case 0x03:	// Next
			gb_ADJ=0;
			if(gb_SetVol)
			{
				Linein_VolAdj();	// Volume Adjust
			}
			else if(gc_FMMenuItem==1)//菜单状态
			{
				if(gc_FMMenuNum==3)//标志第几个菜单
				{
					gc_FMMenuNum=4;
					LCM_ShowFMMenu(4);
					LCM_ShowFMMenu(5);
					LCM_ShowFMMenu(6);
					LCM_erase_one_page(6);
					LCM_erase_one_page(7);
				}
				else if(gc_FMMenuNum==6)
				{
					gc_FMMenuNum=0;
					LCM_ShowFMMenu(0);
					LCM_ShowFMMenu(1);
					LCM_ShowFMMenu(2);
					LCM_ShowFMMenu(3);
				}
				else
				{
					gc_FMMenuNum++;
					LCM_ShowFMMenu(gc_FMMenuNum-1);
					LCM_ShowFMMenu(gc_FMMenuNum);
				}
			}
			else if(gc_FMMenuItem==2) //标志录音质量的菜单
			{
				tc_temp=gc_RecordQuality;
				gc_RecordQuality++;
				if(gc_RecordQuality==3)
				{
					gc_RecordQuality=0;
				}
				LCM_ShowFMMenu_Quality(tc_temp);
				LCM_ShowFMMenu_Quality(gc_RecordQuality);
			}
			else if(gc_FMMenuItem==3)	// Order Record Setting//预约录音的菜单
			{
				gb_LCM_InverseDisp=1;
				if(gc_OrderRecordTimerStep==0)	// On-OFF
				{
					gb_OrderRecord=!gb_OrderRecord;
					LCM_ShowRecONOFFString();
				}
				else if(gc_OrderRecordTimerStep==1)	// +1H
				{
					gdw_OrderRecordTime+=3600;
					if(gdw_OrderRecordTime>=86400)
					{
						gdw_OrderRecordTime-=86400;
					}
					LCM_ShowRecordStartTime_Hour();
				}
				else if(gc_OrderRecordTimerStep==2)	// +1M
				{
					if(gdw_OrderRecordTime%3600==3540)
					{
						gdw_OrderRecordTime-=3540;
					}
					else
					{
						gdw_OrderRecordTime+=60;
					}
					LCM_ShowRecordStartTime_Minute();
				}
				else
				{
					if(gw_OrderRecDuration==7200)	// 120-Min
					{
						gw_OrderRecDuration=600;		// 10-Min
					}
					else
					{
						gw_OrderRecDuration+=600;
					}
					LCM_ShowDuration();
				}
				gb_LCM_InverseDisp=0;
			}
			else //跳到下一个保存的个频点
			{
				// Change to Next Channel Memory
				dbprintf("next channel\n");
				dbprintf("gc_FM_CHAmount=%bx,gc_FM_CH_COUNT=%bx\n",gc_FM_CHAmount,gc_FM_CH_COUNT);
				if(gc_FM_CHAmount>0)
				{
					gc_FM_CH_COUNT++;
					if(gc_FM_CH_COUNT==gc_FM_CHAmount)
					{
						gc_FM_CH_COUNT=0;
					}
					gw_FM_frequency=gw_FM_CH[gc_FM_CH_COUNT];//gw_FM_CH最大可以保存50个电台
				}else
				{
					if(gw_FM_frequency>=1080)
					{
						gw_FM_frequency=875;
					}else
					{
						gw_FM_frequency	++;
					}
				}
				dbprintf("gw_FM_frequency=%x\n",gw_FM_frequency);
				FM_FMTune();
			}
		break;

		case 0x04:	// Prev
			gb_ADJ=1;
			if(gb_SetVol)
			{
				Linein_VolAdj();	// Volume Adjust
			}
			else if(gc_FMMenuItem==1)
			{
				if(gc_FMMenuNum==0)
				{
					gc_FMMenuNum=6;
					LCM_ShowFMMenu(4);
					LCM_ShowFMMenu(5);
					LCM_ShowFMMenu(6);
					LCM_erase_one_page(6);
					LCM_erase_one_page(7);
				}
				else if(gc_FMMenuNum==4)
				{
					gc_FMMenuNum=3;
					LCM_ShowFMMenu(0);
					LCM_ShowFMMenu(1);
					LCM_ShowFMMenu(2);
					LCM_ShowFMMenu(3);
				}
				else
				{
					gc_FMMenuNum--;
					LCM_ShowFMMenu(gc_FMMenuNum+1);
					LCM_ShowFMMenu(gc_FMMenuNum);
				}
			}
			else if(gc_FMMenuItem==2)
			{
				tc_temp=gc_RecordQuality;
				gc_RecordQuality--;
				if(gc_RecordQuality==0xFF)
				{
					gc_RecordQuality=2;
				}
				LCM_ShowFMMenu_Quality(tc_temp);
				LCM_ShowFMMenu_Quality(gc_RecordQuality);
			}
			else if(gc_FMMenuItem==3)	// Order Record Setting
			{
				gb_LCM_InverseDisp=1;
				if(gc_OrderRecordTimerStep==0)	// On-OFF
				{
					gb_OrderRecord=!gb_OrderRecord;
					LCM_ShowRecONOFFString();
				}
				else if(gc_OrderRecordTimerStep==1)	// -1H
				{
					if(gdw_OrderRecordTime<3600)
					{
						gdw_OrderRecordTime+=86400-3600;
					}
					else
					{
						gdw_OrderRecordTime-=3600;
					}
					LCM_ShowRecordStartTime_Hour();
				}
				else if(gc_OrderRecordTimerStep==2)	// +1M
				{
					if(gdw_OrderRecordTime%3600<60)
					{
						gdw_OrderRecordTime+=3600-60;
					}
					else
					{
						gdw_OrderRecordTime-=60;
					}
					LCM_ShowRecordStartTime_Minute();
				}
				else
				{
					if(gw_OrderRecDuration==600)	
					{
						gw_OrderRecDuration=7200;
					}
					else
					{
						gw_OrderRecDuration-=600;
					}
					LCM_ShowDuration();
				}
				gb_LCM_InverseDisp=0;
			}
			else
			{
				// Change to Prev Channel Memory
				if(gc_FM_CHAmount>0)
				{
					if(gc_FM_CH_COUNT==0)
					{
						gc_FM_CH_COUNT=gc_FM_CHAmount-1;
					}else
					{
						gc_FM_CH_COUNT--;
					}
					gw_FM_frequency=gw_FM_CH[gc_FM_CH_COUNT];//gw_FM_CH最大可以保存50个电台
				}else
				{
					if(gw_FM_frequency<=875)
					{
						gw_FM_frequency=1080;
					}else
					{
						gw_FM_frequency	--;
					}
				}
				FM_FMTune();
				 //  LCM_ShowFrequency_L(gw_FM_frequency);

			}
		break;

		case 0x05:	// V
			if(gc_FMMenuItem==0&&gb_SetVol==0)
			{
				gb_SetVol=1;
				gc_MenuTimer=6;
				LCM_Clear_L2_L5();
				LCM_ShowVOL();
				ClearIRNumberVariable();
			}
			else
			{
				gc_MenuTimer=0;
			}
		break;

		case 0x06:	// M
		gb_SetVol=0;
			gc_MenuTimer=6;
			if(gc_FMMenuItem==0)	// FM menu
			{
				gc_FMMenuItem=1;
				if(gc_FMMenuNum<4)
				{
					LCM_ShowFMMenu(0);
					LCM_ShowFMMenu(1);
					LCM_ShowFMMenu(2);
					LCM_ShowFMMenu(3);
				}
				else
				{
					LCM_ShowFMMenu(4);
					LCM_ShowFMMenu(5);
					LCM_ShowFMMenu(6);
					LCM_erase_one_page(6);
					LCM_erase_one_page(7);
				}
			}
			else if(gc_FMMenuItem==1)	
			{
				switch(gc_FMMenuNum)
				{
					case 0:	// Record
						if(gc_CurrentCard==9)
						{
							// Display No Media
							gc_MenuTimer=0;
						}
						else
						{
							set_clock_mode(CLOCK_MODE_DOS);
							gdw_FreeClusterNum[1]=0;
							REC_Start();
							gc_MenuTimer=0;
						}
					break;

					case 1:	// Save channel
						if(gc_FM_CHAmount<30)
						{
							gc_FM_CH_COUNT=gc_FM_CHAmount;
							gw_FM_CH[gc_FM_CHAmount]=gw_FM_frequency; 
							gc_FM_CHAmount++;
						}
						gc_MenuTimer=0;
						USER_LogFile_ReadWrite(1);
					break;

					case 2:	// Delete channel
						if(gc_FM_CHAmount!=0)
						{
							gc_FM_CHAmount--;
							for(i=gc_FM_CH_COUNT;i<gc_FM_CHAmount;i++)
							{
								gw_FM_CH[i]=gw_FM_CH[i+1];
							}
						}
						USER_LogFile_ReadWrite(1);
					break;

					case 3:	// Delete all
						gc_FM_CHAmount=0;
						USER_LogFile_ReadWrite(1);
					break;

					case 4:	// Order record
						gc_FMMenuItem=3;
						LCM_Clear_L0_L7();
						gb_OrderRecord=gb_OrderRecordConfirm;
						gb_LCM_InverseDisp=0;
						gc_OrderRecordTimerStep=0;
						LCM_ShowRecordString();
						LCM_ShowStartTimeString();
						LCM_ShowDurationString();
						LCM_ShowRecordStartTime_Hour();
						LCM_ShowChar8x16(10,87);	// :
						LCM_ShowRecordStartTime_Minute();
						LCM_ShowDuration();
						gb_LCM_InverseDisp=1;
						LCM_ShowRecONOFFString();
						gb_LCM_InverseDisp=0;
					break;

					case 5:	// Quality
						LCM_ShowFMMenu_Quality(0);
						LCM_ShowFMMenu_Quality(1);
						LCM_ShowFMMenu_Quality(2);
						gc_FMMenuItem=2;
					break;

					case 6:	// Exit
						gc_MenuTimer=0;
					break;
				}
			}
			else if(gc_FMMenuItem==2)	// Record Quality
			{
				gc_MenuTimer=0;
			}
			else if(gc_FMMenuItem==3)	// Order Record Setting
			{
				gb_LCM_InverseDisp=0;
				if(gc_OrderRecordTimerStep==0)
				{
					if(gb_OrderRecord==0)
					{
						gb_OrderRecordConfirm=0;
						gc_MenuTimer=0;
					}
					else
					{
						LCM_ShowRecONOFFString();
					}
				}
				else if(gc_OrderRecordTimerStep==1)
				{		
					LCM_ShowRecordStartTime_Hour();
				}
				else if(gc_OrderRecordTimerStep==2)
				{
					LCM_ShowRecordStartTime_Minute();
				}
				else
				{
					gc_MenuTimer=0;
					gb_OrderRecordConfirm=1;
				}
				gc_OrderRecordTimerStep++;
				gb_LCM_InverseDisp=1;
				if(gc_OrderRecordTimerStep==1)
				{
					LCM_ShowRecordStartTime_Hour();
				}
				else if(gc_OrderRecordTimerStep==2)
				{
					LCM_ShowRecordStartTime_Minute();
				}
				else
				{
					LCM_ShowDuration();
				}
				gb_LCM_InverseDisp=0;
			}
		break;

		case 0x12:	// L-Play
			FM_Auto_SearchAllFreq(875);
			USER_LogFile_ReadWrite(1);//save all searched ch
		break;

		case 0x13:	// L-Next
			if(gb_SetVol==1)
			{
				gb_ADJ=0;	// Volume Adjust
				Linein_VolAdj();
				gc_LongKeyCount=30;
			}
			else if(gc_FMMenuItem==0)
			{
				gb_ADJ=0;
				FM_FMSeekFromCurrentCh(1);
			}
		break;

		case 0x14:	// L-Prev
			if(gb_SetVol==1)
			{
				gb_ADJ=1;	// Volume Adjust
				Linein_VolAdj();
				gc_LongKeyCount=30;
			}
			else if(gc_FMMenuItem==0)
			{
				gb_ADJ=1;
				FM_FMSeekFromCurrentCh(0);
			}
		break;

		case 0x15:	// L-V
			if(gc_CurrentCard==2)
			{
				if(gb_Host_Exist==1)
				{
					gb_Host_Exist=0;
				}
			}
			else if(gc_CurrentCard==5)
			{
				if(gb_SD_Exist==1)
				{
					gb_SD_Exist=0;
				}
			}
		break;

		case 0x16:	// L-M
			gc_Task_Current=C_Task_Idle;
		break;
	}
}


void AccessKeyEvent_FM(void)
{
	switch(gs_System_State.c_Phase)
	{
		case TASK_PHASE_STOP:
			AccessKeyEvent_FM_STOP();
		break;

		case TASK_PHASE_PAUSE:
			AccessKeyEvent_FM_PAUSE();
		break;

		case TASK_PHASE_REC:
			AccessKeyEvent_FM_REC();
		break;
	}
	gc_KeyEvent=0;
}

void Linein_VolAdj()
{
	xdata	U8	tc_TmpData;
	
	if(gb_ADJ==0)
	{
		if(gs_DSP_GLOBAL_RAM.sc_Volume==0)
		{
			XBYTE[0xB0C6]=0;   //(Jimi 091030)Line in gain L maximum
			XBYTE[0xB0C7]=0;   //(Jimi 091030)Line in gain R maximum
		}

		if(gs_DSP_GLOBAL_RAM.sc_Volume<42)
		{
			gs_DSP_GLOBAL_RAM.sc_Volume+=2;
		}

		tc_TmpData=31-(gs_DSP_GLOBAL_RAM.sc_Volume/2);
		XBYTE[0xB0C4]=tc_TmpData;  //(Jimi 091030)Headphone gain Lch
		XBYTE[0xB0C5]=tc_TmpData;  //(Jimi 091030)Headphone gain Rch
		LCM_ShowVOL();
	}
	else
	{
		if(gs_DSP_GLOBAL_RAM.sc_Volume>1)
		{
			gs_DSP_GLOBAL_RAM.sc_Volume-=2;
		}

		tc_TmpData=31-(gs_DSP_GLOBAL_RAM.sc_Volume/2);
		XBYTE[0xB0C4]=tc_TmpData;  //(Jimi 091030)Headphone gain Lch
		XBYTE[0xB0C5]=tc_TmpData;  //(Jimi 091030)Headphone gain Rch
		if(gs_DSP_GLOBAL_RAM.sc_Volume==0)
		{
			XBYTE[0xB0C6]=31;
			XBYTE[0xB0C7]=31;
		}
		LCM_ShowVOL();
	}
}

void Linein_Out(void)
{
	DSP_Write_MMR16(0x3FC1,(DSP_Read_MMR16(0x3FC1)|0x0010));
    XBYTE[0xB0C2]=0;       //disable ADC LRch(bit[1:0]), Mic bias(bit[2]) and Mic boost(bit[5:4])
    XBYTE[0xB0C1]&=0xFC;   //set audio driver post-mux as DAC output
    XBYTE[0xB0C1]|=0x30;   //power down ADC input pre-mux
    XBYTE[0xB0ED]|=0x02;   //Audio DA zero cross reset bypass on
    XBYTE[0xB0C4]=0;       //LCH speaker volume(i.e LCH OP Gain)
    XBYTE[0xB0C5]=0;       //RCH speaker volume(i.e RCH OP Gain)
}

void Linein_Set(void)
{
    data	U8 tc_TmpData;
    
	XBYTE[0xB010]|=0x01;
	
	XBYTE[0xB0C4]=0x1F;          //(Jimi 091030)mute headphone Lch
	XBYTE[0xB0C5]=0x1F;          //(Jimi 091030)mute headphone Rch
	XBYTE[0xB0C2]|=0x03;         //(Jimi 091030)ADC enable
	DSP_Write_MMR16(0x3FC0,(DSP_Read_MMR16(0x3FC0)|0x0200));    //audio ADC clock enable
	
	XBYTE[0xB0C1]&=0xCC;       //(Jimi 091030)
	XBYTE[0xB0C1]|=0x11;       //(Jimi 091030)set pre-mux and post-mux as line-in
	XBYTE[0xB0ED]|=0x01;       //(Jimi 091030)disable ADC zero cross detect
	
	if(gb_linein_mute==0)
	{
		tc_TmpData=31-(gs_DSP_GLOBAL_RAM.sc_Volume/2);
		XBYTE[0xB0C4]=tc_TmpData;  //(Jimi 091030)Headphone gain Lch
		XBYTE[0xB0C5]=tc_TmpData;  //(Jimi 091030)Headphone gain Rch
		XBYTE[0xB0C6]=0;   //(Jimi 091030)Line in gain Lch maximum
		XBYTE[0xB0C7]=0;   //(Jimi 091030)Line in gain Rch maximum
	}
	else
	{
		XBYTE[0xB0C6]=31;    //(Jimi 091030)Line in gain Lch mute
		XBYTE[0xB0C7]=31;    //(Jimi 091030)Line in gain Rch mute
	}
}

void DetectSDorHOSTState(void)
{
	data	U8	tc_temp;

	tc_temp=gc_CurrentCard;
	if(gb_SD_Exist==1)
	{
		if(gb_SD_pin==1)	// SD remove
		{
			gb_SD_Exist=0;
			if(gc_CurrentCard==2)
			{
				if(gb_Host_Exist==1)
				{
					if(Host_Initial()==0)
					{
						if(DOS_Initialize()==0)
						{
							gc_CurrentCard=5;
							LCM_ShowSDHostIcon();
							return;
						}
					}
				}
				gc_CurrentCard=9;
				LCM_ShowSDHostIcon();
				return;
			}
		}
	}
	else
	{
		if(gb_SD_pin==0)
		{
			gb_SD_Exist=1;
			if(SD_Identification_Flow()==1)
			{
				gc_CurrentCard=2;
				if(DOS_Initialize()==0)
				{
					LCM_ShowSDHostIcon();
					return;
				}
			}
		}
	}

	if(gb_Host_Exist==1)
	{
		if(Host_DetectDevice()==1)	// Host remove
		{
			gb_Host_Exist=0;
			if(gc_CurrentCard==5)
			{
				gc_CurrentCard=2;
				if(gb_SD_Exist==1)
				{
					if(SD_Identification_Flow()==1)
					{
						if(DOS_Initialize()==0)
						{
							LCM_ShowSDHostIcon();
							return;
						}
					}
				}
				gc_CurrentCard=9;
				LCM_ShowSDHostIcon();
				return;
			}
		}
	}
	else
	{
		if(Host_DetectDevice()==0)
		{
			gc_CurrentCard=5;
			gb_Host_Exist=1;
			if(Host_Initial()==0)
			{
				if(DOS_Initialize()==0)
				{
					LCM_ShowSDHostIcon();
					return;
				}
			}
		}
	}
	gc_CurrentCard=tc_temp;
}

void GetTotalFreeCluster(void)
{
	U32 tdw_temp;

	if(gc_DOS_FileSystemType==2)	// FAT32
	{
		gb_ReadWriteDataArea=1;
		DOS_Read_LogicSector(gdw_DOS_PBRAddr+1);
		((U8 *)(&gdw_TotalFreeClusNumber))[0]=gc_UserDataBuf[491];
		((U8 *)(&gdw_TotalFreeClusNumber))[1]=gc_UserDataBuf[490];
		((U8 *)(&gdw_TotalFreeClusNumber))[2]=gc_UserDataBuf[489];
		((U8 *)(&gdw_TotalFreeClusNumber))[3]=gc_UserDataBuf[488];
		gw_ClusBufValidSize=0;
		((U8 *)(&gdw_FreeClusterNum[0]))[0]=gc_UserDataBuf[495];
		((U8 *)(&gdw_FreeClusterNum[0]))[1]=gc_UserDataBuf[494];
		((U8 *)(&gdw_FreeClusterNum[0]))[2]=gc_UserDataBuf[493];
		((U8 *)(&gdw_FreeClusterNum[0]))[3]=gc_UserDataBuf[492];

		if(gdw_TotalFreeClusNumber==0xFFFFFFFF)
		{
			tdw_temp=gdw_FreeClusterNum[0] ; 
			gdw_FreeClusterNum[0] =0xFFFFFFF; 
			gdw_FreeClusterNum[1] =2; 
			while(gdw_FreeClusterNum[1]<(gdw_DOS_FatMaxCluster-1)) 
			{ 
			        DOS_SearchFreeCluster(1); 
			} 
			gdw_FreeClusterNum[0] =tdw_temp;
		}

		if(gdw_FreeClusterNum[0]>0x0FFFFFFF)
		{
			gdw_FreeClusterNum[0]=gdw_DOS_RootDirClus;
		}
		gdw_FreeClusterNum[0]--;
		while(gw_ClusBufValidSize<16&&gdw_FreeClusterNum[0]<(gdw_DOS_FatMaxCluster-1))
		{
			DOS_SearchFreeCluster(0);
		}
	}
	else
	{
		while(gdw_FreeClusterNum[1]<(gdw_DOS_FatMaxCluster-1)) 
		{         
			DOS_SearchFreeCluster(1);
		} 
		gdw_TotalFreeClusNumber+=2;
	}
#if 0
	dbprintf("gdw_ClusterBuffer[0]=%lx\n",gdw_ClusterBuffer[0]);
	dbprintf("gdw_ClusterBuffer[1]=%lx\n",gdw_ClusterBuffer[1]);
	dbprintf("gdw_ClusterBuffer[2]=%lx\n",gdw_ClusterBuffer[2]);
	dbprintf("gdw_ClusterBuffer[3]=%lx\n",gdw_ClusterBuffer[3]);
	dbprintf("gdw_TotalFreeClusNumber=%lx\n",gdw_TotalFreeClusNumber);	
#endif
}

void REC_Start(void)
{
	data	bit	tb_FindFlag;
	xdata	U8	tc_Loop;

	XBYTE[0xB0ED]|=0x01;	// Audio AD zero cross reset bypass on
	XBYTE[0xB08A]&=0xFE;	//(JC)Audio PLL on
	XBYTE[0xB046]&=~0x04;

	// Count Free Cluster & Search 16-Free Cluster
	GetTotalFreeCluster();
	if(gb_OrderRecordTrigger==0)
	{
		if(gs_System_State.w_SampleRate==32)
		{
			gdw_RecRemaintime=(U32)((double)gdw_TotalFreeClusNumber*gc_DOS_SectorPerCluster*505/1000/gs_System_State.w_SampleRate);			
		}
		else
		{
			gdw_RecRemaintime=(U32)((double)gdw_TotalFreeClusNumber*gc_DOS_SectorPerCluster*505*2/1000/gs_System_State.w_SampleRate);
		}
	}
	else
	{
		gdw_RecRemaintime=gw_OrderRecDuration;
		if(gb_Suspend_OrderRecord==0)
		{
			gdw_RecRemaintime++;
		}
	}
	
	LCM_Clear_L0_L7();
	tb_FindFlag=gb_FindFlag;
	gb_FindFlag=0;
	gs_System_State.c_FileHandle=1;
	gw_FileIndex[1]=1;
	if(SearchFolder_VOICE())	// Change to "VOICE" folder
	{
		RTC_Calendar();
		gw_DOS_CreateDate=(U16)(gc_Year+30)*0x0200+(U16)gc_Month*0x0020+gc_Day;
		gw_DOS_CreateTime=(U16)gc_Hour*0x0800+(U16)gc_Min*0x0020+gc_Sec;
		if(CreateFolder_VOICE()==1)
		{   
			// Create VOICE folder fail
			return;
		}
		SearchFolder_VOICE();	// Change to "VOICE" folder
	}

	gs_System_State.c_Phase=TASK_PHASE_REC;
	if(DOS_Search_File(C_File_OneDir|C_By_Name,C_RecordFileType,C_CmpExtName|C_Prev))//get max index number;
	{
		gdw_REC_IndexNum=0x30303030;
	}
	else
	{
		gdw_REC_IndexNum=gdw_CurrFlag;
	}	
	gb_FindFlag=tb_FindFlag;

	set_clock_mode(CLOCK_MODE_DOS);
	if(!DOS_Open_File_w())
	{//disk is not full,can do record
		//calculate REC Index		 
		((U8 *)(&gdw_REC_IndexNum))[3]++;
		for(tc_Loop=3;tc_Loop>0;tc_Loop--)
		{
			if(((U8 *)(&gdw_REC_IndexNum))[tc_Loop]==0x3a)
			{
				((U8 *)(&gdw_REC_IndexNum))[tc_Loop]='0';
				((U8 *)(&gdw_REC_IndexNum))[tc_Loop-1]++;
			}
			else
			{
				break;
			}
		}

		gc_FileLongName[0x00]=0x00;
		gc_FileLongName[0x01]=0x00;	
		gc_FileLongName[0x02]=0x01;
		gc_FileLongName[0x03]=0x00;
		gc_FileLongName[0x04]=0x0C;
		gc_FileLongName[0x05]='R';
		gc_FileLongName[0x06]='E';
		gc_FileLongName[0x07]='C';
		gc_FileLongName[0x08]='_';
		gc_FileLongName[0x09]=((U8 *)(&gdw_REC_IndexNum))[0];
		gc_FileLongName[0x0A]=((U8 *)(&gdw_REC_IndexNum))[1];
		gc_FileLongName[0x0B]=((U8 *)(&gdw_REC_IndexNum))[2];
		gc_FileLongName[0x0C]=((U8 *)(&gdw_REC_IndexNum))[3];
		gc_FileLongName[0x0D]='.';
		gc_FileLongName[0x0E]='W';
		gc_FileLongName[0x0F]='A';
		gc_FileLongName[0x10]='V';
		
		gdw_CurrFlag=gdw_REC_IndexNum;
		gdw_StartCluster2=gs_File_FCB[gs_System_State.c_FileHandle].dw_File_StartCluster;
		gs_File_FCB[gs_System_State.c_FileHandle].c_FileType=AUDIO_FILE_TYPE_WAV;
		set_clock_mode(CLOCK_MODE_REC);
		IMA_ADPCM_Rec_Download();
		if(gs_System_State.w_SampleRate==32)
		{
			IMA_ADPCM_Rec_PlayCmd(gs_System_State.w_SampleRate,2);	// sample rate,channel(Stereo)			
		}
		else
		{
			IMA_ADPCM_Rec_PlayCmd(gs_System_State.w_SampleRate,1);	// sample rate,channel(Mono)			
		}
		gs_System_State.c_Phase=TASK_PHASE_REC;
		gdw_FATBufferSectorNum=0xFFFFFFFF;
	}
}

void REC_Stop(void)
{
	xdata	U8	i;
	xdata	U32	tdw_reservedSectors;
	xdata	U32	tdw_FreeMemorySize;
	xdata	U32	tdw_AudioSize;
	xdata	U32	tdw_DATA_LogAddr;
	
	if(gs_System_State.c_Phase!=TASK_PHASE_STOP)//(JC)prevent from repeating stop
	{
		gb_OrderRecordTrigger=0;
		IMA_ADPCM_Rec_StopProcess();//(JC)write remain data

		gc_FileLongName[0x0D]='W';
		gc_FileLongName[0x0E]='A';
		gc_FileLongName[0x0F]='V';
		RTC_Calendar();
		DOS_Close_File_w(&gc_FileLongName+5);
		tdw_DATA_LogAddr=DOS_ClusterLogicAddr(gs_File_FCB[1].dw_File_StartCluster);
		gb_ReadWriteDataArea=0;
		DOS_Read_LogicSector(tdw_DATA_LogAddr);

		tdw_AudioSize=gs_File_FCB[1].dw_File_TotalSize-60;
		gc_PlayRecordDataBuf[0x38]=*((U8 *)&tdw_AudioSize+3);
		gc_PlayRecordDataBuf[0x39]=*((U8 *)&tdw_AudioSize+2);
		gc_PlayRecordDataBuf[0x3A]=*((U8 *)&tdw_AudioSize+1);
		gc_PlayRecordDataBuf[0x3B]=*((U8 *)&tdw_AudioSize+0);

		tdw_AudioSize+=0x34;
		gc_PlayRecordDataBuf[0x04]=*((U8 *)&tdw_AudioSize+3);
		gc_PlayRecordDataBuf[0x05]=*((U8 *)&tdw_AudioSize+2);
		gc_PlayRecordDataBuf[0x06]=*((U8 *)&tdw_AudioSize+1);
		gc_PlayRecordDataBuf[0x07]=*((U8 *)&tdw_AudioSize+0);

		// sample rate is 8000,	when block is 256,then 505,	when block is 512 then 1017
		tdw_AudioSize=505*(gs_File_FCB[1].dw_File_TotalSize-60)/(DSP_ADPCM_OUT_SZ_PER_CH*2);
		gc_PlayRecordDataBuf[0x30]=*((U8 *)&tdw_AudioSize+3);
		gc_PlayRecordDataBuf[0x31]=*((U8 *)&tdw_AudioSize+2);
		gc_PlayRecordDataBuf[0x32]=*((U8 *)&tdw_AudioSize+1);
		gc_PlayRecordDataBuf[0x33]=*((U8 *)&tdw_AudioSize+0);
		gb_ReadWriteDataArea=0;
		DOS_Write_LogicSector(tdw_DATA_LogAddr,3);
		//------+ sq added for refreshing the next file name after saving a file in Rec UI @080924 ------
		gdw_CurrFlag=gdw_REC_IndexNum;
		gdw_NextRecordNumber=gdw_CurrFlag;

		((U8 *)(&gdw_NextRecordNumber))[3]++;
		for(i=3;i>0;i--)
		{
			if(((U8 *)(&gdw_NextRecordNumber))[i]==0x3A)
			{
				((U8 *)(&gdw_NextRecordNumber))[i]='0';
				((U8 *)(&gdw_NextRecordNumber))[i-1]++;
			}
			else
			{
				break;
			}
		}
		gc_FileLongName[0x09]=((U8 *)(&gdw_NextRecordNumber))[0];
		gc_FileLongName[0x0A]=((U8 *)(&gdw_NextRecordNumber))[1];
		gc_FileLongName[0x0B]=((U8 *)(&gdw_NextRecordNumber))[2];
		gc_FileLongName[0x0C]=((U8 *)(&gdw_NextRecordNumber))[3];
	}
	gc_FileLongName[0x0D]='.';
	gc_FileLongName[0x0E]='W';
	gc_FileLongName[0x0F]='A';
	gc_FileLongName[0x10]='V';
	IMA_ADPCM_Rec_StopCmd();
	gs_System_State.c_Phase=TASK_PHASE_STOP;

	// Write Total Free Cluster & Last Used Cluster Number into FAT32 FSInfo Sector
	if(gc_DOS_FileSystemType==2)	// FAT32
	{
		gb_ReadWriteDataArea=1;
		DOS_Read_LogicSector(gdw_DOS_PBRAddr+1);
		gc_UserDataBuf[491]=((U8 *)(&gdw_TotalFreeClusNumber))[0];
		gc_UserDataBuf[490]=((U8 *)(&gdw_TotalFreeClusNumber))[1];
		gc_UserDataBuf[489]=((U8 *)(&gdw_TotalFreeClusNumber))[2];
		gc_UserDataBuf[488]=((U8 *)(&gdw_TotalFreeClusNumber))[3];
		gc_UserDataBuf[495]=((U8 *)(&gs_File_FCB[1].dw_File_CurrentCluster))[0];
		gc_UserDataBuf[494]=((U8 *)(&gs_File_FCB[1].dw_File_CurrentCluster))[1];
		gc_UserDataBuf[493]=((U8 *)(&gs_File_FCB[1].dw_File_CurrentCluster))[2];
		gc_UserDataBuf[492]=((U8 *)(&gs_File_FCB[1].dw_File_CurrentCluster))[3];
		DOS_Write_LogicSector(gdw_DOS_PBRAddr+1,3);
	}

	LCM_Clear_L2_L7();
	LCM_ShowFrequency_L();
	LCM_ShowFMOrderRecordIcon();
	tdw_reservedSectors=(U32)(gc_DOS_SectorPerCluster<<2)+36;
	gdw_RecSpareTime=(((tdw_FreeMemorySize-tdw_reservedSectors)*505)/4000)/2;//(JC)H1118 to match with the DSP time calculation, //odin20081127updated

	if((tdw_FreeMemorySize<=tdw_reservedSectors)||(gdw_RecSpareTime==0)) 
	{
		return;
	}
	else
	{
		tdw_FreeMemorySize-=((U32)(gc_DOS_SectorPerCluster<<2)+36);//(JC)H1118 max 36 sectors of bitstream buf for writing remain data
		gdw_RecSpareTime=((tdw_FreeMemorySize*505)/4000)/2;//(JC)H1119 to match w/ the DSP
	}
}

U8 IMA_ADPCM_Rec_StopProcess(void)
{
	data	U8	tc_SectorNum;

	while(1)
	{
		if(!(L2_DSP_Read_DMem16(DSP_RemainBuffer)<512))
		{
			L2_DSP_DM_MCU_DMA(gs_DSP_GLOBAL_RAM.sc_DM_Index,(U16)gc_PlayRecordDataBuf);  // 2^9
			if(L2_DSP_SendCommandSet(DCMD_DatOut) != DCMD_DatOut)
			{
				return DSP_DATAOUT_COMMAND_ERROR;
			}

			gs_DSP_GLOBAL_RAM.sc_DM_Index++;
			if(gs_DSP_GLOBAL_RAM.sc_DM_Index>=24)
			{
				gs_DSP_GLOBAL_RAM.sc_DM_Index=0;
			}

			tc_SectorNum=gs_File_FCB[C_RecordFHandle].dw_File_DataPoint>>9;
			if(gc_DOS_SectorPerCluster==1)
			{   
				gdw_TotalFreeClusNumber--;
				DOS_Write_File(3);
			}
			else if(tc_SectorNum==0||tc_SectorNum==gc_DOS_SectorPerCluster)
			{
				gdw_TotalFreeClusNumber--;
				DOS_Write_File(0);
			}
			else if(tc_SectorNum==gc_DOS_SectorPerCluster-1)
			{
				DOS_Write_File(2);
			}
			else
			{
				DOS_Write_File(1);
			}
		}
		else
		{
			break;
		}
	}
	if(gc_CurrentCard==5)
	{
		tc_SectorNum=gs_File_FCB[C_RecordFHandle].dw_File_DataPoint>>9;
		if(tc_SectorNum!=gc_DOS_SectorPerCluster)
		{
			memset(gc_PlayRecordDataBuf,0,512);
			while(tc_SectorNum!=gc_DOS_SectorPerCluster)
			{
				if(tc_SectorNum==gc_DOS_SectorPerCluster-1)
				{
					DOS_Write_File(2);
				}
				else
				{
					DOS_Write_File(1);
				}
				tc_SectorNum++;
			}
		}
	}
	return 0;
}

U8 IMA_ADPCM_Rec_DataOut(void)
{
	data	U8	tc_SectorNum;

	while(L2_DSP_Read_DMem16(DSP_RemainBuffer)>768)	//Jimi, cuz DSP use lower 16-bit of DM to put out data.
	{
		XBYTE[0xB390]=0x91;	// Fragment enable
		L2_DSP_DM_MCU_DMA(gs_DSP_GLOBAL_RAM.sc_DM_Index,(U16)gc_RecordDataBuf);	// 2^9=512
		XBYTE[0xB390]=0x00;	// Fragment disable
		// gc_RecordDataBuf[] DMA to gc_PlayRecordDataBuf[]
		XBYTE[0xB301]=0x00;
		XBYTE[0xB112]=0x00;
		XBYTE[0xB113]=gc_RecordDataBufHB;
		XBYTE[0xB114]=0x00;
		XBYTE[0xB115]=gc_PlayRecordDataBufHB;
		XBYTE[0xB3B0]=0x01;
		while(!(XBYTE[0xB3C0]&0x01));
		XBYTE[0xB3C0]=0x00;

		if(L2_DSP_SendCommandSet(DCMD_DatOut)!=DCMD_DatOut)
		{
			return DSP_DATAOUT_COMMAND_ERROR;
		}

		gs_DSP_GLOBAL_RAM.sc_DM_Index++;
		if(gs_DSP_GLOBAL_RAM.sc_DM_Index>=24)
		{
			gs_DSP_GLOBAL_RAM.sc_DM_Index=0;
		}

		tc_SectorNum=gs_File_FCB[1].dw_File_DataPoint>>9;
		if(gc_DOS_SectorPerCluster==1)
		{   
			gdw_TotalFreeClusNumber--;
			DOS_Write_File(3);			  //write one sector
		}
		else if(tc_SectorNum==0||tc_SectorNum==gc_DOS_SectorPerCluster)
		{
			gdw_TotalFreeClusNumber--;
			DOS_Write_File(0);
		}
		else if(tc_SectorNum==gc_DOS_SectorPerCluster-1)
		{
			DOS_Write_File(2);
		}
		else
		{
			DOS_Write_File(1);
		}
	}

	if(gb_RTC_wakeup==1)
	{   
		gb_RTC_wakeup=0;
		if(gw_Disp_CurrentSec!=IMA_ADPCM_Rec_Time())
		{
			gw_Disp_CurrentSec=IMA_ADPCM_Rec_Time();
			LCM_ShowRECTime(gw_Disp_CurrentSec);
			gdw_RecRemaintime--;
			LCM_ShowRECRemainTime(gdw_RecRemaintime);
		}
	}
	return 0;
}

U8 IMA_ADPCM_Rec_StopCmd(void)
{
	data	U8  tc_Ret;
	data	U16 tw_temp;
	
	tc_Ret=DSP_StopCmd();
	
	tw_temp=DSP_Read_MMR16(0x3fc0);
	DSP_Write_MMR16(0x3fc0,tw_temp&~0x0200);			// turn off ADC clock for power saving
	return tc_Ret;
}

U16  IMA_ADPCM_Rec_Time(void)
{
	xdata	U32   FrameNum;
	data	U32   tdwTmpSec;
	
	FrameNum=L2_DSP_Read_DMem24(DSP_DecodeFrameCounter)*505/1000;
	tdwTmpSec=FrameNum/gs_System_State.w_SampleRate;
	return ((U16)tdwTmpSec);
}


void L2_DSP_DM_MCU_DMA(U16 BufferIndex,U16 tw_Src)
{
	data	U8  tc_DSPAddr_High;

	// ----- DMA Reset -----
	XBYTE[0xB304]=0x09;
	XBYTE[0xB304]=0x08;
	XBYTE[0xB3C0]=0x00;		// clear DMA complete
	XBYTE[0xB330]=0x01;		// clear checksum			
	// ----- DMA initial settings for DM transfer -----		
	XBYTE[0xB216]=0x00;	// Remain DSP mode.  When you do MCU -> DM, MCU will stop DSP automaticlly
	// Define data size in DMA		
	XBYTE[0xB301]=0x05;	// DM -> SRAM
	XBYTE[0xB302]=0xFF;	// Size=512
	XBYTE[0xB303]=0x01;

	XBYTE[0xB340]=0x01;	// 16-bit DMA
	XBYTE[0xB21D]=0x01;
	tc_DSPAddr_High=BufferIndex;
	tc_DSPAddr_High+=0x20;	// bitstream start address : 0x2000
	XBYTE[0xB217]=(U8)(BufferIndex<<8);
	XBYTE[0xB218]=tc_DSPAddr_High;

	// Source
	XBYTE[0xB114]=(U8)(tw_Src);
	XBYTE[0xB115]=(U8)((tw_Src)>>8);
	XBYTE[0xB3B0]=0x01;	// DMA1 start
	while(!(XBYTE[0xB3C0]&0x01));
	XBYTE[0xB3C0]=0x00;
}

void LCM_ShowRECFilename(void)
{
	gc_LCM_line=6;
	LCM_ShowChar8x16(30,0);		// R
	LCM_ShowChar8x16(17,8);		// E
	LCM_ShowChar8x16(15,16);	// C
	LCM_ShowChar8x16(11,24);	// _
	LCM_ShowChar8x16(gc_FileLongName[ 9]-0x30,32);
	LCM_ShowChar8x16(gc_FileLongName[10]-0x30,40);
	LCM_ShowChar8x16(gc_FileLongName[11]-0x30,48);
	LCM_ShowChar8x16(gc_FileLongName[12]-0x30,56);
	LCM_ShowChar8x16(65,64);	// .
	LCM_ShowChar8x16(35,72);	// W
	LCM_ShowChar8x16(13,80);	// A
	LCM_ShowChar8x16(34,88);	// V
}
