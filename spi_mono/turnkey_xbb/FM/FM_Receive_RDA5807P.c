#define RDA5807P_RD_CTRL   0x21 
#define RDA5807P_WR_CTRL   0x20 


/***************************************************
RDA5807P
****************************************************/
U8 code RDA5807P_initialization_reg[]={
0x80, 0x01, //02H:   //set no mute : 0xc0, 0x01; mute 80 01
0x00, 0x10,        
0x04, 0x00,        
0x88, 0xAF, //05H: 
0x80, 0x00,
0x5F, 0x1A, //07H  change the seek time;
0x5e, 0xc6,
0x00, 0x00,        
0x40, 0x6e, //0AH: 
0x2d, 0x80,
0x58, 0x03,
0x58, 0x04,
0x58, 0x04,
0x58, 0x04,
0x00, 0x47, //10H: 
0x90, 0x00,        
0xF5, 0x87,
0x7F, 0x0B, //13H: reset ADC's parameters
0x04, 0xF1,
0x5E, 0xc0, //15H: 0x42, 0xc0
0x41, 0xe0,
0x50, 0x6f,
0x55, 0x92,
0x00, 0x7d,
0x10, 0xC0, //1AH
0x07, 0x80,
0x41, 0x1d, //1CH,
0x40, 0x06,
0x1f, 0x9B,
0x4c, 0x2b, //1FH. 
0x81, 0x10, //20H: 
0x45, 0xa0, // 21H
0x19, 0x3F, //22H: change AFC threshold
0xaf, 0x40, 
0x06, 0x81,
0x1b, 0x2a, //25H
0x0D, 0x04, //26H, shutdown Rssi_autodown function
0x80, 0x9F, //0x80, 0x2F, 
0x17, 0x8A,
0xD3, 0x49,
0x11, 0x42,
0xA0, 0xC4, //2BH
0x3E, 0xBB,
0x00, 0x00,        
0x58, 0x04,
0x58, 0x04, //2FH
0x58, 0x04,
0x00, 0x74,
0x3D, 0x00,
0x03, 0x0C,
0x2F, 0x68,
0x38, 0x77, //35H     
};


U8 code RDA5807H_initialization_reg[]={
0x80, 0x01, //02H:   //set no mute : 0xc0, 0x01; mute 80 01
0x00, 0x10,        
0x04, 0x00,        
0x86, 0xAF, //05H: 
0x40, 0x00,
0x56,0xc6,
0x00, 0x00,
0x00, 0x00,
0x00, 0x00,  //0X0AH
0x00, 0x00,
0x00, 0x00,
0x00, 0x00,
0x00, 0x00,
0x00, 0x00,
0x00, 0x06,  //0X10H
0x00,0x19,// //0x00,0x09,//0830
0x2a,0x11,
0x00,0x2e,
0x2a,0x30,
0xb8,0x3c,  //0x15h
0x90, 0x00,
0x2a,0x91,
0x84, 0x12,
0x00,0xa8,
0xc4,0x00,  //0x1ah
0xe0,0x00,
0x30,0x1d,//0x24,0x9d,1ch
0x81,0x6a,
0x46, 0x08,
0x00,0x86,  //0x1fh
0x06, 0x61,  //0X20H
0x00, 0x00,
0x10,0x9e,
0x24,0xc9,//   0x24,0x47,//0830//23h
0x04, 0x08,  //0x24H  pilot_th
0x06,0x08,  //0x06,0x08,//0830  //0x25h
0xe1,0x05,
0x3b,0x6c,
0x2b,0xec,
0x09,0x0f,
0x34, 0x14,  //0X2AH
0x14, 0x50,
0x09,0x6d,
0x2d,0x96,
0x01,0xda,
0x2a,0x7b,
0x08, 0x21,   //0X30H
0x13,0xd5,
0x48, 0x91,
0x00,0xbc,
0x08,0x96,//0830  0x34h
0x15,0x3c,
0x0b,0x80,
0x25,0xc7,
0x00,0x00,
};
U8 RDAFM_reg[8] = {0x0};


void RDA5807P_FMPowerdown(void)
{
 	U8 RDA5807P_poweroff[] ={0x00,0x00};  
	SendStr(RDA5807P_WR_CTRL,RDA5807P_poweroff,2);
	IIC_Dealy(7); // wait for 50ms
}


void RDA5807P_MUTE(void)
{
 	U8 RDA5807P_Muteon[] ={0x80,0x01}; 	//{0x80,0x01}; 
		
	SendStr(RDA5807P_WR_CTRL,RDA5807P_Muteon,2);
	IIC_Dealy(7); // wait for 50ms
}

