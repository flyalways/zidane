#include "..\Header\SPDA2K.h"
#include "..\LCM\model_define.h"//(JC)H1124
#include "..\header\variables.h"

#include "..\LCM\TFT_config.h"		// TFT
#include "..\Menu_hzk\MenuDisp_string.c"
#include  "..\Menu_hzk\String_HZK2.c"

//------------------��bank�з������ֲ˵���������FMѡ��ѡȡ������ַ���----
//-----------------��ʾ�ַ����ĺ�������ڸ��ļ��У����������ֵ���

#define MjpegMenuItemNums 3
#define Mjpeg_RepMenuItemNums 3
#define JpegMenuItemNums 3
#define Jpeg_PlayModeNums 2
#define Language_MenuNums 17

#ifdef Contrast_Set
#define SettingMenuNums 5
#else
#define SettingMenuNums 4
#endif
//
#define SetupMenuTpye 41
#define LanguageMenuTpye 42
#define MemeryTitleType 43
#define FWVerTitleType 44

#ifdef Contrast_Set
#define ContrastTitleType 45
#endif

code U8 FW_Data[21]={20,0,'2',0,'0',0,'1',0,'0',0,'/',0,'0',0,'8',0,'/',0,'1',0,'6'};
code U8 FW_Ver[17]={16,0,'V',0,'1',0,'.',0,'0',0,'0',0,'.',0,'0',0,'3'};
//-----------------------------------------------------------------------------
//
//tc_MenuType:gc_TuneVolFreqStatus=1:vol adj 2:FM adj 3:play menu 4:eq 5:rep
//Jpeg:31 file select	32 auto or ,manual 	33 play set
//Mjpeg:21 file select	22 rep mode
//----------------------------------------------------------------------------
void Menu_Disp_Item_Other(U8 tc_MenuType,U8 tc_Select,U8 tc_RefreshType)
{
	U8 tc_i;
	U8 tc_ItemNums;
	U8 tc_language=2;
	U16 tw_icon_ID,tw_icon_x,tw_icon_y,tw_icon_Hsize,tw_icon_Vsize;  //ѡ����ǰ��Сͼ�����ʾ�趨
	U16 tw_color_select,tw_color_BG_select,tw_color_Noselect,tw_color_BG_Noselect;//ѡ���������ɫ��δѡ�е���ɫ
	U8 tc_High;//������Ŀ�ļ��
	U16 tw_word_x;//�˵�����ʼ��x
    U8 *(*p);	  //ָ������
	U8 tc_flag_dispItem=0;
	
	tc_language = gc_MenuHZK;

	if(tc_RefreshType==0xff)
	{
		return;
	}
//    dbprintf("\n@1=%bx @2=%bx @3=%bx\n",tc_MenuType,tc_Select,tc_RefreshType);
	switch(tc_MenuType)
	{
	/*	case 1:	//ˢ���������ñ���������
			LCM_set_view(TFT_IconDispDir,0,0,TFT_H_Size,TFT_V_Size);
			LCD_DisplayIcon(ID_PUBLIC_FULL,OFFSET_VOLSET_BG);  	 

			Menu_Disp_String(Voice_Item_X,Voice_Item_Y,VolORFMTitleTable[0+tc_language*VolORFMTitleNums],Voice_Item_WordColor,Voice_Item_WordColor_BG);
			tc_flag_dispItem=1;
			break;
		case 2://ˢ��FM���ñ���������
			LCM_set_view(TFT_IconDispDir,0,0,TFT_H_Size,TFT_V_Size);
			LCD_DisplayIcon(ID_PUBLIC_FULL,OFFSET_FMSET_BG);  	 
			
			Menu_Disp_String(FM_ModeItem_X,FM_ModeItem_Y,VolORFMTitleTable[1+tc_language*VolORFMTitleNums],FM_ModeItem_WordColor,FM_ModeItem_WordColor_BG);
			tc_flag_dispItem=1;
			break;	   	 */
		case 21://mjpeg menu
		case 31://jpeg
		case 41://setting menu

			if(tc_MenuType==21)
			{
				tc_ItemNums = MjpegMenuItemNums;
				p=MjpegMenuTable;  	
			}else if(tc_MenuType==31)
			{
			 	tc_ItemNums = JpegMenuItemNums;
				p=JPEGMenuTable; 					
				
			}else if(tc_MenuType==41)
			{
				tc_ItemNums = SettingMenuNums;
				p=SettingMenuTable; 
			}
			if(tc_RefreshType == REFRESH_ALL)
			{
				LCM_set_view(TFT_IconDispDir,0,0,TFT_H_Size,TFT_V_Size);
				LCD_DisplayIcon(ID_PUBLIC_FULL,OFFSET_MENU_1_BG);  	 //ˢ�±���
			}	
			tw_icon_x = 0;
			tw_icon_y = ID_SelectOrNot_1_Y_Pos;
			tw_icon_Hsize = SelectOrNot1_H_Size;
			tw_icon_Vsize = SelectOrNot1_V_Size;
			tw_icon_ID = ID_SelectOrNot_1;
			tc_High = 24;
			tw_word_x = 20;
			tw_color_select = WordColor_MenuItem1_Select;
			tw_color_BG_select = WordColor_MenuItem1_BG;
			tw_color_Noselect =	WordColor_MenuItem1_NOSelect;
			tw_color_BG_Noselect = WordColor_MenuItem1_BG;		
			break;
		case 22://Mjpeg rep menu
		case 32://jpeg playmode menu 
			if(tc_MenuType == 22)
			{	
				tc_ItemNums = Mjpeg_RepMenuItemNums;
				p=MJPEG_REPTable;    
				if(tc_RefreshType == REFRESH_ALL)
				{
					LCM_set_view(TFT_IconDispDir,0,0,TFT_H_Size,TFT_V_Size);
					LCD_DisplayIcon(ID_PUBLIC_FULL,OFFSET_MENU_2_BG);  	 //ˢ�±���	 
	
					Menu_Disp_String(2,4,MjpegMenuTable[1+tc_language*MjpegMenuItemNums],WordColor_MenuItem2_Title,WordColor_MenuItem2_Title_BG);
				}
			}else
			{
				tc_ItemNums = Jpeg_PlayModeNums;
				p=JPEG_PlayModeTable;    
				if(tc_RefreshType == REFRESH_ALL)
				{
					LCM_set_view(TFT_IconDispDir,0,0,TFT_H_Size,TFT_V_Size);
					LCD_DisplayIcon(ID_PUBLIC_FULL,OFFSET_MENU_2_BG);  	 //ˢ�±���	 
	
					Menu_Disp_String(2,4,JPEGMenuTable[1+tc_language*JpegMenuItemNums],WordColor_MenuItem2_Title,WordColor_MenuItem2_Title_BG);
				} 
			}
			
			tw_icon_x = 0;
			tw_icon_y = ID_SelectOrNot_2_Y_Pos;
			tw_icon_Hsize = SelectOrNot2_H_Size;
			tw_icon_Vsize = SelectOrNot2_V_Size;
			tw_icon_ID = ID_SelectOrNot_2;
			tc_High = ID_SelectOrNot_BaseOffset;//20;
			tw_word_x = 20;
			tw_color_select = WordColor_MenuItem2_Select;
			tw_color_BG_select = WordColor_MenuItem2_BG;
			tw_color_Noselect =	WordColor_MenuItem2_NOSelect;
			tw_color_BG_Noselect = WordColor_MenuItem2_BG;		
			
			break;
		case 33: //jpeg Play set
			 tc_flag_dispItem=1;
			break;
/////////////////////////setting menu//////////////
		case 42://language menu
			
			if(tc_RefreshType == REFRESH_ALL)
			{
				LCM_set_view(TFT_IconDispDir,0,0,TFT_H_Size,TFT_V_Size);
				LCD_DisplayIcon(ID_PUBLIC_FULL,OFFSET_MENU_2_BG);  	 //ˢ�±���	 

				Menu_Disp_String(2,4,SettingMenuTable[0+tc_language*SettingMenuNums],WordColor_MenuItem2_Title,WordColor_MenuItem2_Title_BG);
			} 
			#if ((defined TFT_18H) || (defined TFT_15H))
			if(((tc_RefreshType == REFRESH_PREV)&&(tc_Select==5 || tc_Select==11 || tc_Select==16))||\
			   ((tc_RefreshType == REFRESH_NEXT)&&(tc_Select==0 || tc_Select==6  || tc_Select==12)) )
			{	
				LCM_set_view(TFT_IconDispDir,0,0,TFT_H_Size,TFT_V_Size);
				LCD_DisplayIcon(ID_PUBLIC_FULL,OFFSET_MENU_2_BG);  	 //ˢ�±���	 

				Menu_Disp_String(2,4,SettingMenuTable[0+tc_language*SettingMenuNums],WordColor_MenuItem2_Title,WordColor_MenuItem2_Title_BG);
				tc_RefreshType=REFRESH_ALL;	
			} 
			if((tc_Select >= 0)&&(tc_Select <= 5))// ������Ե�һ������
			{
				tc_ItemNums=6;
				p=&Language_table[0];				
			}else if((tc_Select >= 6)&&(tc_Select <= 11))// ������Եڶ�������
			{
				tc_ItemNums=6;
				p=&Language_table[6];
				tc_Select-=6;
			}else if((tc_Select >= 12)&&(tc_Select <= 16)) // ������Ե���������
			{
				tc_ItemNums=5;
				p=&Language_table[12];
				tc_Select-=12;
			}    
			#elif defined TFT_18V	
				if (((tc_RefreshType == REFRESH_PREV)&&(tc_Select==6 || tc_Select==13 || tc_Select==16))||\
			   		((tc_RefreshType == REFRESH_NEXT)&&(tc_Select==0 || tc_Select==7  || tc_Select==14)) )
			{	
				LCM_set_view(TFT_IconDispDir,0,0,TFT_H_Size,TFT_V_Size);
				LCD_DisplayIcon(ID_PUBLIC_FULL,OFFSET_MENU_2_BG);  	 //ˢ�±���	 

				Menu_Disp_String(2,4,SettingMenuTable[0+tc_language*SettingMenuNums],WordColor_MenuItem2_Title,WordColor_MenuItem2_Title_BG);
				tc_RefreshType=REFRESH_ALL;	
			}
			if((tc_Select >= 0)&&(tc_Select <= 6))// ������Ե�һ������
			{
				tc_ItemNums=7;
				p=&Language_table[0];				
			}else if((tc_Select >= 7)&&(tc_Select <= 13))// ������Եڶ�������
			{
				tc_ItemNums=7;
				p=&Language_table[7];
				tc_Select-=7;
			}else if((tc_Select >= 14)&&(tc_Select <= 16)) // ������Ե���������
			{
				tc_ItemNums=3;
				p=&Language_table[14];
				tc_Select-=14;
			}  			
			#endif
			tc_language=0;//������Բ˵��޶������ʾ
//-

			tw_icon_x = 0;
			tw_icon_y = ID_SelectOrNot_2_Y_Pos;
			tw_icon_Hsize = SelectOrNot2_H_Size;
			tw_icon_Vsize = SelectOrNot2_V_Size;
			tw_icon_ID = ID_SelectOrNot_2;
			tc_High = ID_SelectOrNot_BaseOffset;//20;
			tw_word_x = 20;
			tw_color_select = WordColor_MenuItem2_Select;
			tw_color_BG_select = WordColor_MenuItem2_BG;
			tw_color_Noselect =	WordColor_MenuItem2_NOSelect;
			tw_color_BG_Noselect = WordColor_MenuItem2_BG;
			break;
			 
		case FWVerTitleType://44: // FW ver 
			if(tc_RefreshType == REFRESH_ALL)
			{
				LCM_set_view(TFT_IconDispDir,0,0,TFT_H_Size,TFT_V_Size);
				LCD_DisplayIcon(ID_PUBLIC_FULL,OFFSET_FWVer_BG);  	 //ˢ�±���	 

				Menu_Disp_String(FW_Ver_Start_X,FW_Ver_Start_Y,FW_Ver,WordColor_MenuItem2_Title,WordColor_MenuItem2_Title_BG);
				Menu_Disp_String(FW_Data_Start_X,FW_Data_Start_Y,FW_Data,WordColor_MenuItem2_Title,WordColor_MenuItem2_Title_BG);
			}
			tc_flag_dispItem=1;
			break;
#ifdef Contrast_Set
		case ContrastTitleType://45 // �Աȶ�
			if(tc_RefreshType == REFRESH_ALL)
			{
				LCM_set_view(TFT_IconDispDir,0,0,TFT_H_Size,TFT_V_Size);
				LCD_DisplayIcon(ID_PUBLIC_FULL,OFFSET_MENU_1_BG);  	 //ˢ�±���	 

				Menu_Disp_String(FM_ModeItem_X,FM_ModeItem_Y,SettingMenuTable[1+tc_language*SettingMenuNums],FM_ModeItem_WordColor,FM_ModeItem_WordColor_BG);
			}
			tc_flag_dispItem=1;
			break;
#endif
		case 60://�ļ�ѡ��
			if(tc_RefreshType == REFRESH_ALL)
			{
				LCM_set_view(TFT_IconDispDir,0,0,TFT_H_Size,TFT_V_Size);
				LCD_DisplayIcon(ID_PUBLIC_FULL,OFFSET_MENU_2_BG);  	 //ˢ�±���	 
	
				Menu_Disp_String(2,4,JPEGMenuTable[0+tc_language*JpegMenuItemNums],WordColor_MenuItem2_Title,WordColor_MenuItem2_Title_BG);
			}
			tc_flag_dispItem=1;
			break;	
		default: 
			break;
	}
	if(tc_flag_dispItem)
	{  // not disp item 
		tc_flag_dispItem=0;
		return;
	}
	for(tc_i=0;tc_i<tc_ItemNums;tc_i++)
	{
		if(tc_i == tc_Select)
		{
			LCM_set_view(TFT_IconDispDir,tw_icon_x,tw_icon_y+tc_High*tc_i,tw_icon_Hsize,tw_icon_Vsize);//��ʾѡ�е�Сͼ��
            LCD_DisplayIcon(tw_icon_ID,Select_Offset);
        
            Menu_Disp_String(tw_word_x,tw_icon_y+tc_High*tc_i,p[tc_i+tc_language*tc_ItemNums],tw_color_select,tw_color_BG_select);		
		}else
		{
			if(tc_RefreshType == REFRESH_ALL)
            {  //ȫ����Ŀ��ˢ��
                LCM_set_view(TFT_IconDispDir,tw_icon_x,tw_icon_y+tc_High*tc_i,tw_icon_Hsize,tw_icon_Vsize); //��ʾδѡ�е�Сͼ��
                LCD_DisplayIcon(tw_icon_ID,SelectNot_Offset);   
         
                Menu_Disp_String(tw_word_x,tw_icon_y+tc_High*tc_i,p[tc_i+tc_language*tc_ItemNums],tw_color_Noselect,tw_color_BG_Noselect);  
            }else if(tc_RefreshType == REFRESH_NEXT)
            {  //ֻˢ�µ�ǰѡ����Ŀ����ǰ��Ŀ����һ��  
                if((tc_i+1 == tc_Select) || (tc_Select == 0))
                {
                    LCM_set_view(TFT_IconDispDir,tw_icon_x,tw_icon_y+tc_High*tc_i,tw_icon_Hsize,tw_icon_Vsize);   //��ʾδѡ�е�Сͼ��
                    LCD_DisplayIcon(tw_icon_ID,SelectNot_Offset);   
         
                    Menu_Disp_String(tw_word_x,tw_icon_y+tc_High*tc_i,p[tc_i+tc_language*tc_ItemNums],tw_color_Noselect,tw_color_BG_Noselect);   
                 }
            }else if(tc_RefreshType == REFRESH_PREV)
            {  //ֻˢ�µ�ǰѡ����Ŀ����ǰ��Ŀ����һ��
                if((tc_i-1 == tc_Select) || (tc_Select == tc_ItemNums-1))
                {
                    LCM_set_view(TFT_IconDispDir,tw_icon_x,tw_icon_y+tc_High*tc_i,tw_icon_Hsize,tw_icon_Vsize);   //��ʾδѡ�е�Сͼ��
                    LCD_DisplayIcon(tw_icon_ID,SelectNot_Offset);   
                    
                    Menu_Disp_String(tw_word_x,tw_icon_y+tc_High*tc_i,p[tc_i+tc_language*tc_ItemNums],tw_color_Noselect,tw_color_BG_Noselect);  
                }
                
            } 
		}
	}
	gc_refresh_type=0xff;	
		
}



