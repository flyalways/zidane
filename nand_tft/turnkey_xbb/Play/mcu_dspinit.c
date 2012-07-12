#include "SPDA2K.h"
#include "dos\fs_struct.h"
#include "Memalloc.h"
#include "..\DSP\DSPPHYSIC.H"
#include "..\DSP\dspuser.h"
#include "..\header\variables.h"

#include "..\UI_Display\common_UI_disp_func.c"

extern	U8	Get_LogData_PageIndex(void);
extern	U8	WMA_ASF_Parser();
extern	U8	USER_LogFile_ReadWrite(U8 tbt_ReadOrWrite);//use reserveblock for log
extern	U16	MP3_Total_Time_Parser();
extern	U16	MP3_Bitrate_Parser();

extern	xdata	System_Struct gs_System_State;

U8 MP3_Download(void)
{
	U8 tc_Cnt = 0;

 	if ( DSP_Download(0x00, 0x10)!=DSP_SUCCESS )
	{
		return DSP_PLAY_COMMAND_ERROR;
	}

	for (tc_Cnt=0;tc_Cnt<10;++tc_Cnt) 
	{
		if (DSP_PlayInit()==DSP_SUCCESS)
		{
			return 0;
		}

		if (tc_Cnt < 5) 
		{
			GLOBAL_REG[0x02] &= 0xFE;		// Enable DSP Run Normal Mode
		}
		else
		{
			GLOBAL_REG[0x02] |= 0x01;		// DSP reset
			GLOBAL_REG[0x02] &= 0xFE;		// Enable DSP Run Normal Mode
		}
	}

	return DSP_PLAY_COMMAND_ERROR;
}


U8 WMA_Download(void)
{
	U8 tc_Cnt = 0;
	U8 tc_result = 0;



	if ( DSP_Download(0x12, 0x25)!=DSP_SUCCESS )
		return DSP_PLAY_COMMAND_ERROR;

	L2_DSP_Write_DMem16(DSP_W_asf_parse_ok, 0x0);
	L2_DSP_Write_DMem16(DSP_W_asf_parse_mode, 0x01);		//Set ASF Parser Mode

	for (tc_Cnt=0;tc_Cnt<10;++tc_Cnt) 
	{
		if (DSP_PlayInit()==DSP_SUCCESS)
		{
			break;
		}

		if (tc_Cnt < 5) 
		{
			GLOBAL_REG[0x02] &= 0xFE;		// Enable DSP Run Normal Mode
		}
		else
		{
			GLOBAL_REG[0x02] |= 0x01;		// DSP reset
			GLOBAL_REG[0x02] &= 0xFE;		// Enable DSP Run Normal Mode
		}
	}
	if (tc_Cnt == 10)
		return DSP_PLAY_COMMAND_ERROR;

/*------------------------------------------------------------------*/
/* WMA download IMPM step:                                          */
/* 1. ASF parser mode: to check what entropy mode this bitstream is */
/* 2. check DRM: option                                             */
/* 3. normal wma download: to download the correct IMPM             */
/*------------------------------------------------------------------*/

//-------------------
//1. ASF parser mode
//-------------------

	if(L2_DSP_SendCommandSet(DCMD_Play) != DCMD_Play)
	{
		return DSP_PLAY_COMMAND_ERROR;
	}

	gs_DSP_GLOBAL_RAM.sc_DM_Index = 0;	// reset bitstream address of DM

	if (DOS_Open_File_r(0x00,0x02,0x00))
		return DSP_PLAY_COMMAND_ERROR;

	while(tc_result == 0)
	{
		if(WMA_DataIn() == DSP_DATAIN_COMMAND_ERROR) 	
			return DSP_DATAIN_COMMAND_ERROR;

		tc_result = L2_DSP_Read_DMem16(DSP_W_asf_parse_ok);
	}

	if(tc_result != 1)
	{
		return DSP_PLAY_COMMAND_ERROR;
	}

	L2_DSP_Write_DMem16(L2_DSP_Write_DMem16,0);
/*-- DSP will jump to pc=0 automatically after asf parse has been done--*/

//----------------------
//2. Normal wma download
//----------------------

	switch(L2_DSP_Read_DMem16(DSP_w_EntropyMode))
	{
		case 1:
				tc_result = DSP_Download(0x01, 0x23);
				break;

		case 2:
				tc_result = DSP_Download(0x01, 0x24);
				break;

		case 3:
				tc_result = DSP_Download(0x01, 0x25);
				break;
		default:
				break;
	}
		
	if(tc_result != DSP_SUCCESS )	
		return DSP_PLAY_COMMAND_ERROR;

	for (tc_Cnt=0;tc_Cnt<10;++tc_Cnt) {
		if (DSP_PlayInit()==DSP_SUCCESS)
			break;

		if (tc_Cnt < 5) 
		{
			//GLOBAL_REG[0x02] |= 0x01;		// DSP reset, ycc mark
			GLOBAL_REG[0x02] &= 0xFE;		// Enable DSP Run Normal Mode
		}
		else
		{
			GLOBAL_REG[0x02] |= 0x01;		// DSP reset
			GLOBAL_REG[0x02] &= 0xFE;		// Enable DSP Run Normal Mode
		}
	}

	if (tc_Cnt == 10)
		return DSP_PLAY_COMMAND_ERROR;	

	L2_DSP_Write_DMem16(DSP_W_asf_parse_mode, 0x0);		//Clear ASF Parser Mode

	return DSP_SUCCESS;
}