void RDA5807P_UnMUTE(void)
{
 	U8 RDA5807P_Muteclose[] ={0xc0,0x01}; 

	SendStr(RDA5807P_WR_CTRL,RDA5807P_Muteclose,2);
	IIC_Dealy(7); // wait f
}

void RDA5807P_FMTune() //87.5Mhz -> 8750
{   
 	U8 RDA5807P_channel_start_tune[] ={0x80,0x01,0x00,0x10}; 	//87.0MHz	{0xc0,0x01,0x00,0x10};
	U16 curChan;	
	
	RDA5807P_channel_start_tune[0]=0xc0;
	curChan=(gw_FM_frequency)-870;
	RDA5807P_channel_start_tune[2]=curChan>>2;
	RDA5807P_channel_start_tune[3]=(((curChan&0x0003)<<6)|0x10) | (RDAFM_reg[3]&0x0f);	//set tune bit
	
	SendStr(RDA5807P_WR_CTRL,RDA5807P_channel_start_tune,4);
	IIC_Dealy(7); // wait f
	
	LCM_ShowFrequency_L();
}

/**************************************************************************************
*   函数名称：CL6017_FMSeekFromCurrentCh
*   功    能：
*   输入参数：BOOL seekUp: 0-seek up, 1-seek down
*   输入参数：unsigned int *TunedFrequency: the radio frequency stay in when seeking process complete, the frequency unit is 10khz	 

*   输出参数：BOOL sfflag: 0-Seek successful  1-Seek failure
*   说    明： seek from current channel automaticly,if want to seek from certain channel,  use CL6017_FMTune to certain channel ahead
**************************************************************************************/
xdata U8 gc_seekUp;
U8 times;
U8  finefreq;
U8 RDA5807P_reg_data[4]={0};
/*
void RDA5807P_FMSeekFromCurrentCh(U8 seekUp)
{
 	

//	gw_FM_frequency=TunedFrequency;
	gc_seekUp=seekUp;
   	do
	{
		if (gc_seekUp)
		{
			if(gw_FM_frequency >= 1080)
			{
				gw_FM_frequency = 1080;
				break;
			}else
			{
				gw_FM_frequency++;
			}
		}
		else
		{
			if(gw_FM_frequency <= 875)
			{
				gw_FM_frequency = 875;
				break;
			}else
			{
				gw_FM_frequency--;
			}
		}
		LCM_ShowFrequency_L();
//dbprintf("FM_disp=%x\n",gw_FM_frequency);
	
		RDA5807P_reg_data[0]=0x80;
		RDA5807P_reg_data[1]=0x01;
		RDA5807P_reg_data[2]=(gw_FM_frequency-870)>>2;
		RDA5807P_reg_data[3]=((((gw_FM_frequency-870)&0x0003)<<6)|0x10) | (RDAFM_reg[3]&0x0f);
		SendStr(RDA5807P_WR_CTRL,RDA5807P_reg_data,4);
		IIC_Dealy(10); 
		times=0;
		do{
			times++;
			IIC_Dealy(4);  	
			//read REG0A&0B	
			RecStr(RDA5807P_RD_CTRL,RDA5807P_reg_data,4);			
			//check whether STC=1
		}while(((RDA5807P_reg_data[0]&0x40)==0)&&(times<10));
		//check FM_TURE

		if((RDA5807P_reg_data[2] &0x01)==0)
		{
		//	return 0;
			finefreq=0;
		}else
		{	
		//	return 1;
			finefreq=1;
		}
   	}while(finefreq == 0);
    
	RDA5807P_FMTune() ;
}*/

void RDA5807P_FMVolumeSet(unsigned int vol)  /*input: 0 - 15 */
{
 	RDAFM_reg[0]|=0x40;  //set no mute
 	RDAFM_reg[3]&=0xef;
	RDAFM_reg[7]=( RDAFM_reg[7] & 0xf0 ) | vol; 

	SendStr(RDA5807P_WR_CTRL,RDAFM_reg,8);
	IIC_Dealy(7); // wait for 50ms

}

