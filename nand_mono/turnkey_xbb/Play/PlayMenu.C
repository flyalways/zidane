#include "..\Header\SPDA2K.h"
#include "..\header\dos\fs_struct.h"
#include "..\header\Memalloc.h"
#include "PlayMenu.h" 
#include "..\IR\LCM_BMP.h" 
#include "..\IR\remote.h"  //20090206 chiayen add
#include "..\IR\FM.h"
#include "..\header\variables.h"

#if (LCM_SIZE==1)
#define C_LCM_ROW_MAX     2
#endif
#if (LCM_SIZE==0)||(LCM_SIZE==2)
#define C_LCM_ROW_MAX     4
#endif

xdata U8 gc_EQChangeFlag=0;
xdata U8 gc_LCMDispIndex=0;
xdata U8 gc_LCMDispTotalIconNum=0;
xdata U8 gc_RepPlayMode_Dir=2;
xdata U8 gc_RepPlayMode_Nor=2;
xdata U8 gc_TableOffset=0;
xdata U8 gc_PlayMenuLevel=0; //0:playmenu 1:pause menu 2:language menu 3:play mode menu 4:EQ menu 5:play all menu 6:play dir menu
xdata U8 gc_PlayMenuTotalNum=3;
xdata U8 gc_IRRepAllFlag=0;

extern xdata  U8 gc_PlayMenu_Timeout;  //20090206 chiayen add
extern data	 U8		gc_LCM_Media;
extern data	 U8	 	gc_LCM_line;
extern data	 U8		gc_DisplayEQIcon;
extern data	 U8		gc_DisplayPlayMode;
extern xdata U8		gc_PlayMenu_IR;  //20090206 chiayen add  
extern xdata	 U16	gw_DisplayBitRate;
extern xdata	 U16	gw_DispSongNum1;
extern xdata	 U16	gw_DispSongNum;

extern bit	gb_DirPlay_Flag;  
extern xdata U8 	gc_PlayModeChange_Flag;
extern xdata U8  	gc_DIRTimeOut_Flag;
extern xdata U8  	gc_CurrentHZK;
extern xdata UBYTE gc_PhaseInx;
extern xdata UBYTE gc_RepPlayMode;

void LCM_ShowChar8x16_Play(U8 Num,U8 tc_column,bit reverse);
void PlayMenuProcess();
void PlayMenuProcess_IR();
void PlayModeMenuProcess_IR();
void LanguageMenuProcess_IR();
void EQMenuProcess_IR();
void ir_commandservice_menu();
void IR_Service_Process_Menu();
extern void TimeOutHandle();

extern 	xdata U8 gc_irkey;
extern data	U16	gw_IR_Timer;
extern 	data 	U16   gw_FM_frequency;
void TimeOutHandle();
//extern xdata U8 gc_RPTMenuSet;


//code U8 LanguageTable[]={15,16,0,18,17,0,0,0,0,0,1,0,0,0,1,4,0};

extern void Menu_Disp_Item_Play(U8 tc_MenuType,U8 tc_Select,U8 tc_RefreshType);	  //tc_RefreshType always =1;

