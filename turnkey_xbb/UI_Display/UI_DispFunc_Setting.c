

//tt

/*============================================================
U32 gdw_Display_MSG��24bit��ӦͼƬ�ֲ���
0	Clear All				//����
1	DispAll_RecTime 		//��ʾ���е�¼��ʱ��
2	Disp_RecTime_On			//��ʾ��ǰ������
3   Disp_RecTime_Off		//�����ǰ������
4	Menu Display			//�˵���ʾ
5	Capacity				//������ѯ
6	FW edition				//�̼��汾
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
//���и����ͼƬ��Ҫ��ʾ�������������,����32λ	

//===============================================================*/

static void Display_Function0(U8 tc_FuncIndex)
{
//	dbprintf("0 %bx\n",tc_FuncIndex);
	switch(tc_FuncIndex)
	{
		case 0:
			//����
			break;
		case 1:
			//��ʾ���е�¼��ʱ��
			LCD_Disp_All_RecTime();
			break;
		case 2:
			//��ʾ��ǰ������

			LCD_Disp_Rectime_Item_OnorOff(1);
			break;
		case 3:
			//�����ǰ������
			LCD_Disp_Rectime_Item_OnorOff(0);
			break;
		case 4:
			//�˵���ʾ
			LCD_Disp_SettingMenu(gc_Menu_State);
			break;
		case 5:
			//������ʾ
//			dbprintf("cap disp");
//			LCD_Disp_Memory_Menu();
			break;
		case 6:
			//�̼��汾
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
	//���и����ͼƬ��Ҫ��ʾ���Դ�����������
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
	//���и����ͼƬ��Ҫ��ʾ���Դ�����������
	switch(tc_FuncIndex)
	{
		default:
			break;
	}
	return;
}

static void Display_Function3(U8 tc_FuncIndex)
{
	//���и����ͼƬ��Ҫ��ʾ���Դ�����������
	switch(tc_FuncIndex)
	{
		default:
			break;
	}
	return;
}