void play_init()
{ 
	gs_str_scroll_state.c_str_1st_char = 0;
	gs_str_scroll_state.c_shift_in_1st_char = 0;

	if(gw_init_needed & SET_BIT0)
    {
        gw_init_needed&=CLR_BIT0;      
        gs_System_State.c_FileHandle=0;
        gs_System_State.c_Phase = TASK_PHASE_STOP;
        gs_DSP_GLOBAL_RAM.sw_Volume = 50;//(JC)init value
        gs_DSP_GLOBAL_RAM.sc_EQ_Type=0;//(JC)init value
        gb_LrcFileName_Exist=0;
        gs_DSP_GLOBAL_RAM.sc_DM_Index = 0;  // reset bitstream address of DM
        gs_DSP_GLOBAL_RAM.s_DSP_RAM.sc_RepeatMode = REPEAT_AB_NULL;
        //(JC)search music file overall by FDB, findout 1st one in ROOT

		if(gb_DirPlay_Flag==1)
		{
			U16 tw_FileIndex=0;
	        Get_LogData_PageIndex();  //chiayen0808
	        USER_LogFile_ReadWrite(0);//use reserveblock for log

			gb_DirPlay_Flag=0;
            gc_RepPlayMode_Pre=gc_RepPlayMode;
            gc_RepPlayMode=2;
            gb_InitLogFileIndex=1;
			tw_FileIndex=gw_FileIndex[0];
            gw_FileSkipNumber=gw_FileIndex[0];
			gb_TriggerFileSkip=1;
			DOS_Search_File(C_File_OneDir|C_Cnt_FileNo, C_MusicFileType, C_CmpExtName|C_Next);
			if(DOS_Search_File(C_File_OneDir|C_By_FDB, C_MusicFileType, C_CmpExtName|C_Next))
			{
				gs_File_FCB[gs_System_State.c_FileHandle].dw_FDB_StartCluster = gdw_DOS_RootDirClus;
				DOS_Search_File(C_File_OneDir|C_Cnt_FileNo, C_MusicFileType, C_CmpExtName|C_Next);
				gw_FileSkipNumber=0;
				gw_FileIndex[0]=0;		
		        if(DOS_Search_File(C_File_All|C_By_Time, C_MusicFileType, C_CmpExtName|C_Next))
		        {
		            if(gc_CurrentCard==2) //if SD no file change to Host or flash
		            {
		                gb_SDNoFileflag=1;  
		            }
					if(gc_CurrentCard==5) //if Host no file change to SD or flash
		            {
		                gc_HostNoFileflag=1;  
		            }
					gb_FlashNoFileflag=0;
		            gw_init_needed |= SET_BIT8;
		            gc_PhaseInx = 9;
		
		            return;
		        }				
			}
			else
			{
				gw_FileIndex[0]=tw_FileIndex;
			}
			gb_TriggerFileSkip=0;
            gc_RepPlayMode=gc_RepPlayMode_Pre;
            gb_InitLogFileIndex=0;			
			gb_DirPlay_Flag=1;
		}
		else
		{
			gb_TriggerFileSkip=1;
			DOS_Search_File(C_File_All|C_Cnt_FileNo, C_MusicFileType, C_CmpExtName|C_Next);
			gs_File_FCB[gs_System_State.c_FileHandle].dw_FDB_StartCluster = gdw_DOS_RootDirClus;
	
	        if(DOS_Search_File(C_File_All|C_By_Time, C_MusicFileType, C_CmpExtName|C_Next))
	        {
	            if(gc_CurrentCard==2) //if SD no file change to Host or flash
	            {
	                gb_SDNoFileflag=1;  
	            }
				if(gc_CurrentCard==5) //if Host no file change to SD or flash
	            {
	                gc_HostNoFileflag=1;  
	            }
				gb_FlashNoFileflag=0;
	            gw_init_needed |= SET_BIT8;
	            gc_PhaseInx = 9;
	
	            return;
	        }
		}

		gc_Play_FileType=gs_File_FCB[gs_System_State.c_FileHandle].c_FileType;
		if(gc_Play_FileType==0)//(JC)MP3
		{
			set_clock_mode(CLOCK_MODE_MP3);
		}
		else if(gc_Play_FileType==1)//(JC)WMA //Jimi: gc_Play_FileType==2 ==>WMV
		{
			set_clock_mode(CLOCK_MODE_WMA);
        }

		if(gc_Play_FileType==0)
		{
			gs_System_State.w_BitRate=MP3_Bitrate_Parser();
			gw_TotalSec = MP3_Total_Time_Parser();//(JC)H0630 LCM test
		}
		else if(gc_Play_FileType==1)
		{
			WMA_ASF_Parser();
		}
		
		if(gb_DirPlay_Flag==0)
		{
			Get_LogData_PageIndex();  //chiayen0808
			USER_LogFile_ReadWrite(0);//use reserveblock for log
		}

		if((gw_FM_frequency<875) || (gw_FM_frequency>1080))
        {
            gw_FM_frequency=875;
        }

        if((gs_DSP_GLOBAL_RAM.sw_Volume<0) || (gs_DSP_GLOBAL_RAM.sw_Volume>62))
        {
            gs_DSP_GLOBAL_RAM.sw_Volume=50; 
        }

        if((gs_DSP_GLOBAL_RAM.sc_EQ_Type<0) || (gs_DSP_GLOBAL_RAM.sc_EQ_Type>5))
        {
            gs_DSP_GLOBAL_RAM.sc_EQ_Type=0; 
        }

		if(gc_MenuHZK > 17)
		{
			gc_MenuHZK=HZK_LANGUAGE;
		}

        if((gc_RepPlayMode>2))
        {
            gc_RepPlayMode=2;
        }

		if(gb_DirPlay_Flag==0)
		{
			gw_FileIndex[0]=1;
	        if(gc_CurrentCard==0)
	        { 
	            if(gw_LogFileIndex<=gw_FileTotalNumber[0])
	            {
	                if(gw_LogFileIndex==0)
	                {
	                    gw_LogFileIndex=1;
	                }
	                if(gw_LogFileIndex!=gw_FileIndex[0])
	                {
	                    gc_RepPlayMode_Pre=gc_RepPlayMode;
	                    gc_RepPlayMode=2;
	                    gb_InitLogFileIndex=1;
	                    gw_FileSkipNumber=gw_LogFileIndex-1;
	                    play_next();
	                    gc_RepPlayMode=gc_RepPlayMode_Pre;
	                    gb_InitLogFileIndex=0;
	                }
	            }
	        }
	        else if(gc_CurrentCard==2)
	        {
	            if(gw_SDLogFileIndex==0)
	            {
	                gw_SDLogFileIndex=1;    
	            }
	            if((gw_SDLogFileIndex<=gw_FileTotalNumber[0]))
	            {
	                if(gw_SDLogFileIndex!=gw_FileIndex[0])
	                {
	                    gc_RepPlayMode_Pre=gc_RepPlayMode;
	                    gc_RepPlayMode=2;
	                    gb_InitLogFileIndex=1;
	                    gw_FileSkipNumber=gw_SDLogFileIndex-1;
	                    play_next();
	                    gc_RepPlayMode=gc_RepPlayMode_Pre;
	                    gb_InitLogFileIndex=0;
	                }
	            }
	        }
	        else if(gc_CurrentCard==5)
	        {
	            if(gw_USBLogFileIndex==0)
	            {
	                gw_USBLogFileIndex=1;    
	            }
	            if((gw_USBLogFileIndex<=gw_FileTotalNumber[0]))
	            {
	                if(gw_USBLogFileIndex!=gw_FileIndex[0])
	                {
	                    gc_RepPlayMode_Pre=gc_RepPlayMode;
	                    gc_RepPlayMode=2;
	                    gb_InitLogFileIndex=1;
	                    gw_FileSkipNumber=gw_USBLogFileIndex-1;
	                    play_next();
	                    gc_RepPlayMode=gc_RepPlayMode_Pre;
	                    gb_InitLogFileIndex=0;
	                }
	            }
	        }
		}
		else
		{
		}

		if(gc_MenuHZK > 17)
		{
			gc_MenuHZK=HZK_LANGUAGE;//0; --sunzhk modify	
		}

        if((gc_RepPlayMode>2))
        {
            gc_RepPlayMode=2;
        }
	}

//	DOS_GetLongFileName(0,gc_FileLongName); 
	gw_DispSongNum1=0xFFFF;
	gc_PhaseInx=3;
}
