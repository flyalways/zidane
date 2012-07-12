#include "SPDA2K.h"
#include "..\IR\LCM_BMP.h" 
#include "..\Menu_Hzk\Menu_Hzk_String.c" 
/////////////////////////////===============================================////////////////////////
/////////////////////////////===============================================////////////////////////
extern xdata U8  	gc_language_offset;

void LCD_erase_one_page_PlayMenu_test(U8 tc_page)
{
	U8 i;
	LCM_set_address(tc_page, 0);
	for(i=0;i</*LCD_WIDTH*/128;i++)
	{
		LCM_write_data(0x00);
	}
}
void LCM_disp_HZKCharBMP_Menu(U8 Page, U8 Column,U8 *BMPdataBuf, U8 CharOrWord,U8 reverse)
{//防止切Bank，在该Bank复制一份显示函数
	U8 i,j;

	if(CharOrWord)//char(8x16)--------word(12x16)
	{
		i=0;j=31;
	}else{
		i=0;j=15;
	}
	LCM_set_address(Page,Column);

	for(;i<j;i=i+2)
	{
		if(reverse)
			LCM_write_data(~*(BMPdataBuf+(i)));
		else
			LCM_write_data(*(BMPdataBuf+(i)));
	}

	if(CharOrWord)
	{
		i=1;j=32;
	}else{
		i=1;j=16;
	}

	LCM_set_address(Page+1,Column);
	
	for(;i<j;i=i+2)
	{
		if(reverse)
			LCM_write_data(~*(BMPdataBuf+(i)));
		else
			LCM_write_data(*(BMPdataBuf+(i)));
	}
}
void disp_string(U8 tc_Page,U8 tc_Column,U8 *tpc_DataBuf,U8 tc_Reverse)//在tc_page,tc_Column以tc_Reverse方式显示unicode字符串buf,buf的结构参看strID
{
   U8 i;
	U8 Tmp_DataBuf[32];
	U8 tc_ByteNumber;
	U8 tc_ColumnNum;

	tc_ByteNumber = tpc_DataBuf[0] & 0x7f;

    i = 1;  
	while(i<=tc_ByteNumber)
	{
        tc_ColumnNum = LCM_UNICODE_HZK_GET_BMP(*(tpc_DataBuf+i),*(tpc_DataBuf+i+1),Tmp_DataBuf,1);
      
        if((tc_ColumnNum&0x7f)>8)
		{  	       
			LCM_disp_HZKCharBMP_Menu(tc_Page,tc_Column,Tmp_DataBuf,1,tc_Reverse);
		}else
        {  
		     LCM_disp_HZKCharBMP_Menu(tc_Page,tc_Column,Tmp_DataBuf,0,tc_Reverse); 
		}	

		tc_Column+=(tc_ColumnNum&0x7f);
		if (tc_Column>128-9)
		{
			break;
		}
	    if(tc_ColumnNum&0x80)     
		{
			i+=2;
		}else
        {
			i+=1;
		}		 
    }
	//disp blank 整行显示完
	{
		U8 i;
		LCM_set_address(tc_Page, tc_Column);
		for(i=0;i<128;i++)
		{
			LCM_write_data(0x00);
		}
		LCM_set_address(tc_Page+1, tc_Column);
		for(i=0;i<128;i++)
		{
			LCM_write_data(0x00);
		}
	}

}

#define PLAYMENU_1_NUMS  4
#define PLAYMENU_2_NUMS  3 //文件选择+全部播放
#define REPEAT_NUMS  4
#define EQ_NUMS		   7
#define LANGUAGE_NUMS  17

