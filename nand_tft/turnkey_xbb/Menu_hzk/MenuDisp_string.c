//#include "..\turnkey_xbb\UI_header\UI_define.h"	//

#include "..\UI_Display\unicode_hzk.c"

#define HZK_CHAR        0
#define HZK_WORD        1
#define HZK_NOTREVERSE  0
#define HZK_REVERSE     1

void Menu_Disp_String(U16 tc_x_start,U16 tc_y_start,U8 *tpc_DataBuf,U16 tw_FontColor,U16 tw_BackColor);                                                                             
void TFT_disp_HZKCharBMP_Menu(U8 CharOrWord,U8 reverse,U8 *BMPdataBuf,U16 tw_FontColor,U16 tw_BackColor);  
/////////////////////////////////////////sunzhk add new

static void Menu_Disp_String(U16 tc_x_start,U16 tc_y_start,U8 *tpc_DataBuf,U16 tw_FontColor,U16 tw_BackColor)
{
    U8 i;
	U8 Tmp_DataBuf[32];
	U8 tc_ColumnNum;
	U8 tc_ByteNumber;
	U8 tc_LowByteFirstFlag;
	tc_ByteNumber = tpc_DataBuf[0] & 0x7f;
	tc_LowByteFirstFlag = tpc_DataBuf[0] & 0x80;
    
    i = 1;  
	while(i<=tc_ByteNumber)
	{
		if(tc_LowByteFirstFlag)
		{
			tc_ColumnNum = UNICODE_HZK_GET_BMP(*(tpc_DataBuf+i+1),*(tpc_DataBuf+i),Tmp_DataBuf,0);
		}else
        {  //ĿǰUI_Tab�ĵ����ݴ洢��ʽ���߸÷�֧
            tc_ColumnNum = UNICODE_HZK_GET_BMP(*(tpc_DataBuf+i),*(tpc_DataBuf+i+1),Tmp_DataBuf,0);
        }
    	LCM_set_view(TFT_HZKDispDir, tc_x_start, tc_y_start,tc_ColumnNum&0x7f,16);	
        if((tc_ColumnNum&0x7f)>8)
		{  
           // dbprintf("word\n");
	        TFT_disp_HZKCharBMP_Menu(HZK_WORD,HZK_NOTREVERSE,Tmp_DataBuf,tw_FontColor,tw_BackColor);  
		}else
        {  
           // dbprintf("char\n");
	        TFT_disp_HZKCharBMP_Menu(HZK_CHAR,HZK_NOTREVERSE,Tmp_DataBuf,tw_FontColor,tw_BackColor); 
		}	

	    tc_x_start+=(tc_ColumnNum&0x7f);
           if (tc_x_start>TFT_H_Size-9)  break;
	    if(tc_ColumnNum&0x80)     
		{
			i+=2;
		}else
        {
			i+=1;
		}  
    }
}  

extern void LCD_Write_OneWord(U16 w);
extern void LCD_Nand2DataMode(void);
extern void LCD_Data2NandMode(void);
/////��ʾһ���ַ�����ɫ                                                                                 
static void TFT_disp_HZKCharBMP_Menu(U8 CharOrWord,U8 reverse,U8 *BMPdataBuf,U16 tw_FontColor,U16 tw_BackColor)  
{ //��ʾ���ַ�Ϊ8�л���16��--�Ƿ�ɫ--�ַ�λͼ���ݴ�ŵ�buffer---��ʾ�������ɫ---���屳����ɫ
  //reverse:0����ɫ 1������ͱ�������ɫ  //�ú�������չΪ����ֻ�б�����ɫ��ֻ�����巴ɫ 
	U8 tc_i,j,m;
	U8 tc_char;

	if(CharOrWord)			tc_i = 32;		
	else					tc_i = 16;
       ////////////// test         
      
LCD_Nand2DataMode(); 
    //////////////////////
	for(j=0;j<tc_i;j++)
	{
		tc_char = BMPdataBuf[j];       
		for(m=0;m<8;m++)
		{
			tc_char = tc_char>>1;
			if(CY)		//��λΪ1 ������һ�������ɫ
			{
				if(reverse)
				{				
					LCD_Write_OneWord(~tw_FontColor);
    			}
    			else
    			{
    				LCD_Write_OneWord(tw_FontColor);
    			}	
    		}
			else
			{
				if(reverse)
				{				
					LCD_Write_OneWord(~tw_BackColor);
				}
				else
				{
					LCD_Write_OneWord(tw_BackColor);
				}
			}
		}
	}
	LCD_Data2NandMode();			
}
