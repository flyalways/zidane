#include "..\Header\SPDA2K.h"
#include "..\IR\remote.h"
#include "..\LCM\LCM_BMP.h"
#include "..\DSP\DSPPHYSIC.H"
#include "..\DSP\dspuser.h"
#include "..\header\variables.h"

void ShowSongFreq(U8 x_Column,U8 x_SongMhz);
void NumberIRKeySuccess(U8 x_MHz_SONG);
void ShowIrNumber(U8 x_Num,U8 x_Song_Freq);


void IR_Init(void)
{
    // set time out
    XBYTE[0xBA00] = LO_BYTE(TIME_OUT_MIN);
    XBYTE[0xBA01] = HI_BYTE(TIME_OUT_MIN);
    // start 1 max
    XBYTE[0xBA02] = LO_BYTE(START1_MAX);
    XBYTE[0xBA03] = HI_BYTE(START1_MAX);
    // start 1 min
    XBYTE[0xBA04] = LO_BYTE(START1_MIN);
    XBYTE[0xBA05] = HI_BYTE(START1_MIN);
    // start 0 max
    XBYTE[0xBA06] = LO_BYTE(START0_MAX);
    XBYTE[0xBA07] = HI_BYTE(START0_MAX);
    // start 0 min
    XBYTE[0xBA08] = LO_BYTE(START0_MIN);
    XBYTE[0xBA09] = HI_BYTE(START0_MIN);
#if (IR_MODULATION==IR_MOD_PPM) || (IR_MODULATION==IR_MOD_PWM)
    // logic 1 max
    XBYTE[0xBA0A] = LO_BYTE(LOGIC1_MAX);
    XBYTE[0xBA0B] = HI_BYTE(LOGIC1_MAX);
    // logic 1 min
    XBYTE[0xBA0C] = LO_BYTE(LOGIC1_MIN);
    XBYTE[0xBA0D] = HI_BYTE(LOGIC1_MIN);
#else
    // bit time
    XBYTE[0xBA0A] = LO_BYTE(BIT_TIME);
    XBYTE[0xBA0B] = HI_BYTE(BIT_TIME);
    // sample point
    XBYTE[0xBA0C] = LO_BYTE(SAMPLE_POINT);
    XBYTE[0xBA0D] = HI_BYTE(SAMPLE_POINT);
#endif
    // logic 0 max
    XBYTE[0xBA0E] = LO_BYTE(LOGIC0_MAX);
    XBYTE[0xBA0F] = HI_BYTE(LOGIC0_MAX);
    // logic 0 min
    XBYTE[0xBA10] = LO_BYTE(LOGIC0_MIN);
    XBYTE[0xBA11] = HI_BYTE(LOGIC0_MIN);
    // set sampling period = 1 usec
    XBYTE[0xBA12] = LO_BYTE(T_SAMPLE);
    XBYTE[0xBA13] = HI_BYTE(T_SAMPLE);       
    // set polarity
#if IR_MODULATION == IR_MOD_PPM
    XBYTE[0xBA17] = 0x01;
#elif IR_MODULATION == IR_MOD_PWM
    XBYTE[0xBA17] = 0x03;
#elif IR_MODULATION == IR_MOD_BIPHASE
    XBYTE[0xBA17] = 0x07;
#elif IR_MODULATION == IR_MOD_BINARY
    XBYTE[0xBA17] = 0x05;
#else
    while(1);
#endif

    XBYTE[0xBA19]=0x07; // clear all events
    XBYTE[0xBA18]=0x07; // enable all IR interrupts
    XBYTE[0xBA17]|=0x80; // IR reveiver HW enabled
    XBYTE[0xBA19]=0x07; // clear all events
}


void ClearIRNumberVariable(void)
{
	memset(gc_NUMBER,0,4); //Clear
	gc_ComColumn=0;
	gb_IRNumberFlag=0;
	gc_IRNumberCnt=0;
}


