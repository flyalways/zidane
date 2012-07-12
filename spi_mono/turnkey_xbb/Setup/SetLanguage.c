#include "..\Header\SPDA2K.h"
#include "..\header\variables.h"

#ifdef SPI_1M
void SetLanguage(void)
{
	data	U8	tc_Language;
	//data	U8	i;

	tc_Language=gc_CurrentHZK;
	LCM_Clear_L0_L7();
	if(tc_Language==0)
	{	
		gc_CurrentHZK=0;
		gb_LCM_InverseDisp=1;	 
		LCM_ShowCountryMenu();
		gc_CurrentHZK++;
		gb_LCM_InverseDisp=0;
		LCM_ShowCountryMenu();	
	}
	else
	{  
		gc_CurrentHZK=0;
		gb_LCM_InverseDisp=0;	 
		LCM_ShowCountryMenu();
		gc_CurrentHZK++;
		gb_LCM_InverseDisp=1;
		LCM_ShowCountryMenu();	
	}
	

	gc_CurrentHZK=tc_Language;
	gc_MenuTimer=6;
	while(1)
	{
		Key_Detect();
		if(gc_KeyEvent!=0)
		{
			gc_MenuTimer=6;
			if(gc_KeyEvent==3 || gc_KeyEvent==4)	//next/ Prev
			{  
				gb_LCM_InverseDisp=0;
				LCM_ShowCountryMenu();
			   	if(gc_CurrentHZK==0x00)
				{
					gc_CurrentHZK=1;
				}else
				{	
					gc_CurrentHZK=0;
				}
				//gc_CurrentHZK--;
				gb_LCM_InverseDisp=1;
				LCM_ShowCountryMenu();
			}
			else if(gc_KeyEvent==6)	// M
			{
				USER_LogFile_ReadWrite(1);
				gb_LCM_InverseDisp=0;
				gc_Task_Current=C_Task_Setup;
				break;
			}
			else if(gc_KeyEvent==0x16)	// L-M(Abort)
			{
				gc_MenuTimer=0;
			}
		}

		if(gc_MenuTimer==0)
		{
			gc_Task_Current=C_Task_Idle;
			break;
		}
	}

}

#else
void SetLanguage(void)
{
	data	U8	tc_Language;
	data	U8	i;

	tc_Language=gc_CurrentHZK;
	gc_CurrentHZK&=~0x03;
	for(i=0;i<4;i++)
	{
		gb_LCM_InverseDisp=0;
		if(gc_CurrentHZK==tc_Language)
		{
			gb_LCM_InverseDisp=1;
		}
		LCM_ShowCountryMenu();
		gc_CurrentHZK++;
	}
	gc_CurrentHZK=tc_Language;
	gc_MenuTimer=6;
	while(1)
	{
		Key_Detect();
		if(gc_KeyEvent!=0)
		{
			gc_MenuTimer=6;
			if(gc_KeyEvent==3)	// Next
			{
				if(((gc_CurrentHZK&0x03)==0x03)||(gc_CurrentHZK==16))
				{
					if(gc_CurrentHZK==16)
					{
						gc_CurrentHZK=0;
					}
					else
					{
						gc_CurrentHZK++;
					}
					tc_Language=gc_CurrentHZK;
					gc_CurrentHZK&=~0x03;
					for(i=0;i<4;i++)
					{
						gb_LCM_InverseDisp=0;
						if(gc_CurrentHZK==tc_Language)
						{
							gb_LCM_InverseDisp=1;
						}
						LCM_ShowCountryMenu();
						gc_CurrentHZK++;
					}
					gc_CurrentHZK=tc_Language;
				}
				else
				{
					gb_LCM_InverseDisp=0;
					LCM_ShowCountryMenu();
					gc_CurrentHZK++;
					gb_LCM_InverseDisp=1;
					LCM_ShowCountryMenu();
				}
			}
			else if(gc_KeyEvent==4)	// Prev
			{
				if((gc_CurrentHZK&0x03)==0x00)
				{
					gc_CurrentHZK--;
					if(gc_CurrentHZK==0xFF)
					{
						gc_CurrentHZK=16;
					}
					tc_Language=gc_CurrentHZK;
					gc_CurrentHZK&=~0x03;
					for(i=0;i<4;i++)
					{
						gb_LCM_InverseDisp=0;
						if(gc_CurrentHZK==tc_Language)
						{
							gb_LCM_InverseDisp=1;
						}
						LCM_ShowCountryMenu();
						gc_CurrentHZK++;
					}
					gc_CurrentHZK=tc_Language;
				}
				else
				{
					gb_LCM_InverseDisp=0;
					LCM_ShowCountryMenu();
					gc_CurrentHZK--;
					gb_LCM_InverseDisp=1;
					LCM_ShowCountryMenu();
				}
			}
			else if(gc_KeyEvent==6)	// M
			{
				USER_LogFile_ReadWrite(1);
				gb_LCM_InverseDisp=0;
				gc_Task_Current=C_Task_Setup;
				break;
			}
			else if(gc_KeyEvent==0x16)	// L-M(Abort)
			{
				gc_MenuTimer=0;
			}
		}

		if(gc_MenuTimer==0)
		{
			gc_Task_Current=C_Task_Idle;
			break;
		}
	}

}

#endif