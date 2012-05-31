/*
#define BYTE_0(x)  (*((unsigned char*)&x+3))
#define BYTE_1(x)  (*((unsigned char*)&x+2))
#define BYTE_2(x)  (*((unsigned char*)&x+1))
#define BYTE_3(x)  (*((unsigned char*)&x))
*/

static void UI_Display_Func( U32 tdw_display_msg)
{
	U8 tc_3_FuncIndex=0;//��3��8bit��Ϣ���ܺ���������
	U8 tc_2_FuncIndex=0; //��2��8bit��Ϣ���ܺ���������			  
	U8 tc_1_FuncIndex=0;//��1��8bit��Ϣ���ܺ���������
	U8 tc_0_FuncIndex=0; //��0��8bit��Ϣ���ܺ���������
//	tdw_display_msg = tdw_display_msg&CLEAR_MSG_FLAG;//CLEAR_MSG_FLAG��ʾ���һЩ�ض������һ������Ҫ��ʾͼƬ,���ݲ�ͬ��define��ȡ��Ӧֵ.
//	dbprintf("tdw_display_msg====%lx\n",tdw_display_msg);
	while(tdw_display_msg!=0) //�ж�����Ϣ�Ƿ�Ϊ�ա���Ϊ�գ�����
	{

		while(BYTE_0(tdw_display_msg))//�жϵ�8bit���Ƿ�����Ϣ���������
		{
			if(BYTE_0(tdw_display_msg)&0x1) //�����bit��־����Ϣ
			{
				Display_Function0(tc_0_FuncIndex);				
			}
			else
			{
				//do nothing
			}
			BYTE_0(tdw_display_msg) >>= 1;	 //��ձ�bit�ı�־��׼���ж�ǰ1bit����Ϣ
			tc_0_FuncIndex++; //���ܺ��������ŵ���			
		}
		


		//���µĸ�8bit����ʽ���8bit��ͬ

		while(BYTE_1(tdw_display_msg))//�жϸ�8bit���Ƿ�����Ϣ���������
		{
			if(BYTE_1(tdw_display_msg)&0x1)
			{
				Display_Function1(tc_1_FuncIndex);				
			}
			else
			{
				//do nothing
			}	
			BYTE_1(tdw_display_msg) >>= 1; 	
			tc_1_FuncIndex++;
		}

		while(BYTE_2(tdw_display_msg))
		{
			if(BYTE_2(tdw_display_msg)&0x1)
			{
				Display_Function2(tc_2_FuncIndex);				
			}
			else
			{
				//do nothing
			}	
			BYTE_2(tdw_display_msg) >>= 1; 	
			tc_2_FuncIndex++;
		}

		while(BYTE_3(tdw_display_msg))
		{
			if(BYTE_3(tdw_display_msg)&0x1)
			{
				Display_Function3(tc_3_FuncIndex);				
			}
			else
			{
				//do nothing
			}	
			BYTE_3(tdw_display_msg) >>= 1; 	
			tc_3_FuncIndex++;
		}
	}
}