#include "..\LCM\LCM.h"
#include "..\LCM\LCM_func.h"
#include "..\LCM\UI_icon.h"


extern xdata U8	gc_strobe_bak;
extern xdata U8  gc_0xB400_bak; 
extern xdata U16 gw_BmpInx;
extern xdata U8  gc_BmpString[];
//extern xdata U16 gw_icon_sector;



#ifdef USE_TFT_disp_HZKCharBMP
static void TFT_disp_HZKCharBMP(U8 tc_Width,U8 reverse,U8 xdata *BMPdataBuf)  //yflin080820
{
	data	U8	tc_i,j,m;
	data	U8	tc_char;

	//for 023Only
	LCD_Nand2DataMode();
	
	if(reverse)
	{		
		while(tc_Width!=0)
		{
			if(tc_Width>16)
			{
				tc_Width -= 16;
				tc_i = 32;
			}
			else
			{ 
				tc_i = tc_Width*2;
				if (tc_i == 0)
				{
					break;
				}			
				tc_Width = 0;
			}

			do 
			{ // loop 'tc_i' times
				tc_char = *BMPdataBuf++;
				m = 8;
				do 
				{ // loop 8 times
					tc_char = tc_char>>1;	//bitbit shift
					if(CY)		//低位为1 将输入一个点的颜色
					{	
						XBYTE[0xB420] = WordColor_DIR_Select/256; 
						XBYTE[0xB420] = WordColor_DIR_Select; 
					}
					else
					{
						XBYTE[0xB420] = WordColor_DIRBG_Select/256; 
						XBYTE[0xB420] = WordColor_DIRBG_Select; 
					}
				} while (--m);
			} while (--tc_i);
		}
	}
	else  // if(reverse)
	{// not reverse,┏256Ω笲衡┪128Ω笲衡,color16bit,┮羆bytes256*2 or 128*2			
		while(tc_Width!=0)
		{
			if(tc_Width>16)
			{
				tc_Width -= 16;
				tc_i = 32;
			}
			else
			{ 
				tc_i = tc_Width*2;
				if (tc_i == 0)
				{
					break;
				}
				tc_Width = 0;
			}

			j = tc_i;
			do { // loop 'tc_i' times
				tc_char = *BMPdataBuf++;
				m = 8;
				do { // loop 8 times
					tc_char = tc_char>>1;	//bitbit shift
					if(CY)		//低位为1 将输入一个点的颜色
					{	
						XBYTE[0xB420] = WordColor_DIR_NOSelect/256; 
						XBYTE[0xB420] = WordColor_DIR_NOSelect; 
					}
					else
					{
						XBYTE[0xB420] = WordColor_DIRBG_NOSelect/256; 
						XBYTE[0xB420] = WordColor_DIRBG_NOSelect; 	
					}
				} while (--m);
			} while (--tc_i);
		} 
	}

	LCD_Data2NandMode();
}
#endif










