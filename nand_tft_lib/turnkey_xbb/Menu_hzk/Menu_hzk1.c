#include "..\Header\SPDA2K.h"
#include "..\LCM\model_define.h"//(JC)H1124
#include "..\header\variables.h"

#include "..\LCM\TFT_config.h"		// TFT
#include "..\Menu_hzk\MenuDisp_string.c"
#include  "..\Menu_hzk\String_HZK1.c"

//------------------该bank中放了音乐菜单及音量。FM选择、选取界面的字符串----
//-----------------显示字符串的函数请放在该文件中，供其他部分调用

#define PlayMenuItemNums 5
#define EQMenuItemNums 6
#define REPMenuItemNums 3
#define VolORFMTitleNums 3
//-----------------------------------------------------------------------------
//
//tc_MenuType:gc_TuneVolFreqStatus=1:vol adj 2:FM adj 3:play menu 4:eq 5:rep
//
//----------------------------------------------------------------------------
void Menu_Disp_Item_Play(U8 tc_MenuType,U8 tc_Select,U8 tc_RefreshType)
{
	U8 tc_i;
	U8 tc_ItemNums;
	U8 tc_language;
	U16 tw_icon_ID,tw_icon_x,tw_icon_y,tw_icon_Hsize,tw_icon_Vsize;  //选中项前的小图标的显示设定
	U16 tw_color_select,tw_color_BG_select,tw_color_Noselect,tw_color_BG_Noselect;//选中字体的颜色和未选中的颜色
	U8 tc_High;//两个项目的间隔
	U16 tw_word_x;//菜单字起始的x
    U8 *(*p);	  //指针数组
	U8 tc_flag_dispItem=0;

	tc_language= gc_MenuHZK;
	    
//	dbprintf("\n#1=%bx #2=%bx #3=%bx\n",tc_MenuType,tc_Select,tc_RefreshType);

	if(tc_RefreshType==0xff)
	{
		return;
	}

	switch(tc_MenuType)
	{
		case 1:	//刷新音量设置背景及文字
			LCM_set_view(TFT_IconDispDir,0,0,TFT_H_Size,TFT_V_Size);
			LCD_DisplayIcon(ID_PUBLIC_FULL,OFFSET_VOLSET_BG);  	 

			Menu_Disp_String(Voice_Item_X,Voice_Item_Y,VolORFMTitleTable[0+tc_language*VolORFMTitleNums],Voice_Item_WordColor,Voice_Item_WordColor_BG);
			tc_flag_dispItem=1;
			break;
		case 2://刷新FM设置背景及文字
			LCM_set_view(TFT_IconDispDir,0,0,TFT_H_Size,TFT_V_Size);
			LCD_DisplayIcon(ID_PUBLIC_FULL,OFFSET_FMSET_BG);  	 
			
			Menu_Disp_String(FM_ModeItem_X,FM_ModeItem_Y,VolORFMTitleTable[1+tc_language*VolORFMTitleNums],FM_ModeItem_WordColor,FM_ModeItem_WordColor_BG);
			tc_flag_dispItem=1;
			break;
		case 3://play menu
			tc_ItemNums = PlayMenuItemNums;
			p=PlayMenuTable;  
			
			if(tc_RefreshType == REFRESH_ALL)
			{
				LCM_set_view(TFT_IconDispDir,0,0,TFT_H_Size,TFT_V_Size);
				LCD_DisplayIcon(ID_PUBLIC_FULL,OFFSET_MENU_1_BG);  	 //刷新背景
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
		case 4://eq menu
		case 5://rep menu 
			if(tc_MenuType == 4)
			{	
				tc_ItemNums = EQMenuItemNums;
				p=EQMenuTable;    
				if(tc_RefreshType == REFRESH_ALL)
				{
					LCM_set_view(TFT_IconDispDir,0,0,TFT_H_Size,TFT_V_Size);
					LCD_DisplayIcon(ID_PUBLIC_FULL,OFFSET_MENU_2_BG);  	 //刷新背景	 
	
					Menu_Disp_String(2,4,PlayMenuTable[2+tc_language*PlayMenuItemNums],WordColor_MenuItem2_Title,WordColor_MenuItem2_Title_BG);
				}
			}else
			{
				tc_ItemNums = REPMenuItemNums;
				p=REPMenuTable;    
				if(tc_RefreshType == REFRESH_ALL)
				{
					LCM_set_view(TFT_IconDispDir,0,0,TFT_H_Size,TFT_V_Size);
					LCD_DisplayIcon(ID_PUBLIC_FULL,OFFSET_MENU_2_BG);  	 //刷新背景	 
	
					Menu_Disp_String(2,4,PlayMenuTable[3+tc_language*PlayMenuItemNums],WordColor_MenuItem2_Title,WordColor_MenuItem2_Title_BG);
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
		default: 
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
			LCM_set_view(TFT_IconDispDir,tw_icon_x,tw_icon_y+tc_High*tc_i,tw_icon_Hsize,tw_icon_Vsize);//显示选中的小图标
            LCD_DisplayIcon(tw_icon_ID,Select_Offset);
        
            Menu_Disp_String(tw_word_x,tw_icon_y+tc_High*tc_i,p[tc_i+tc_language*tc_ItemNums],tw_color_select,tw_color_BG_select);		
		}else
		{
			if(tc_RefreshType == REFRESH_ALL)
            {  //全部项目都刷新
                LCM_set_view(TFT_IconDispDir,tw_icon_x,tw_icon_y+tc_High*tc_i,tw_icon_Hsize,tw_icon_Vsize); //显示未选中的小图标
                LCD_DisplayIcon(tw_icon_ID,SelectNot_Offset);   
         
                Menu_Disp_String(tw_word_x,tw_icon_y+tc_High*tc_i,p[tc_i+tc_language*tc_ItemNums],tw_color_Noselect,tw_color_BG_Noselect);  
            }else if(tc_RefreshType == REFRESH_NEXT)
            {  //只刷新当前选中项目及当前项目的下一项  
                if((tc_i+1 == tc_Select) || (tc_Select == 0))
                {
                    LCM_set_view(TFT_IconDispDir,tw_icon_x,tw_icon_y+tc_High*tc_i,tw_icon_Hsize,tw_icon_Vsize);   //显示未选中的小图标
                    LCD_DisplayIcon(tw_icon_ID,SelectNot_Offset);   
         
                    Menu_Disp_String(tw_word_x,tw_icon_y+tc_High*tc_i,p[tc_i+tc_language*tc_ItemNums],tw_color_Noselect,tw_color_BG_Noselect);   
                 }
            }else if(tc_RefreshType == REFRESH_PREV)
            {  //只刷新当前选中项目及当前项目的上一项
                if((tc_i-1 == tc_Select) || (tc_Select == tc_ItemNums-1))
                {
                    LCM_set_view(TFT_IconDispDir,tw_icon_x,tw_icon_y+tc_High*tc_i,tw_icon_Hsize,tw_icon_Vsize);   //显示未选中的小图标
                    LCD_DisplayIcon(tw_icon_ID,SelectNot_Offset);   
                    
                    Menu_Disp_String(tw_word_x,tw_icon_y+tc_High*tc_i,p[tc_i+tc_language*tc_ItemNums],tw_color_Noselect,tw_color_BG_Noselect);  
                }
                
            } 
		}
	}
	gc_refresh_type=0xff;	
		
}