void RDA5807P_FMInit(void)
{
 	U8 error_ind = 0;
	U8 RDA5807P_REG[]={0x00,0x02};
	U8 RDA5807P_on[] ={0x80,0x01}; 
	U8 RDA5807P_on_tune[] ={0x80,0x01,0x00,0x10}; 
	U8 RDA5807P_ReadData[10] = {0x0};
	U8 i = 0;
		
//	Delay_Time_ms( 100 );
//	error_ind = I2C_WriteRDA5807PReg( (unsigned char *)&RDA5807P_REG[0], 2);
//	Delay_Time_ms( 30 );

	IIC_Dealy(10);	
	SendStr(RDA5807P_WR_CTRL,RDA5807P_REG,2);
	IIC_Dealy(4);

//	I2C_ReadRDA5807PReg(&(RDA5807P_ReadData[0]), 10);
	RecStr(RDA5807P_RD_CTRL,RDA5807P_ReadData,10);
//	Delay_Time_ms( 100 );
	IIC_Dealy(10);	
	{
		U8 i;
		dbprintf("init\n");
		for(i=0;i<9;i++)
		{
			dbprintf("%bx ",RDA5807P_ReadData[i]);
		}
	}
//	error_ind = I2C_WriteRDA5807PReg( (unsigned char *)&RDA5807P_on[0], 2);
	SendStr(RDA5807P_WR_CTRL,RDA5807P_on,2);
	
	//Delay_Time_ms( 600 );
	IIC_Dealy(70);
	if ((RDA5807P_ReadData[8] == 0x58) && (RDA5807P_ReadData[9] == 0x01))
	{
	dbprintf("5807 -H\n");
	//	chipID = 0x5801;	////H版芯片
		for (i=0; i<8; i++)
			RDAFM_reg[i] = RDA5807H_initialization_reg[i];

	//	error_ind = I2C_WriteRDA5807PReg( (unsigned char *)&RDA5807H_initialization_reg[0], sizeof(RDA5807H_initialization_reg));
		SendStr(RDA5807P_WR_CTRL,RDA5807H_initialization_reg,sizeof(RDA5807H_initialization_reg));

	}
	else if((RDA5807P_ReadData[8] == 0x58) && (RDA5807P_ReadData[9] == 0x04))
	{//E版芯片
	dbprintf("5807 -E\n");
		for (i=0; i<8; i++)
			RDAFM_reg[i] = RDA5807P_initialization_reg[i];
	//	error_ind = I2C_WriteRDA5807PReg( (unsigned char *)&RDA5807P_initialization_reg[0], sizeof(RDA5807P_initialization_reg));
		SendStr(RDA5807P_WR_CTRL,RDA5807P_initialization_reg,sizeof(RDA5807P_initialization_reg));
	}/**/
	else
	{
	dbprintf("5807 -P\n");
		for (i=0; i<8; i++)
			RDAFM_reg[i] = RDA5807P_initialization_reg[i];
		//error_ind = I2C_WriteRDA5807PReg( (unsigned char *)&RDA5807P_initialization_reg[0], 8);
		SendStr(RDA5807P_WR_CTRL,RDA5807P_initialization_reg,8);
	}
	//Delay_Time_ms( 100 );
	IIC_Dealy(10);
	//error_ind = I2C_WriteRDA5807PReg( (unsigned char *)&RDA5807P_on_tune[0], 4);
	SendStr(RDA5807P_WR_CTRL,RDA5807P_on_tune,4);
//	Delay_Time_ms( 200 );
	IIC_Dealy(20);

	
}

/*U8 RDA5807P_Read_ID(void)
{
	U8 RDA5807P_ReadData[10] = {0x0};
	U8 RDA5807P_data[]={0x00,0x02};
	
	//Delay_Time_ms( 100 );
	//I2C_WriteRDA5807PReg( (U8 *)&RDA5807P_data[0], 2);
	//Delay_Time_ms( 30 );
	//I2C_ReadRDA5807PReg(&(RDA5807P_ReadData[0]), 10);

//	dbprintf("5807send %bx,%bx",RDA5807P_data[0],RDA5807P_data[1]);
	IIC_Dealy(10);//7.5*10ms
	SendStr(RDA5807P_WR_CTRL,RDA5807P_data,2);
	IIC_Dealy(4);
	RecStr(RDA5807P_RD_CTRL,RDA5807P_ReadData,10);
	{
		U8 i;
		for(i=0;i<10;i++)
		{
			dbprintf("%bx ",RDA5807P_ReadData[i]);
		}
	}
		dbprintf("\n5807 read id end\n");

	if((RDA5807P_ReadData[4] == 0x58) || (RDA5807P_ReadData[6] == 0x58) || (RDA5807P_ReadData[8] == 0x58)) 
	    return 0x58;
	else
	    return 0;
}*/


U8 FmMode_GetSearchStatus(U16 gw_FM_frequency)
{
		RDA5807P_reg_data[0]=0x80;
		RDA5807P_reg_data[1]=0x01;
		//==set freq
		RDA5807P_reg_data[2]=(gw_FM_frequency-870)>>2;
		RDA5807P_reg_data[3]=((((gw_FM_frequency-870)&0x0003)<<6)|0x10) | (RDAFM_reg[3]&0x0f);
		//
		SendStr(RDA5807P_WR_CTRL,RDA5807P_reg_data,4);

		IIC_Dealy(10); 
		times=0;
		do{
			times++;
			IIC_Dealy(4);  	
			//read REG0A&0B	
			RecStr(RDA5807P_RD_CTRL,RDA5807P_reg_data,4);			
			//check whether STC=1
		}while(((RDA5807P_reg_data[0]&0x40)==0)&&(times<10));
		//check FM_TURE

		if((RDA5807P_reg_data[2] &0x01)==0)
		{
			return 0;
		}else
		{	
			return 1;
		}
}