// iftc_RefreshType==0x80 
void Menu_Disp_Item_Play(U8 tc_MenuType,U8 tc_Select,U8 tc_RefreshType)          //tc_RefreshType always =1; 
{ 
        U8 tc_i; 
        U8 tc_ItemNums,tc_ItemTotalNums; 
        U8 tc_language; 
    U8 *(*p);          //指针数组 
        U8 tc_flag_dispItem=0; 
        U8 x_start; 

        tc_language = gc_language_offset; 
        
        //        dbprintf("\n#1=%bx #2=%bx #3=%bx\n",tc_MenuType,tc_Select,tc_RefreshType); 

        if(tc_RefreshType==0xff) 
        { 
                return; 
        } 

        switch(tc_MenuType) 
        { 
                case 0:        //palying to PlayMenu 
                                tc_ItemNums = PLAYMENU_2_NUMS; 
                                tc_ItemTotalNums=PLAYMENU_2_NUMS; 
                                p=PlayMenuTable_2;   
                        break; 
                ////////////////////////////////////////////////////////////////////// 
                case 1:        //pause to PlayMenu           
                        tc_ItemNums = PLAYMENU_1_NUMS; 
                        tc_ItemTotalNums=PLAYMENU_1_NUMS; 
                        p=PlayMenuTable_1;   
                        break; 
                case 2://language 
                
                        if((tc_Select >= 0)&&(tc_Select <= 3))// 多国语言第一个界面 
                        { 
                                tc_ItemNums=4; 
                                p=&Language_table[0];                                 
                        }else if((tc_Select >= 4)&&(tc_Select <= 7))// 多国语言第二个界面 
                        { 
                                tc_ItemNums=4; 
                                p=&Language_table[4]; 
                                tc_Select-=4; 
                        }else if((tc_Select >= 8)&&(tc_Select <= 11)) // 多国语言第三个界面 
                        { 
                                tc_ItemNums=4; 
                                p=&Language_table[8]; 
                                tc_Select-=8; 
                        }else if((tc_Select >= 12)&&(tc_Select <= 15)) // 多国语言第三个界面 
                        { 
                                tc_ItemNums=4; 
                                p=&Language_table[12]; 
                                tc_Select-=12; 
                        }else if((tc_Select >= 16)&&(tc_Select <= 19)) // 多国语言第三个界面 
                        { 
                                tc_ItemNums=1; 
                                p=&Language_table[16]; 
                                tc_Select-=16; 
                        } 
                        tc_language=0;                   
                        break; 
                case 3://rep_2Menu 
                /*        tc_ItemNums = REPEAT_NUMS; 
                        p=Repeat_Table; */ 
//                        dbprintf("menulevel=3,err!\n") ; 
                        break; 
                case 4://EQMenu 
                         if(tc_RefreshType==0x80) 
                        { 
                                LCD_erase_one_page_PlayMenu_test(3); 
                                LCD_erase_one_page_PlayMenu_test(4); 
                                if(tc_language==0 || tc_language==1|| tc_language==3 || tc_language==4  || tc_language==15  ) 
                                { 
                                        //column_string=;//*16/2 
                                        //x_start=64-EQ_Table[tc_language*6][0]*16/2/2;                                                        ; 
                                        x_start=64-EQ_Table[tc_Select+tc_language*7][0]*4; 
                                        if(x_start>128)         //not : x_start < 0 
                                        { 
                                                x_start=0; 
                                        } 
                                        disp_string(3,x_start,EQ_Table[tc_Select+tc_language*7],0); 
                                }else 
                                { 
                                        x_start=64-EQ_Table[tc_Select+tc_language*7][0]*2;         
                                        if(x_start>128)         //not : x_start < 0 
                                        { 
                                                x_start=0; 
                                        } 
                                        disp_string(3,x_start,EQ_Table[tc_Select+tc_language*7],0);                                 
                                } 

                                tc_flag_dispItem=1;         
                        }else 
                        { 
                                if((tc_Select >= 0)&&(tc_Select <= 3))// 多国语言第一个界面 
                                {         
                                        tc_ItemNums = 4; 
                                        tc_ItemTotalNums=EQ_NUMS; 
                                        p=EQ_Table;                         
                                }else if((tc_Select >= 4)&&(tc_Select <= 7))// 多国语言第二个界面 
                                { 
                                        tc_ItemNums=3; 
                                        tc_ItemTotalNums=EQ_NUMS; 
                                        p=&EQ_Table[4]; 
                                        tc_Select-=4; 
                                } 
                        } 
                        break; 
                case 5://rep mode 
                        if(tc_RefreshType==0x80) 
                        {         
                                LCD_erase_one_page_PlayMenu_test(3); 
                                LCD_erase_one_page_PlayMenu_test(4); 
                                if(tc_language==0 || tc_language==1|| tc_language==3 || tc_language==4 || tc_language==15) 
                                { 
                                        //column_string=;//*16/2 
                                        //x_start=64-EQ_Table[tc_language*6][0]*16/2/2;                                                        ; 
                                        x_start=64-Repeat_Table[tc_Select+tc_language*4][0]*4; 
                                        if(x_start>128)         //not : x_start < 0 
                                        { 
                                                x_start=0; 
                                        } 
                                        disp_string(3,x_start,Repeat_Table[tc_Select+tc_language*4],0); 
                                }else 
                                { 
                                        x_start=64-Repeat_Table[tc_Select+tc_language*4][0]*2;         
                                        if(x_start>128) 
                                        { 
                                                x_start=0; 
                                        } 
                                        disp_string(3,x_start,Repeat_Table[tc_Select+tc_language*4],0);                                 
                                } 

                                tc_flag_dispItem=1;         
                        }else 
                        { 
                                tc_ItemNums = REPEAT_NUMS; 
                                tc_ItemTotalNums= REPEAT_NUMS; 
                                p=Repeat_Table;   
                        }         
                        
                          
                        break; 

                default: 
                        tc_flag_dispItem=1; 
                        break; 
        } 
        if(tc_flag_dispItem) 
        {  // not disp item 
                return; 
        } 
        for(tc_i=0;tc_i<tc_ItemNums;tc_i++) 
        { 
                if(tc_i == tc_Select) 
                {       
           // Menu_Disp_String(tw_word_x,tw_icon_y+tc_High*tc_i,p[tc_i+tc_language*tc_ItemTotalNums],tw_color_select,tw_color_BG_select);         
                        disp_string(tc_i*2,0,p[tc_i+tc_language*tc_ItemTotalNums],1);                 
                }else 
                {   
                        disp_string(tc_i*2,0,p[tc_i+tc_language*tc_ItemTotalNums],0);   
            //Menu_Disp_String(tw_word_x,tw_icon_y+tc_High*tc_i,p[tc_i+tc_language*tc_ItemTotalNums],tw_color_Noselect,tw_color_BG_Noselect);             
                } 
        } 
        if(tc_ItemNums<4) 
        { //擦除不需要显示的行 
                for(tc_i=tc_ItemNums;tc_i<4;tc_i++) 
                { 
                        LCD_erase_one_page_PlayMenu_test(tc_i*2); 
                        LCD_erase_one_page_PlayMenu_test(tc_i*2+1); 
                } 
        } 
} 