void PlayMenuProcess()
{
	U8 tc_MenuItem=0xFF;
	gc_PlayMenu_Timeout=160;
	gc_PlayMenuLevel=0;
	gc_PlayMenuTotalNum=3;
	gc_TableOffset=0;
	gc_PhaseInx = 0;
	if(gc_DIRTimeOut_Flag==0)
	{
		gc_LCMDispIndex=0;
	}
	else
	{
		gc_LCMDispIndex=1;
		gc_DIRTimeOut_Flag=0;
	}
	LCM_clear();
	Menu_Disp_Item_Play(gc_PlayMenuLevel,gc_LCMDispIndex,1);
    while(1)
    {
		if(gc_PhaseInx)
		{
	        switch(gc_PhaseInx)
	        {
	            case C_FmFreqChg:
					tc_MenuItem = 2;
	            break;
	
	            case C_PlayNext:
					if(gc_LCMDispIndex==2)
					{
						gc_LCMDispIndex=0;
					}
					else
					{
						gc_LCMDispIndex++;
					}
					Menu_Disp_Item_Play(gc_PlayMenuLevel,gc_LCMDispIndex,1); 
					gc_PlayMenu_Timeout=160; 
	            break;
	
	            case C_PlayPrev:
					if(gc_LCMDispIndex==0)
					{
						gc_LCMDispIndex=2;
					}
					else
					{
						gc_LCMDispIndex--;
					}
					Menu_Disp_Item_Play(gc_PlayMenuLevel,gc_LCMDispIndex,1);  ;
					gc_PlayMenu_Timeout=160;
	            break;	
	
				default:
					tc_MenuItem=0xFF;  //20090216 chiayen add
				break;
	        }
			if(tc_MenuItem==2) //enter child dir or confirm selected item
			{
				if(gc_LCMDispIndex==0)
				{
					gb_DirPlay_Flag=0; //20081224 chiayen test
					gc_PlayModeChange_Flag=1;
					gw_init_needed=0xFFFF;             
            		gb_FindFlag = 0;  //20090513 chiayen add for play all
					gs_File_FCB[0].dw_FDB_StartCluster = gdw_DOS_RootDirClus;           
					DOS_Search_File(C_File_All|C_Cnt_FileNo, C_MusicFileType, C_CmpExtName|C_Next);//(JC)count music file no. in root
					gc_Task_Next=C_Task_Play;
				}
				else if(gc_LCMDispIndex==1)
				{
					gc_Task_Next=C_Task_Dir;
				}
				else if(gc_LCMDispIndex==2)
				{
					gc_Task_Next=C_Task_Play;
				}
			}
	   	}
		else
		{
			if(gc_PlayMenu_Timeout==0)  //TimeOut
			{
				gc_DIRTimeOut_Flag=1;
				gb_FindFlag = 1;
				gc_Task_Next=C_Task_Play;
			}
		}

		gc_PhaseInx=0;		
		IR_Service_Process_Menu();
		Polling_TaskEvents();

       	if(gc_Task_Current!=gc_Task_Next)
        {
			LCM_clear();
			gc_LCM_Media=0xff;
			gc_DisplayEQIcon=0xFF;
			gw_DisplayBitRate=0xFFFF;
			gc_DisplayPlayMode=0xFF;
			gw_DispSongNum=0xFFFF;
			gw_DispFileName_ByteOffset=0;
			gc_Task_Current=gc_Task_Next;
            break;  
        }

	}	
}


void LanguageMenuProcess()
{
	U8 tc_MenuItem=0xFF;
	gc_TableOffset=0;
	gc_PhaseInx = 0;
	gc_PlayMenuLevel=2;
	gc_PlayMenuTotalNum=17;
	gc_LCMDispIndex=gc_language_offset;//sunzhk add
	LCM_clear();
	Menu_Disp_Item_Play(gc_PlayMenuLevel,gc_LCMDispIndex,1);
    while(1)
    {
		if(gc_PhaseInx)
		{
	        switch(gc_PhaseInx)
	        {
	            case C_FmFreqChg:
					tc_MenuItem = 2;
	            break;
	
	            case C_PlayNext:
					if(gc_LCMDispIndex==16)
					{
						gc_LCMDispIndex=0;
					}
					else
					{
						gc_LCMDispIndex++;
					}
					Menu_Disp_Item_Play(gc_PlayMenuLevel,gc_LCMDispIndex,1);
					gc_PlayMenu_Timeout=160;
	            break;
	
	            case C_PlayPrev:
					if(gc_LCMDispIndex==0)
					{
						gc_LCMDispIndex=16;
					}
					else
					{
						gc_LCMDispIndex--;
					}
					Menu_Disp_Item_Play(gc_PlayMenuLevel,gc_LCMDispIndex,1);
					gc_PlayMenu_Timeout=160;
	            break;
	
				default:		
					tc_MenuItem=0xFF;  //20090216 chiayen add	
				break;
	        }

			 if(tc_MenuItem==2) //enter child dir or confirm selected item
			{
				if(gc_LCMDispIndex<=16)
				{
					gc_language_offset=	gc_LCMDispIndex;
					gc_CurrentHZK=LanguageTable[gc_language_offset];
					gc_Task_Next=C_Task_Play;
					break;
				}
				else
				{
					gc_PlayMenuLevel=2;
					gc_PlayMenuTotalNum=4;
					gc_LCMDispIndex=0;

					gc_TableOffset=(gc_LCMDispIndex/C_LCM_ROW_MAX)*(C_LCM_ROW_MAX);
					gc_PhaseInx = 0;
                    
					LCM_clear();
					Menu_Disp_Item_Play(gc_PlayMenuLevel,gc_LCMDispIndex,1);

					break;
				}
			}
		}
		gc_PhaseInx=0;		
		Polling_TaskEvents();
		IR_Service_Process_Menu();//sunzhk add 100818
	}	
}


