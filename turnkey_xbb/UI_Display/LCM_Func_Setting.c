
static U8 code Menu_Combination_Setting[][2]=	
{								
	//Setting one level
	MID_SYSY_SETTING,	0,    /*0x0a Set_RecTime     */
	MID_SYSY_SETTING,	1,    /*0x0b Set_BL_Time     */
	MID_SYSY_SETTING,	2,    /*0x0c Set_BL_Mode     */
	MID_SYSY_SETTING,	2,    /*0x0d Set_Language    */
	MID_SYSY_SETTING,	3,    /*0x0e Set_IdleTime    */
	MID_SYSY_SETTING,	4,    /*0x0f Set_Contrast    */
	MID_SYSY_SETTING,	5,    /*0x10 Set_On_Logo     */ 
	MID_SYSY_SETTING,	6,    /*0x11 Set_Off_Logo    */
	MID_SYSY_SETTING,	7,    /*0x12 Set_Cap         */
	MID_SYSY_SETTING,	8,    /*0x13 Set_DEFAULT     */
	MID_SYSY_SETTING,	9,   /*0x14 Set_FW_Ver      */
	MID_SYSY_SETTING,	10,   /*0x15 Set_LED       */
 	MID_SYSY_SETTING,	11,   /*0x16 Set_Exit       */
	MID_SYSY_SETTING,	12,   /*0x17 Set_LED_ON        */
	MID_SYSY_SETTING,	13,   /*0x18 Set_LED_OFF        */

	//Setting rectime
	MID_RecTime,		0,    /*0x17 Mod_RecTime     */
	MID_RecTime,		0,    /*0x18 Mod_RecTime     */
	MID_RecTime,		0,    /*0x19 Mod_RecTime     */
	MID_RecTime,		0,    /*0x1A Mod_RecTime     */
	MID_RecTime,		0,    /*0x1B Mod_RecTime     */
	MID_RecTime,		0,    /*0x1C Mod_RecTime     */
	MID_RecTime,		0,    /*0x1D Mod_RecTime     */
	//Setting BL set
	MID_BL_Time_SET,	0,	  /*0x1E Long_BL_Time    */				
	MID_BL_Time_SET,	1,	  /*0x1F Short_BL_Time   */				
	//Setting BL time level
	MID_BL_Time_Level,	0,    /*0x20 Mod_BL_Time00   */
	MID_BL_Time_Level,	1,    /*0x21 Mod_BL_Time01   */
	MID_BL_Time_Level,	2,    /*0x22 Mod_BL_Time02   */
	MID_BL_Time_Level,	3,    /*0x23 Mod_BL_Time03   */
	MID_BL_Time_Level,	4,    /*0x24 Mod_BL_Time04   */
	MID_BL_Time_Level,	5,    /*0x25 Mod_BL_Time05   */ 
	MID_BL_Time_Level,	6,    /*0x26 Mod_BL_Time06   */ 
	MID_BL_Time_Level,	7,    /*0x27 Mod_BL_Time07   */
	MID_BL_Time_Level,	8,    /*0x28 Mod_BL_Time08   */
	MID_BL_Time_Level,	9,    /*0x29 Mod_BL_Time09   */
	MID_BL_Time_Level,	10,   /*0x2A Mod_BL_Time09   */

	//Setting BL type 
	MID_BL_Type,		0,    /*0x2B Mod_BL_Mode00   */
	MID_BL_Type,		1,    /*0x2C Mod_BL_Mode01   */
	MID_BL_Type,		2,    /*0x2D Mod_BL_Mode02   */
	MID_BL_Type,		3,    /*0x2E Mod_BL_Mode03   */
	MID_BL_Type,		4,    /*0x2F Mod_BL_Mode04   */
	MID_BL_Type,		5,    /*0x30 Mod_BL_Mode05   */
	MID_BL_Type,		6,    /*0x31 Mod_BL_Mode06   */
	MID_BL_Type,		7,    /*0x32 Mod_BL_Mode07   */

	MID_Language,		0,    /*0x33 Mod_Language00  */
	MID_Language,		1,    /*0x34 Mod_Language01  */
	MID_Language,		2,    /*0x35 Mod_Language02  */
	MID_Language,		3,    /*0x36 Mod_Language03  */
	MID_Language,		4,    /*0x37 Mod_Language04  */
	MID_Language,		5,    /*0x38 Mod_Language05  */
	MID_Language,		6,    /*0x39 Mod_Language06  */
	MID_Language,		7,    /*0x3A Mod_Language07  */
	MID_Language,		8,    /*0x3B Mod_Language08  */
	MID_Language,		9,    /*0x3C Mod_Language09  */
	MID_Language,		10,   /*0x3D Mod_Language10  */
	MID_Language,		11,   /*0x3E Mod_Language11  */
	MID_Language,		12,   /*0x3F Mod_Language12  */
	MID_Language,		13,   /*0x40 Mod_Language13  */
	MID_Language,		14,   /*0x41 Mod_Language14  */
	MID_Language,		15,   /*0x42 Mod_Language15  */


	MID_SET_IdleTime,	0,    /*0x43 Set_SavePower   */
	MID_SET_IdleTime,	1,    /*0x44 Set_SleepMod    */

	MID_SAVE_Power,		0,    /*0x45 SavePower00	 */
	MID_SAVE_Power,		1,    /*0x46 SavePower01	 */
	MID_SAVE_Power,		2,    /*0x47 SavePower02	 */
	MID_SAVE_Power,		3,    /*0x48 SavePower03	 */
	MID_SAVE_Power,		4,    /*0x49 SavePower04	 */
	MID_SAVE_Power,		5,    /*0x4A SavePower05	 */
	MID_SAVE_Power,		6,    /*0x4B SavePower06	 */

	MID_SleepMod,		0,    /*0x4C SleepMod00	     */
	MID_SleepMod,		1,    /*0x4D SleepMod01	     */
	MID_SleepMod,		2,    /*0x4E SleepMod02	     */
	MID_SleepMod,		3,    /*0x4F SleepMod03	     */
	MID_SleepMod,		4,    /*0x50 SleepMod04	     */
	MID_SleepMod,		5,    /*0x51 SleepMod05	     */
	MID_SleepMod,		6,    /*0x52 SleepMod06	     */
	MID_SleepMod,		7,    /*0x53 SleepMod07	     */
	MID_SleepMod,		8,    /*0x54 SleepMod08	     */
	MID_SleepMod,		9,    /*0x55 SleepMod09	     */
	MID_SleepMod,		10,   /*0x56 SleepMod10	     */
	MID_SleepMod,		11,   /*0x57 SleepMod11	     */
	MID_SleepMod,		12,   /*0x58 SleepMod12	     */

	MID_Contrast,		0,    /*0x59 Contrast00	     */
	MID_Contrast,		1,    /*0x5A Contrast01	     */
	MID_Contrast,		2,    /*0x5B Contrast02	     */
	MID_Contrast,		3,    /*0x5C Contrast03	     */
	MID_Contrast,		4,    /*0x5D Contrast04	     */
	MID_Contrast,		5,    /*0x5E Contrast05	     */
	MID_Contrast,		6,    /*0x5F Contrast06	     */
	MID_Contrast,		7,    /*0x60 Contrast07	     */
	MID_Contrast,		8,    /*0x61 Contrast08	     */
	MID_Contrast,		9,    /*0x62 Contrast09	     */
	MID_Contrast,		10,   /*0x63 Contrast10	     */
	MID_Contrast,		11,   /*0x64 Contrast11	     */
	MID_Contrast,		12,   /*0x65 Contrast12	     */
	MID_Contrast,		13,   /*0x66 Contrast13	     */		
	MID_Contrast,		14,   /*0x67 Contrast14	     */		
	MID_Contrast,		15,   /*0x68 Contrast15	     */		


	MID_Logo_On,		0,    /*0x69 Logo_On_Mod00   */
	MID_Logo_On,		1,    /*0x6A Logo_On_Mod01   */

	MID_Logo_Off,		0,    /*0x6B Logo_Off_Mod00  */
	MID_Logo_Off,		1,    /*0x6C Logo_Off_Mod01  */

	MID_Cap,			0,    /*0x6D Mod_Cap		 */
	MID_FW_Ver,			0,    /*0x6E Mod_FW_Ver	     */
	MID_FW_UG,			0,    /*0x6F Mod_FW_UG	     */
};


static void LCD_Disp_LargNumber_Rec(/*U8 tc_Page,U8 tc_Column,U8* tpc_DataPoint, U8 tc_DataLength,U8 tc_OnorOff*/) //**Play_ //_PLAY_&REC //show the time
{
}


static void LCD_Display_REC_SetTime(U8 DisPlayType, U8 tc_OnorOff)//ym add
{
	DisPlayType = DisPlayType;
	tc_OnorOff = tc_OnorOff;
}


static void LCD_Disp_All_RecTime()
{
}

static void LCD_Disp_Rectime_Item_OnorOff(U8 tc_OnorOff)
{
	tc_OnorOff = tc_OnorOff;
}

static void LCD_Disp_SettingMenu(U8 SubState)
{
	SubState = SubState;
}

extern void GetMyParaData(U8 *DataBuf,U8 tc_GetLen,U8 tc_Para_Offset);//huiling.gu add 081016

static void LCD501_Disp_FWVersion()//_Menu_
{
	U8 FW_VERSION[17];
	GetMyParaData(FW_VERSION,FW_VERSION_LEN,FW_VERSION_OFFSET);//huiling.gu add 081016	

	// add display functions here ...
}


