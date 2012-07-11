/*
#define BYTE_0(x)  (*((unsigned char*)&x+3))
#define BYTE_1(x)  (*((unsigned char*)&x+2))
#define BYTE_2(x)  (*((unsigned char*)&x+1))
#define BYTE_3(x)  (*((unsigned char*)&x))
*/

static void UI_Display_Func( U32 tdw_display_msg)
{
	U8 tc_3_FuncIndex=0;//第3个8bit消息功能函数索引号
	U8 tc_2_FuncIndex=0; //第2个8bit消息功能函数索引号			  
	U8 tc_1_FuncIndex=0;//第1个8bit消息功能函数索引号
	U8 tc_0_FuncIndex=0; //第0个8bit消息功能函数索引号
//	tdw_display_msg = tdw_display_msg&CLEAR_MSG_FLAG;//CLEAR_MSG_FLAG表示清除一些特定情况下一定不需要显示图片,根据不同的define获取相应值.
//	dbprintf("tdw_display_msg====%lx\n",tdw_display_msg);
	while(tdw_display_msg!=0) //判断总消息是否为空。不为空，继续
	{

		while(BYTE_0(tdw_display_msg))//判断低8bit中是否有消息，有则继续
		{
			if(BYTE_0(tdw_display_msg)&0x1) //若最低bit标志有消息
			{
				Display_Function0(tc_0_FuncIndex);				
			}
			else
			{
				//do nothing
			}
			BYTE_0(tdw_display_msg) >>= 1;	 //清空本bit的标志，准备判断前1bit的消息
			tc_0_FuncIndex++; //功能函数索引号递增			
		}
		


		//以下的高8bit处理方式与低8bit相同

		while(BYTE_1(tdw_display_msg))//判断高8bit中是否有消息，有则继续
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