void EQMenuProcess()
{
	U8 tc_MenuItem=0xFF;
	gc_TableOffset=0;
	gc_PhaseInx = 0;
	gc_PlayMenuLevel=4;
	gc_PlayMenuTotalNum=7;
	gc_LCMDispIndex=0;
	LCM_clear();
	Menu_Disp_Item_Play(gc_PlayMenuLevel,gc_LCMDispIndex,1);
    while(1)
    {
		if(gc_PhaseInx)
		{
	        switch(gc_PhaseInx)
	        {
	            case C_FmFreqChg:
					tc_MenuItem = 2;
	            break;
	
	            case C_PlayNext:
					if(gc_LCMDispIndex==6)
					{
						gc_LCMDispIndex=0;
					}
					else
					{
						gc_LCMDispIndex++;
					}
					Menu_Disp_Item_Play(gc_PlayMenuLevel,gc_LCMDispIndex,1);
					gc_PlayMenu_Timeout=160;
	            break;
	
	            case C_PlayPrev:
					if(gc_LCMDispIndex==0)
					{
						gc_LCMDispIndex=6;
					}
					else
					{
						gc_LCMDispIndex--;
					}
					Menu_Disp_Item_Play(gc_PlayMenuLevel,gc_LCMDispIndex,1);
					gc_PlayMenu_Timeout=160;
	            break;
	
				default:		
					tc_MenuItem=0xFF;  //20090216 chiayen add	
				break;
	        }
		
			 if(tc_MenuItem==2) //enter child dir or confirm selected item
			{
				if(gc_LCMDispIndex<6)
				{
					gs_DSP_GLOBAL_RAM.sc_EQ_Type=gc_LCMDispIndex;
					gc_EQChangeFlag=1;
					gc_Task_Next=C_Task_Play;
					break;
				}
				else
				{
					gc_PlayMenuLevel=1;
					gc_PlayMenuTotalNum=4;
					gc_LCMDispIndex=1;


					gc_TableOffset=(gc_LCMDispIndex/C_LCM_ROW_MAX)*(C_LCM_ROW_MAX);
					gc_PhaseInx = 0;

					LCM_clear();
					Menu_Disp_Item_Play(gc_PlayMenuLevel,gc_LCMDispIndex,1);
					break;
				}
			}
		}
		gc_PhaseInx=0;		
		Polling_TaskEvents();
		IR_Service_Process_Menu();//sunzhk add 100818
	}	
}