//----------------------------------------------------------------------------
// 顯示 SONG 0
// 顯示 MHz  1
//---------------------------------------------------------------------------
void ShowSongFreq(U8 x_Column,U8 x_SongMhz)
{
    if(x_SongMhz)
    {
       LCM_ShowChar12x32(21,x_Column+12);   // M
       LCM_ShowChar12x32(17,x_Column+24);   // H
       LCM_ShowChar12x32(30,x_Column+36);   // z
    }
	else
	{
       LCM_ShowChar12x32(26,x_Column);      // S        
       LCM_ShowChar12x32(23,x_Column+12);   // O
       LCM_ShowChar12x32(22,x_Column+24);   // N
       LCM_ShowChar12x32(16,x_Column+36);   // G
       LCM_ShowChar12x32(31,x_Column+48);   // space
    }
}
//---------------------------------------------------------------------------
//  x_MHz_SONG ==2   SONG
//  x_MHz_SONG ==1   Freqency
//---------------------------------------------------------------------------
void NumberIRKeySuccess(U8 x_MHz_SONG)
{
	U16 xw_temp;

	xw_temp = (gc_NUMBER[3]&0x0F)*1000;
	xw_temp += (gc_NUMBER[2]&0x0F)*100;
	xw_temp += (gc_NUMBER[1]&0x0F)*10;
	xw_temp += (gc_NUMBER[0]&0x0F);

	if(x_MHz_SONG==1)   //1==FREQ
	{
		LCM_Clear_L2_L5();
		if((xw_temp<875)||(xw_temp>1080))
		{
			LCM_ShowERROR();
		}
		else
		{
//-t			FMIRNumberKey(xw_temp);
		}
	}
	else
	{            //SONG
		if((xw_temp>gw_FileTotalNumber[0])||(xw_temp==0))
		{
			LCM_Clear_L2_L5();
			LCM_ShowERROR();
		}
		else
		{//Select Song Success-----------------------------
			if(xw_temp==gw_FileIndex[0])
			{
				gc_IR_Timer=0;
			}
			else if(xw_temp>gw_FileIndex[0])
			{
				gw_FileSkipNumber=xw_temp-gw_FileIndex[0];
				play_next();
			}
			else
			{
				gw_FileSkipNumber=(gw_FileTotalNumber[0]-gw_FileIndex[0])+xw_temp;
				play_next();
			}
		}
	}
	gb_ClearBigIcon=1;
	ClearIRNumberVariable();
}

//-------------------------------------------------------------------------------
// U8 x_Song_Freq 0=最後在設定
//                1=頻率
//                2=歌曲
//   x_Num==數字 0~9
//-------------------------------------------------------------------------------
void ShowIrNumber(U8 x_Num,U8 x_Song_Freq)
{
    if(gs_System_State.c_Phase==TASK_PHASE_PAUSE) return;
    gb_IRNumberFlag=1;
    switch(gc_IRNumberCnt)
    {
      case 0x00:
           LCM_Clear_L2_L5();
           if(x_Song_Freq==1)
           {
              ShowSongFreq(70,1);    //SHOW MHz
              gc_NUMBER[0] = x_Num;  //Save Number
              gc_ComColumn=5;
              LCM_ShowChar12x32(gc_NUMBER[0],gc_ComColumn);
           }
           else if(x_Song_Freq==2)
           {
              ShowSongFreq(0,0);     //Show Song 12*5=60
              gc_NUMBER[0] = x_Num;  //Save Number
              gc_ComColumn=60;
              LCM_ShowChar12x32(gc_NUMBER[0],gc_ComColumn);
           }
           else                      //Null
           {
              gc_NUMBER[0] = x_Num;  //Save Number
              gc_ComColumn=60;
              LCM_ShowChar12x32(gc_NUMBER[0],gc_ComColumn);
           }
           break;
      case 0x01:
           gc_NUMBER[1] = gc_NUMBER[0];
           gc_NUMBER[0] = x_Num;  //Save Number

           LCM_ShowChar12x32(gc_NUMBER[1],gc_ComColumn);
           LCM_ShowChar12x32(gc_NUMBER[0],gc_ComColumn+12);
           break;
      case 0x02:
           gc_NUMBER[2] = gc_NUMBER[1];
           gc_NUMBER[1] = gc_NUMBER[0];
           gc_NUMBER[0] = x_Num;  //Save Number
           LCM_ShowChar12x32(gc_NUMBER[2],gc_ComColumn);
           LCM_ShowChar12x32(gc_NUMBER[1],gc_ComColumn+12);
           LCM_ShowChar12x32(gc_NUMBER[0],gc_ComColumn+12+12);
           break;
      case 0x03:
           gc_NUMBER[3] = gc_NUMBER[2];
           gc_NUMBER[2] = gc_NUMBER[1];
           gc_NUMBER[1] = gc_NUMBER[0];
           gc_NUMBER[0] = x_Num;  //Save Number
           LCM_ShowChar12x32(gc_NUMBER[3],gc_ComColumn);
           LCM_ShowChar12x32(gc_NUMBER[2],gc_ComColumn+12);
           LCM_ShowChar12x32(gc_NUMBER[1],gc_ComColumn+12+12);
           LCM_ShowChar12x32(gc_NUMBER[0],gc_ComColumn+12+12+12);
           break;
    }
    gc_IR_Timer=120;
    if(gc_IRNumberCnt>=3)
    {
       NumberIRKeySuccess(x_Song_Freq);
       return;
    }
    gc_IRNumberCnt++;
}
//-------------------------------------------------------------------------------
// x_Sg_Fr == 0 or 2   SONG
// x_Sg_Fr == 1   Frequency
//-------------------------------------------------------------------------------
void CheckIRKeyPushDownTimerOut(U8 x_Sg_Fr)
{
	if(gc_IR_Timer==0)
	{
		NumberIRKeySuccess(x_Sg_Fr);
	}
}