/**************************************************************************************
*   函数名称：CL6017_FMSeekFromCurrentCh
*   功    能：
*   输入参数：BOOL seekUp: 0-seek up, 1-seek down
*   输入参数：unsigned int *TunedFrequency: the radio frequency stay in when seeking process complete, the frequency unit is 10khz	 

*   输出参数：BOOL sfflag: 0-Seek successful  1-Seek failure
*   说    明： seek from current channel automaticly,if want to seek from certain channel,  use CL6017_FMTune to certain channel ahead
**************************************************************************************/
xdata U8 gc_seekUp;
#define FM_SEARCH_UP 0
#define FM_SEARCH_DOWN 1
#define FM_SEARCH_ALL 2
void RDA5807P_FMSeekFromCurrentCh(U8 seekUp)
{
 	
	U8 i,cnt=0;
    U8 seek_auto,tc_TmpData;


//	gw_FM_frequency=TunedFrequency;
	gc_seekUp=seekUp;	

	if((gw_FM_frequency<lowfreq)||(gw_FM_frequency>highfreq))
	gw_FM_frequency=lowfreq;

 
		if(gc_seekUp==FM_SEARCH_ALL)
		{
			gw_FM_frequency=lowfreq;
			gc_FM_CHAmount=0;
	
			for(i=0;i<30;i++)
			gw_FM_CH[i]=0;
		}
		else if(gc_seekUp==FM_SEARCH_UP)
		{
			if(gw_FM_frequency>=highfreq)
	        {
	          gw_FM_frequency=lowfreq;
	        }
	        else
	        {
	          gw_FM_frequency+=1;
	        }
		}
		else if(gc_seekUp==FM_SEARCH_DOWN)
		{
			if(gw_FM_frequency<=lowfreq)
	        {
	          gw_FM_frequency=highfreq;
	        }
	        else
	        {
	          gw_FM_frequency-=1;
	        }
		}
			//LCM_ShowFrequency_L();

			RDA5807P_MUTE();


	seek_auto =1;
   do
   {
      
      tc_TmpData = FmMode_GetSearchStatus(gw_FM_frequency);
      //------------------------------------------------------------
      if(tc_TmpData)
      {

	  	if(gc_seekUp==FM_SEARCH_ALL)
		{
          gw_FM_CH[cnt]=gw_FM_frequency;
          gc_FM_CHAmount=cnt;
          cnt++;

         // LCD_Disp_FM_CHNUM(cnt);
		 }
		 else if(gc_seekUp==FM_SEARCH_UP || gc_seekUp==FM_SEARCH_DOWN)
		 {
		 	seek_auto=0;
		 }
      }
      if((gw_FM_frequency>=highfreq)||(cnt>=30))
      {
        seek_auto=0;
      }
//	  LCM_ShowFrequency_L(gw_FM_frequency);

	  if(seek_auto)
      {
	  	if(gc_seekUp==FM_SEARCH_ALL||gc_seekUp==FM_SEARCH_UP )
		{
	        if(gw_FM_frequency>=highfreq)
	        {
	          gw_FM_frequency=highfreq;
	          seek_auto=0;
	        }
	        else
	        {
	          gw_FM_frequency+=1;
	        }
	
			if(gw_FM_frequency==980)gw_FM_frequency+=1;	   //miss 98.0MHz
		}
		else if(gc_seekUp==FM_SEARCH_DOWN)
		{
			if(gw_FM_frequency<=lowfreq)
	        {
	          gw_FM_frequency=lowfreq;
	          seek_auto=0;
	        }
	        else
	        {
	          gw_FM_frequency-=1;
	        }	
			if(gw_FM_frequency==980)gw_FM_frequency-=1;	   //miss 98.0MHz
		}
      }

	
   }while(seek_auto);
   if(gc_seekUp==FM_SEARCH_ALL)
	{
	   if(gc_FM_CHAmount==0)
	   {
	     gw_FM_CH[0]=lowfreq;
	     gw_FM_frequency= gw_FM_CH[0];
	   }
	   else
	   {
	     gw_FM_frequency = gw_FM_CH[0]; 
	   }
	   gc_FM_CH_COUNT=0;
   }
//dbprintf("FM_disp=%x\n",gw_FM_frequency);
		//mute
	RDA5807P_UnMUTE();

	//SPEAKER_ON;	
	RDA5807P_FMTune() ;
}

