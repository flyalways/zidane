

//tt

/*============================================================
U32 gdw_Display_MSG低24bit对应图片分布表
0	Clear All				//清屏
1	DispAll_RecTime 		//显示所有的录音时间
2	Disp_RecTime_On			//显示当前设置项
3   Disp_RecTime_Off		//清除当前设置项
4	Menu Display			//菜单显示
5	Capacity				//容量查询
6	FW edition				//固件版本
7	

8	
9			
10	
11	
12	
13	
14	
15	
	
16	
17	
18			
19	
20
21
22
23
//如有更多的图片需要显示请从这儿继续添加,共有32位	

//===============================================================*/

static void Display_Function0(U8 tc_FuncIndex)
{
//	dbprintf("0 %bx\n",tc_FuncIndex);
	switch(tc_FuncIndex)
	{
		case 0:
			//清屏
			break;
		case 1:
			//显示所有的录音时间
			LCD_Disp_All_RecTime();
			break;
		case 2:
			//显示当前设置项

			LCD_Disp_Rectime_Item_OnorOff(1);
			break;
		case 3:
			//清除当前设置项
			LCD_Disp_Rectime_Item_OnorOff(0);
			break;
		case 4:
			//菜单显示
			LCD_Disp_SettingMenu(gc_Menu_State);
			break;
		case 5:
			//容量显示
//			dbprintf("cap disp");
//			LCD_Disp_Memory_Menu();
			break;
		case 6:
			//固件版本
//			dbprintf("FWVersion disp");
			LCD501_Disp_FWVersion();
			break;

		default:
			break;
					
	}
}

static void Display_Function1(U8 tc_FuncIndex)
{
//	dbprintf("1 %bx\n",tc_FuncIndex);
	//如有更多的图片需要显示可以从这儿继续添加
	switch(tc_FuncIndex)
	{
		default:
			break;
	}
	return;

}

static void Display_Function2(U8 tc_FuncIndex)
{
//	dbprintf("2 %bx\n",tc_FuncIndex);
	//如有更多的图片需要显示可以从这儿继续添加
	switch(tc_FuncIndex)
	{
		default:
			break;
	}
	return;
}

static void Display_Function3(U8 tc_FuncIndex)
{
	//如有更多的图片需要显示可以从这儿继续添加
	switch(tc_FuncIndex)
	{
		default:
			break;
	}
	return;
}