void PlayALLMenuProcess()
{
	U8 tc_MenuItem=0xFF,tc_LCDMDispIndex;
	tc_LCDMDispIndex=gc_LCMDispIndex;
	gc_TableOffset=0;
	gc_PhaseInx = 0;
	gc_PlayMenuLevel=5;
	gc_PlayMenuTotalNum=4;
	gc_LCMDispIndex=0;
	LCM_clear();
	Menu_Disp_Item_Play(gc_PlayMenuLevel,gc_LCMDispIndex,1);
    while(1)
    {
		if(gc_PhaseInx)
		{
	        switch(gc_PhaseInx)
	        {
	            case C_FmFreqChg:
					tc_MenuItem = 2;
	            break;
	
	            case C_PlayNext:
					if(gc_LCMDispIndex==3)
					{
						gc_LCMDispIndex=0;
					}
					else
					{
						gc_LCMDispIndex++;
					}
					Menu_Disp_Item_Play(gc_PlayMenuLevel,gc_LCMDispIndex,1);
					gc_PlayMenu_Timeout=160;
	            break;
	
	            case C_PlayPrev:
					if(gc_LCMDispIndex==0)
					{
						gc_LCMDispIndex=3;
					}
					else
					{
						gc_LCMDispIndex--;
					}
					Menu_Disp_Item_Play(gc_PlayMenuLevel,gc_LCMDispIndex,1);
					gc_PlayMenu_Timeout=160;
	            break;
	
				default:		
					tc_MenuItem=0xFF;  //20090216 chiayen add	
				break;
	        }
		
			if(tc_MenuItem==2) //enter child dir or confirm selected item
			{
                                /*if(gc_LCMDispIndex==0) 
				{
					if((gb_DirPlay_Flag==0)&&(tc_LCDMDispIndex==0))
					{
						gc_RepPlayMode_Nor=2;
						gc_RepPlayMode=gc_RepPlayMode_Nor;
					}
					else if((gb_DirPlay_Flag==1)&&(tc_LCDMDispIndex==1))
					{
						gc_RepPlayMode_Dir=2;
						gc_RepPlayMode=gc_RepPlayMode_Dir;
					}
					else if((gb_DirPlay_Flag==1)&&(tc_LCDMDispIndex==0))
					{
						gc_RepPlayMode_Nor=2;
					}
					else if((gb_DirPlay_Flag==0)&&(tc_LCDMDispIndex==1))
					{
						gc_RepPlayMode_Dir=2;	
					}					
					gc_Task_Next=C_Task_Play;
					gw_DispSongNum=0xFFFF;
					break;
				}
				else if(gc_LCMDispIndex==1)
				{
					if((gb_DirPlay_Flag==0)&&(tc_LCDMDispIndex==0))
					{
						gc_RepPlayMode_Nor=3;
						gc_RepPlayMode=gc_RepPlayMode_Nor;
					}
					else if((gb_DirPlay_Flag==1)&&(tc_LCDMDispIndex==1))
					{
						gc_RepPlayMode_Dir=3;
						gc_RepPlayMode=gc_RepPlayMode_Dir;
					}
					else if((gb_DirPlay_Flag==1)&&(tc_LCDMDispIndex==0))
					{
						gc_RepPlayMode_Nor=3;
					}
					else if((gb_DirPlay_Flag==0)&&(tc_LCDMDispIndex==1))
					{
						gc_RepPlayMode_Dir=3;	
					}
					gc_Task_Next=C_Task_Play;
					gw_DispSongNum=0xFFFF;
					break;
				}
				else if(gc_LCMDispIndex==2)
				{
					if((gb_DirPlay_Flag==0)&&(tc_LCDMDispIndex==0))
					{
						gc_RepPlayMode_Nor=1;
						gc_RepPlayMode=gc_RepPlayMode_Nor;
					}
					else if((gb_DirPlay_Flag==1)&&(tc_LCDMDispIndex==1))
					{
						gc_RepPlayMode_Dir=1;
						gc_RepPlayMode=gc_RepPlayMode_Dir;
					}
					else if((gb_DirPlay_Flag==1)&&(tc_LCDMDispIndex==0))
					{
						gc_RepPlayMode_Nor=1;
					}
					else if((gb_DirPlay_Flag==0)&&(tc_LCDMDispIndex==1))
					{
						gc_RepPlayMode_Dir=1;	
					}
					gc_Task_Next=C_Task_Play;
					break;
                                }*/ 
                                if(gc_LCMDispIndex==0) 
                                {         
                                        gc_RepPlayMode_Nor=2; 
                                        gc_RepPlayMode=gc_RepPlayMode_Nor; 
                                                        
                                        gc_Task_Next=C_Task_Play; 
                                        break; 
                                } 
                                else if(gc_LCMDispIndex==1) 
                                { 
                                        gc_RepPlayMode_Nor=3; 
                                        gc_RepPlayMode=gc_RepPlayMode_Nor; 
                                
                                        gc_Task_Next=C_Task_Play; 
                                        break; 
                                } 
                                else if(gc_LCMDispIndex==2) 
                                {                 
                                        gc_RepPlayMode_Nor=1; 
                                        gc_RepPlayMode=gc_RepPlayMode_Nor; 

                                        gc_Task_Next=C_Task_Play; 
                                        break; 
				}
				else if(gc_LCMDispIndex==3)
				{
					gc_PlayMenuLevel=1;//3;
					gc_PlayMenuTotalNum=3;
					gc_LCMDispIndex=tc_LCDMDispIndex;

					gc_TableOffset=(gc_LCMDispIndex/C_LCM_ROW_MAX)*(C_LCM_ROW_MAX);
					gc_PhaseInx = 0;

					LCM_clear();
					Menu_Disp_Item_Play(gc_PlayMenuLevel,gc_LCMDispIndex,1);
					break;
				}
			}
		}
		gc_PhaseInx=0;		
		Polling_TaskEvents();
                IR_Service_Process_Menu();//sunzhk add 100818    
	}	
}

