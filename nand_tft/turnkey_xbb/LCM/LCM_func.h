#include "..\LCM\LCM.h"
//#include "..\LCM\coordinate.h"
#ifndef ____DDDD___
#define ____DDDD___
enum MenuSmallIcon{
		D_SmallIcon_Music,
		D_SmallIcon_Voice,
		D_SmallIcon_Movie,
		D_SmallIcon_Photo,
		D_SmallIcon_EBook,
		D_SmallIcon_Dir,
		D_SmallIcon_Dark
};
#endif
//==============================================================
//=========================LCM Driver===========================
void TFT_init(void);
extern void LCD_set_contrast(U8 contrast);
extern void USER_ReadReserveBlock(U16 tw_ReserveBLKSector);
extern void LCD_DisplayIcon(U8 tw_IconIDNumber,U8 tc_CurrentOffset);
extern void User_ReadReserveBlockData_To_TFT(U32 tdw_StartByteAddr,U32 tdw_ByteNum);
void LCD_Write_Command(U8 tc_Data);
void LCD_Write_OneData(U8 tc_Data);
void LCD_Write_OneWord(U16 w);

extern void LCD_Data2NandMode(void);
extern void LCD_Nand2DataMode(void);
extern void LCM_set_view(U8 view_type, U16 x_start, U16 y_start, U16 x_size, U16 y_size);
#define LCD_set_view	LCM_set_view

//==============================================================
//========================LCM Function==========================
extern void LCD_Init(void);
extern void LCD_print_PowerOnlogo(void);
//---------------------------Common-----------------------------
extern void LCD_disp_HZKCharBMP(U8 Page, U8 Column,U8 *BMPdataBuf, U8 CharOrWord,U8 reverse);
extern U8 LCD_disp_HZK_string(/*U8 Page,U8 *DataBuf,U8 tc_ISNOrUnicode, U8 nByte,U8 ColumnOffset,bit reverse*/);
extern void LCD_Disp_LittleChar(U8 CharOffset);

extern U8 LCD_Disp_FileName(U8 *tpc_DataBuf, U8 tc_ANSI, U8 tc_nByte, U8 tc_scroll_flag);

extern U8 UI_get_string_hzk_bitmap(struct string_view_t * p_disp_param,
								   struct string_disp_t xdata * p_string_disp_param,
								   U8 * tpc_DataBuf,
								   U8 tc_nByte,
								   U8 tc_ANSI,
								   U8 line_num,
								   U8 font_truncate_flag,
								   U8 tc_scroll_flag, //rolling factor
								   U8 xdata * p_bitmap);
extern void UI_render_strings(struct string_view_t * p_disp_param,
							  struct string_disp_t xdata * p_string_disp_param);

//-----------------------------USB------------------------------
extern void LCD_Disp_PCConnect();
extern void LCD_Disp_PC();
extern void LCD_Disp_MyMP3Logo();
extern void LCD_Disp_USBLine();
extern void LCD_Disp_XBMPArray_USB(U8 *Condition,U8 *Data);
extern void LCD_Disp_USBRWLogo(U8 Condition);	//USB 0:(Read); 1:(Write)
extern void LCD_Disp_FW_UG();
//------------------------play idle voice-----------------------
extern void LCD_FillBlank(U8 Page,U8 Column,U8 Width,U8 Height);
extern void LCD_Clear_DisplayBug();
extern void LCD_Disp_Menu_Idle(U8 SubState);
extern void Menu_Disp_Icon_PlayMenu(UBYTE tc_Menu_State);
extern void LCD_Disp_DosError();

//-----------------------------menu------------------------------
extern void LCD_Disp_LargNumber(U8 tc_Page,U8 tc_Column,U8* tpc_DataPoint, U8 tc_DataLength,U8 tc_OnorOff);
extern void LCD_Display_REC_SetTime(U8 DisPlayType, U8 tc_OnorOff);
extern void LCD_Disp_Menu(U8 SubState);
//------------------------------REC------------------------------
extern void LCD_DispLitIcon_REC(U8 column);
extern void LCD_Disp_Initialing();
extern void LCD_Disp_RecTotalTime(U32 LCD_SecondNum);
extern U32 User_GetFreeMemorySize_REC();