void IR_Service_Process(void)
{
	data	U16 x_IrTemp;
	data	U8	tc_IR_NUM;
	
	((U8*)(&x_IrTemp))[0]=XBYTE[0xBA1B];
	((U8*)(&x_IrTemp))[1]=XBYTE[0xBA1A];

	tc_IR_NUM=0xFF;
	switch(x_IrTemp)   //用戶碼
	{
		case IR_CustomerCode://-----IR 21 Key Function-----
			if((XBYTE[0xBA1C]+XBYTE[0xBA1D])!=0xFF) 
			{
				return; //Code ERROR
			}

			switch(XBYTE[0xBA1C])
			{
				case 0x19://-------------- +100/RPT---------------
					switch(gc_Task_Current)
					{
						case C_Task_FM:
							if(gc_IRCmdStatus==1)    //Short Key
							{
								gc_KeyEvent=0;        //NULL Key
							}
						break;
                    
						case C_Task_MusicPlay:
							if(gc_IRCmdStatus==1)   // Short Key
							{
								gc_KeyEvent=0x12;	// Play Mode change
							}
						break;
					}
				break;

				case 0x0D://----------- +200/ U/SD----------------------
					switch(gc_Task_Current)
					{
						case C_Task_FM:
							if(gc_IRCmdStatus==1)    // Short Key
							{
								gc_KeyEvent=0x05;    // Media change
							}
						break;

						case C_Task_MusicPlay:
							if(gc_IRCmdStatus==1)    //Short Key
							{
								gc_KeyEvent=0x05;    // Media change
							}
						break;
					}
				break;

				case 0x43://-----------Play/Pause-----------------------
					switch(gc_Task_Current)
					{
						case C_Task_FM:
							if(gc_IRCmdStatus==1)    //Short Key
							{
								gc_KeyEvent=0;        //NULL Key
							}
						break;
                    
						case C_Task_MusicPlay:
							if(gc_IRCmdStatus==1)    //Short Key
							{
								gc_KeyEvent=0x02;     //Play/Pause
							}
						break;
					}
				break;

				case 0x44://-----------Prev---------------------
					switch(gc_Task_Current)
					{
						case C_Task_FM:
							if(gc_IRCmdStatus==1)    //Short Key
							{
								gc_KeyEvent=0;        //NULL Key
							}
						break;
                    
						case C_Task_MusicPlay:
							if(gc_IRCmdStatus==1)    //Short Key
							{
								gc_KeyEvent=0x04;     //Prev
							}
							else if(gc_IRCmdStatus==2)    //Long Key
							{
								gc_KeyEvent=0x14;     //Prev folder
							}
						break;
					}
				break;

				case 0x40://-----------Next-----------------------
					switch(gc_Task_Current)
					{
						case C_Task_FM:
							if(gc_IRCmdStatus==1)    //Short Key
							{
								gc_KeyEvent=0;        //NULL Key
							}
						break;

						case C_Task_MusicPlay:
							if(gc_IRCmdStatus==1)    //Short Key
							{
								gc_KeyEvent=0x03;     //Next
							}
							else if(gc_IRCmdStatus==2)    //Long Key
							{
								gc_KeyEvent=0x13;     //Next folder
							}
						break;
					}
				break;

				case 0x09://-----------EQ Mode--------------------
					switch(gc_Task_Current)
					{
						case C_Task_FM:
							if(gc_IRCmdStatus==1)    //Short Key
							{
								gc_KeyEvent=0;        //NULL Key
							}
						break;

						case C_Task_MusicPlay:
							if(gc_IRCmdStatus==1)    //Short Key
							{
								gc_KeyEvent=0x15;     //EQ Mode
							}
						break;
					}
				break;
    
				case 0x07://-----------VolumeDown-----------------------
					switch(gc_Task_Current)
					{
						case C_Task_FM:
						case C_Task_MusicPlay:
							if(gc_IRCmdStatus==1)		//Short Key
							{
								gc_KeyEvent=0x07;		//Vol-
							}
							else if(gc_IRCmdStatus==2)	//Long Key
							{
								gc_IRLongKeyCount=IRKEY_REPEATTIMES-3;
								gc_KeyEvent=0x07;		//Vol-
							}
						break;
					}
				break;
    
				case 0x15://-----------VolumeUp-----------------------
					switch(gc_Task_Current)
					{
						case C_Task_FM:
						case C_Task_MusicPlay:
							if(gc_IRCmdStatus==1)		//Short Key
							{
								gc_KeyEvent=0x06;		//Vol+
							}
							else if(gc_IRCmdStatus==2)	//Long Key
							{
								gc_IRLongKeyCount=IRKEY_REPEATTIMES-3;
								gc_KeyEvent=0x06;		//Vol+
							}
						break;		
					}
				break;

				case 0x45://-----------CH- -------------------------
					switch(gc_Task_Current)
					{
						case C_Task_FM:
							if(gc_IRCmdStatus==1)    //Short Key
							{
								gc_KeyEvent=0x04;     //Fequency Down
							}
						break;

						case C_Task_MusicPlay:
							if(gc_IRCmdStatus==1)    //Short Key
							{
								gc_KeyEvent=0x00;     //Null Key
							}
						break;
					}
				break;
			
				case 0x47://-----------CH+-----------------------
					switch(gc_Task_Current)
					{
						case C_Task_FM:
							if(gc_IRCmdStatus==1)    //Short Key
							{
								gc_KeyEvent=0x03;     //Fequency Up
							}
						break;
			
						case C_Task_MusicPlay:
							if(gc_IRCmdStatus==1)    //Short Key
							{
								gc_KeyEvent=0x00;     //Null Key
							}
						break;
					}
				break;
	
				case 0x46://-----------CH-----------------------
					if(gc_IRCmdStatus==1)
					{
						gc_Task_Current=C_Task_Suspend;
					}
				break;

				case 0x16://-----------0------------------------
					if(gc_IRCmdStatus==1)    //Short Key
					{
						tc_IR_NUM=0;
					}
				break;
			
				case 0x0C://-----------1---------------------------
					if(gc_IRCmdStatus==1)    //Short Key
					{
						tc_IR_NUM=1;
					}
				break;
            
				case 0x18://-----------2--------------------------
					if(gc_IRCmdStatus==1)    //Short Key
					{
						tc_IR_NUM=2;					
					}
				break;

				case 0x5E://-----------3--------------------------
					if(gc_IRCmdStatus==1)    //Short Key
					{
						tc_IR_NUM=3;
					}
				break;

				case 0x08://-----------4---------------------------
					if(gc_IRCmdStatus==1)    //Short Key
					{
						tc_IR_NUM=4;
					}
				break;

				case 0x1C://-----------5-----------------------
					if(gc_IRCmdStatus==1)    //Short Key
					{
						tc_IR_NUM=5;
					}
				break;

				case 0x5A://-----------6-----------------------
					if(gc_IRCmdStatus==1)    //Short Key
					{
						tc_IR_NUM=6;
					}
				break;

				case 0x42://-----------7-----------------------
					if(gc_IRCmdStatus==1)    //Short Key
					{
						tc_IR_NUM=7;
					}
				break;
            
				case 0x52://-----------8-----------------------
					if(gc_IRCmdStatus==1)    //Short Key
					{
						tc_IR_NUM=8;
					}
				break;
			
				case 0x4A://-----------9-----------------------
					if(gc_IRCmdStatus==1)    //Short Key
					{
						tc_IR_NUM=9;
					}
				break;
			}

			if(tc_IR_NUM!=0xFF)
			{
				if(gc_Task_Current==C_Task_FM)
				{
					ShowIrNumber(tc_IR_NUM,1);    //FM MODE
				}
				else
				{
					ShowIrNumber(tc_IR_NUM,2);    //SONG MODE
				}	
			}		
		break;
	}
	gc_IRCmdStatus=0;
}


void Change_EQ(void)
{
	gs_DSP_GLOBAL_RAM.sc_EQ_Type++;
	if(gs_DSP_GLOBAL_RAM.sc_EQ_Type>5)
	{
		gs_DSP_GLOBAL_RAM.sc_EQ_Type=0;    
	}
	Music_EQ_Cmd();
	LCM_Clear_L2_L5();
	//---Show EQ大圖形------------------------------------
	LCM_ShowEQ();
	//---Show EQ小圖形------------------------------------
	LCM_ShowEQIcon();
	gc_ShowTimer=72;
}


void MediaChange(void)
{
	gb_Dosinitfail=0;
	switch(gc_CurrentCard)
	{
		case 0x02:  //SD-->HOST
			if(gb_Host_Exist==0)
			{
				return;
			}
			gb_MediaMask=1;
		break;

		case 0x05:  //HOST--->SD
			if(gb_SD_Exist==0)
			{
				return;
			}
			gb_MediaMask=1;
		break;
	}
	gb_LrcFileName_Exist=0;
	play_stop();
}