void PauseMenuProcess()
{
	U8 tc_MenuItem=0xFF;
	gc_PlayMenu_Timeout=160;
	gc_TableOffset=0;
	gc_PhaseInx = 0;
	gc_PlayMenuLevel=1;
	gc_PlayMenuTotalNum=4;
	gc_LCMDispIndex=0;
	LCM_clear();
	Menu_Disp_Item_Play(gc_PlayMenuLevel,gc_LCMDispIndex,1);

    while(1)
    {
		if(gc_PhaseInx)
		{
	        switch(gc_PhaseInx)
	        {
	            case C_FmFreqChg:
					tc_MenuItem = 2;
	            break;
	
	            case C_PlayNext:
					if(gc_LCMDispIndex==3)
					{
						gc_LCMDispIndex=0;
					}
					else
					{
						gc_LCMDispIndex++;
					}
					Menu_Disp_Item_Play(gc_PlayMenuLevel,gc_LCMDispIndex,1);
					gc_PlayMenu_Timeout=160;
	            break;
	
	            case C_PlayPrev:
					if(gc_LCMDispIndex==0)
					{
						gc_LCMDispIndex=3;
					}
					else
					{
						gc_LCMDispIndex--;
					}
					Menu_Disp_Item_Play(gc_PlayMenuLevel,gc_LCMDispIndex,1);   	
					gc_PlayMenu_Timeout=160;
	            break;
	
				default:
					tc_MenuItem=0xFF;  //20090216 chiayen add
				break;
	        }
		
			if(tc_MenuItem==2) //enter child dir or confirm selected item
			{
				if(gc_LCMDispIndex==0)
				{
					//PlayModeMenuProcess(); //sunzhk  remove
					PlayALLMenuProcess();
				}
				else if(gc_LCMDispIndex==1)
				{
						EQMenuProcess();
				}
				else if(gc_LCMDispIndex==2)
				{
					LanguageMenuProcess();	
				}
				else if(gc_LCMDispIndex==3)
				{
					gc_Task_Next=C_Task_Play;
				}
				tc_MenuItem=0xff;
			}
		}
		else
		{
			if(gc_PlayMenu_Timeout==0)  //TimeOut
			{
				gc_DIRTimeOut_Flag=1;
				gb_FindFlag = 1;
				gc_Task_Next=C_Task_Play;
			}
		}

		gc_PhaseInx=0;		
		IR_Service_Process_Menu();
		Polling_TaskEvents();

       	if(gc_Task_Current!=gc_Task_Next)
        {
			gc_PhaseInx=0;
			LCM_clear();
			gc_LCM_Media=0xff;
			gc_DisplayEQIcon=0xFF;
			gw_DisplayBitRate=0xFFFF;
			gc_DisplayPlayMode=0xFF;
			gw_DispSongNum=0xFFFF;
			gw_DispFileName_ByteOffset=0;
			gc_Task_Current=gc_Task_Next;
            break;  
        }
	}	
}

void ir_service_menu(void)
{
	U16 tw_temp=0;	
	if(gc_IRCmdStatus!=0)
	{
		tw_temp=(IR_REG[0x1b]<<8)+IR_REG[0x1a];
		if(tw_temp==IR_6_Key)
		{
			gc_irkey=1;
		}
		if(tw_temp==IR_7_Key)
		{
			gc_irkey=2;	
		}
		if(tw_temp==IR_18_Key)
		{
			gc_irkey=3;	
		}
		if(tw_temp==IR_20_Key)
		{
			gc_irkey=4;	
		}
		if(tw_temp==IR_23_Key)
		{
			gc_irkey=5;	
		}
		if((tw_temp==IR_11_Key) || (tw_temp==IR_11_1_Key))
		{
			gc_irkey=6;	
		}

		if(gc_irkey!=0)
		{
			if((IR_REG[0x1c]+IR_REG[0x1d])==0xFF)
			{
				if(gc_IRCmdStatus==1)
				{
					ir_commandservice_menu();
					gc_IRCmdStatus=0;
				}
				else if(gc_IRCmdStatus==2)
				{					
					ir_commandservice_menu();
					gc_IRCmdStatus=0;
				}
			}					
		}
		else
		{
			gc_IRCmdStatus=0;
		}
		gc_IRCmdStatus=0;
	}
	gc_irkey=0;
}


void ir_commandservice_menu(void)
{
	if(gc_irkey==3)					// 21key_02FD
	{
	    switch(IR_REG[0x1c])
	    {
	        case 0x05:	// EQ
				gc_PhaseInx = 0x14;
				break;
	        case 0x04:	// volume --
				gc_PhaseInx = 6;
				break;
	        case 0x06:	// volume ++
				gc_PhaseInx = 5;
				break;
			default:		
			break;
	    }
	}
	else if(gc_irkey==4)	// 21-key_00FF
	{
	    switch(IR_REG[0x1c])
	    {
	        case 0x46:	// EQ
				gc_PhaseInx = 0x14;
				break;
	        case 0x44:	// volume --
				gc_PhaseInx = 6;
				break;
	        case 0x40:	// volume ++
				gc_PhaseInx = 5;
				break;
			default:		
			break;
	    }
	}
	else if(gc_irkey==5)	// 23-key
	{
	    switch(IR_REG[0x1c])
	    {
	        case 0x01:	// menu
				gc_PhaseInx = 0x14;
				break;
	        case 0x0d:	// volume --
				gc_PhaseInx = 6;
				break;
	        case 0x0e:	// volume ++
				gc_PhaseInx = 5;
				break;
			default:		
			break;
	    }
	}

}


void IR_Service_Process_Menu()
{
    if(gc_IRCmdStatus==1)
    {
		gc_PlayMenu_Timeout=160;
        ir_service_menu();
    }
}

//================================================================================
void PowerOffProcess()
{
	U16 tw_temp;
    U8 i;
    data U16 fm_backup;
    fm_backup=gw_FM_frequency;	
	gw_FM_frequency =875-5;
	FM_drive();
	gc_PhaseInx=0;
	set_clock_mode(CLOCK_MODE_MP3);

	LCM_BACKLIGHT=1;
	LCM_clear();
	gw_IR_Timer=0;
//	gc_VolumeMute=0;
//	gc_linein_mute=0;
//	gb_Host_Exist=1;
//	gb_SD_Exist=1;
	while(1)
	{
		if(gc_PhaseInx==C_PlayPause)		
		{		
		//	SourceDevicePreSet(); 									
			gc_Task_Next=C_Task_Play; 
            gw_FM_frequency=fm_backup;                                                                                            		
		}
		else gc_PhaseInx = 0;
//=========================================================
//=========================================================
		if(gc_Vrefinit_Timer==0)
		{
			TimeOutHandle();
		}
		if(gc_KeyValue==0x02)//³¤°´Play
		{					
			gc_PhaseInx = C_PlayPause;
		}  
		else if(gc_IRCmdStatus!=0)
		{
			tw_temp=(IR_REG[0x1b]<<8)+IR_REG[0x1a];
			
			if(tw_temp==IR_23_Key)
			{
				if((IR_REG[0x1c]+IR_REG[0x1d])==0xFF)
				{
					if(IR_REG[0x1c] == 0x00)  //Power ON
					{
						if(gc_IRCmdStatus==1)
						gc_PhaseInx = C_PlayPause;
					}
				}				
			}
            else if(tw_temp==IR_18_Key)
			{
				if((IR_REG[0x1c]+IR_REG[0x1d])==0xFF)
				{
					if(IR_REG[0x1c] == 0x12)  //Power ON
					{
						if(gc_IRCmdStatus==1)
						gc_PhaseInx = C_PlayPause;
					}
				}				
			}
			gc_IRCmdStatus=0;
		}

		gc_KeyValue=0;
//=========================================================
//=========================================================
		if(gc_Task_Current!=gc_Task_Next)
		{
			gc_PhaseInx = 0;
			gc_LCM_Media=0xff;
			gc_DisplayEQIcon=0xFF;
			gw_DisplayBitRate=0xFFFF;
			gc_DisplayPlayMode=0xFF;
			gw_DispSongNum=0xFFFF;
			gc_DIRTimeOut_Flag=1;
			gb_FindFlag = 1;
			gw_DispFileName_ByteOffset=0;
		//	gw_init_needed=0xffff;
			gc_Task_Current=gc_Task_Next;                         
			break;  
		}
	}
	if((gw_FM_frequency<875) || (gw_FM_frequency>1080))
    {
        gw_FM_frequency=875;
    }
//	FM_initial(); 
	FM_drive();

	LCM_BACKLIGHT=0;
	LCM_ShowCompanyLogo();  //chiayenmark for car
    for(i=0;i<=5;i++)
	{
		USER_DelayDTms(250);
	}
	LCM_clear();  //20081124 chiayen add
}

