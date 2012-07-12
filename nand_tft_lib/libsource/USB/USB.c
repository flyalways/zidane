#include "SPDA2K.h"
#include "..\..\turnkey_xbb\LCM\TFT_display.h"		// TFT	

extern void Get_KeyValue(UBYTE ADCValue);
extern data bit gb_USB_ISR_Detected;
U32 gdw_USB_CountDown=0;  //080925chiayen add

extern	U8		gc_clock_mode;  //20090526 chiayen add


data	bit gb_cmd_timeout=0;
data	bit	gb_ShowUSB=0;

void USB_DisConnect(void)
{
	XBYTE[0xB01D] = 0x01;

	USB_REG[0xA1]=0x00;
	USB_REG[0x27] = 0x01; 	// 080324 USB disconnect PC
	GLOBAL_REG[0x17]=0x00;
	USB_REG[0xC0]=0x00;	//clear suspend/resume/configuration change,turn off USB D+
	
	//Adding for avoid losting power
	USB_REG[0xD0]=0x00;
	USB_REG[0xD1]=0x00;
	USB_REG[0xD2]=0x00;
	USB_REG[0xD3]=0x00;
	GLOBAL_REG[0x16]=0x05;	//Close USB UTMI 30Mhz

	GLOBAL_REG[0x16] = 0x01;//Close USB UTMI 30Mhz
   	GLOBAL_REG[0x11] &= 0xEF;       //Close USB clk source
    USER_DelayDTms(20);
}


void USB_Mode(void) 
{
	
	U8  tc_clock_mode_backup;  //20090526 chiayen add
	


	gb_cmd_timeout=0;
	set_clock_mode(CLOCK_MODE_USB); 
    PowerOn_Init_USB();
	Fill_EndpointDataTable_Into_ExRamBase();
	USBInitialize(); 
	Init_Flash_Reg();
	Init_Flash_Variable();
	gc_WriteProof = 0x00;
	InitFlash();
	
	USB_REG[0x27]    = 0x00; 	// 080304 jay change for new fpga
	GLOBAL_REG[0x17] = 0x01;	// UDC O/P	
    IE = 0x83;	   

	gdw_USB_CountDown = USB_DETECT_TIMEOUT;

	if(gb_cmd_timeout==1)  //080925chiayen modify
	{
		USB_DisConnect();
		set_clock_mode(CLOCK_MODE_MP3); 
		InitFlash();
		return;
	}
	   
	for(;;)
	{
		USB_PlugDetect();
		if(!gbt_USB_Detected)
		{//usb connected 
			USB_DisConnect();
			set_clock_mode(CLOCK_MODE_MP3);
			InitFlash();
			break;
		}
		else
		{
			if (gc_USBStage == CBWStage) 
			{
				if(gbt_USB_Connect_Flag)
				{
				    //tdw_CMD_In_Count =0x02FFFFF;
				}
				else
				{
				    gdw_USB_CountDown =0x01FFFF;  //081208  detect charge first
				}
				EA=1;							
				while (!gbt_USB_CMD_IN)                        // Bulk_In_Out  將跳出while 迴圈
				{
					USB_PlugDetect();
					if(!gbt_USB_Detected)
					{
						break;
					}
					if(gbt_USB_Connect_Flag)
					{
					    if(gbt_USER_Remove_Flag)
						{
					        gb_cmd_timeout=1;
						    break;
						}
					} 
					else
					{
				    	if((--gdw_USB_CountDown==0)&&(!gb_USB_ISR_Detected)) 
						{
							gb_cmd_timeout=1;
				    	  	break;
				    	}
					}
				}
				EA=0;                                       // 表示有 SCSI CMD 進入 關閉中斷避免常常進入			
				if((!gbt_USB_Detected)||(gb_cmd_timeout==1))  //080925chiayen modify
				{
					USB_DisConnect();
					set_clock_mode(CLOCK_MODE_MP3); 
					InitFlash(); 		
					break;
				}
				gbt_USB_CMD_IN = 0;
				SD_Card_Detect();		
				gbt_USB_Connect_Flag =1; // 081208 jay add			
				Save_CBW_Packet();
				Decode_CBW_Command();                             // 解CBW和記錄相關手續		    
			}

			if(gb_ShowUSB==0)
			{
				tc_clock_mode_backup=gc_clock_mode;

				set_clock_mode(CLOCK_MODE_MP3);
				TFT_ShowUSBIcon();	
				set_clock_mode(tc_clock_mode_backup);
				gb_ShowUSB=1;
			}

			if (gc_USBStage == BulkInReadSectorsStage) 
			{
				USB_REG[0xD2]      &= 0xDD;				//Disable Bulk-Out中斷
				ReadSectors();			    
				USB_REG[0xE9]       = 0x11;
				if (gc_ErrorCode == SET_ABRT) 
				{
					gc_RequestSenseCode = 0x06;        //ECC Error				
					gdw_dCBW_DataXferLength.LW = ((LWORD)TotalXferPageNum.WD) << 9;
					gc_bCSWStatus = 0x01;
				}
				else if (gc_ErrorCode == ID_NOT_FND) 
				{
					gc_RequestSenseCode = 0x07;        				//over range
					gdw_dCBW_DataXferLength.LW = ((LWORD)TotalXferPageNum.WD) << 9;
					gc_bCSWStatus = 0x01;
				}
				else 
				{
					gdw_dCBW_DataXferLength.LW = 0;
				}
				gc_USBStage = CSWStage;                                   // 進入Comand Status Wrapper
			}
			if (gc_USBStage == BulkOutWriteSectorsStage) 
			{
				USB_REG[0xD2]      &= 0xDD;				//Disable Bulk-Out中斷
				USB_REG[0x11]       = 0x01;   //HW auto ACK 
				WriteSectors();
				USB_REG[0x11]       = 0x00;
				USB_REG[0xE9]       = 0x22;

				if (gc_ErrorCode == SET_ABRT) 
				{
					gc_bCSWStatus = 0x01;
					gc_RequestSenseCode = 0x08;       					// write error 
				}
				else if (gc_ErrorCode == ID_NOT_FND) 
				{
					gc_RequestSenseCode = 0x07;      					//over range
					gc_bCSWStatus = 0x01;
				}
				else 
				{
					gc_bCSWStatus = 0x00;
					gc_RequestSenseCode = 0xFF;                        // normal
				}
				gdw_dCBW_DataXferLength.LW = 0;
				gc_USBStage = CSWStage;
			}
			if (gc_USBStage == BulkInResponseStage) 
			{                          // '4' BulkInProcess
				USB_REG[0xD2]      &= 0xDD;				//Disable Bulk-Out中斷
				if (gbt_XferNormalResponseData_or_ModeSense) 
				{                      // '1':Normal
			   		if (COMD_IN[0] == 0x23) 
					{
						gdw_dCBW_DataXferLength.WD[1] = 0x00;
						USB_REG[0x16]  = 0x00;
						USB_REG[0x15]  = 0x0C;
						USB_REG[0xA1] |= 0x01;
						while (USB_REG[0xA1] & 0x01)
						{
							USB_PlugDetect();  // 081014 Jay add for usb plug out
							if(!gbt_USB_Detected) break;
						}
						USB_REG[0xE9]  = 0x01;
			   		}
			   		else 
					{
						while (gdw_dCBW_DataXferLength.WD[1]) 
						{
							if (gdw_dCBW_DataXferLength.WD[1] > 64) 
							{
								gdw_dCBW_DataXferLength.WD[1] = gdw_dCBW_DataXferLength.WD[1] - 64;
					       		USB_REG[0x16] = 0x00;
								USB_REG[0x15] = 0x40;
							}
							else 
							{
					       		USB_REG[0x16] = gdw_dCBW_DataXferLength.BY[2] & 0x0F;
								USB_REG[0x15] = gdw_dCBW_DataXferLength.BY[3];
					       		gdw_dCBW_DataXferLength.WD[1] = 0x00;
					       	}
							USB_REG[0xA1] |= 0x01;
							while (USB_REG[0xA1] & 0x01)
							{  //081014 Jay add for usb plug out
								USB_PlugDetect();
								if(!gbt_USB_Detected) break;
							}
							USB_REG[0xE9]  = 0x01;
						}
		   			}
				}
				else 
				{
					if (COMD_IN[0] == 0x1A) 
					{
						gdw_dCBW_DataXferLength.WD[1] = 0x00;
						USB_REG[0x15]  = 0x04;
						USB_REG[0x16]  = 0x00;
						USB_REG[0xA1] |= 0x01;
						while (USB_REG[0xA1] & 0x01)
						{  //081014 Jay add for usb plug out
								USB_PlugDetect();
								if(!gbt_USB_Detected) break;
						}
						USB_REG[0xE9]  = 0x01;
					}
					else 
					{
						gdw_dCBW_DataXferLength.WD[1] = 0x00;
						USB_REG[0x15]  = 0x08;
						USB_REG[0x16]  = 0x00;
						USB_REG[0xA1] |= 0x01;
						while (USB_REG[0xA1] & 0x01)
						{  //081014 Jay add for usb plug out
							USB_PlugDetect();
							if(!gbt_USB_Detected) break;
						}
						USB_REG[0xE9]  = 0x01;
					}
					gbt_XferNormalResponseData_or_ModeSense = NormalResponseData;// 將預設設為一般型
				}
				gc_USBStage = CSWStage;
			}
			if (gc_USBStage == BulkStallStage) 
			{
				StallBulkRW();
				gc_USBStage = CSWStage;
			}
			if (gc_USBStage == BulkInReadBufferStage) 
			{
				USB_REG[0xD2]      &= 0xDD;				//Disable Bulk-Out中斷
				NoCard_ReadSectors();		    
				USB_REG[0xE9]       = 0x11;
				gdw_dCBW_DataXferLength.LW = 0;			
				gc_USBStage = CSWStage;                                   // 進入Comand Status Wrapper			                          
			}
			if (gc_USBStage == BulkOutWriteBufferStage) 
			{
				USB_REG[0xD2]      &= 0xDD;				//Disable Bulk-Out中斷
				USB_REG[0x11]       = 0x01;   //HW auto ACK 
				NoCard_WriteSectors();			
				USB_REG[0x11]       = 0x00;
				USB_REG[0xE9]       = 0x22;
				gdw_dCBW_DataXferLength.LW = 0;
				gc_USBStage = CSWStage;
			}				
			if (gc_USBStage == CSWStage) 
			{
				gc_USBStage = CBWStage;                                   // 重新準備收下一筆command
				USB_BUF_Bank0[ 0] = 'U';					// 以下由SetCSWData expand
				USB_BUF_Bank0[ 1] = 'S';
				USB_BUF_Bank0[ 2] = 'B';
				USB_BUF_Bank0[ 3] = 'S';
				USB_BUF_Bank0[ 4] = dCBW_Tag[0];
				USB_BUF_Bank0[ 5] = dCBW_Tag[1];
				USB_BUF_Bank0[ 6] = dCBW_Tag[2];
				USB_BUF_Bank0[ 7] = dCBW_Tag[3];
				USB_BUF_Bank0[ 8] = gdw_dCBW_DataXferLength.BY[3];                  // LSB 記錄剩下未被傳輸的資料
				USB_BUF_Bank0[ 9] = gdw_dCBW_DataXferLength.BY[2];                  //     記錄剩下未被傳輸的資料
				USB_BUF_Bank0[10] = gdw_dCBW_DataXferLength.BY[1];                  //     記錄剩下未被傳輸的資料
				USB_BUF_Bank0[11] = gdw_dCBW_DataXferLength.BY[0];                  // MSB 記錄剩下未被傳輸的資料
				USB_BUF_Bank0[12] = gc_bCSWStatus;                                 // Command Status 見BulkOnly P.15說明各種碼的情形
				USB_REG[0x2B]  = 0x00; 
				USB_REG[0x16]  = 0x00;
				USB_REG[0x15]  = 0x0D;
				USB_REG[0xA1] |= 0x01;
				while (USB_REG[0xA1] & 0x01)
				{  //081014 Jay add for usb plug out
					USB_PlugDetect();
					if(!gbt_USB_Detected) break;
				}
			}
			USB_REG[0xE9]  = 0x01;			
			USB_REG[0xD2]  |= 0x02;	//Enable Bulk-Out中斷
			USB_REG[0xA1]  |= 0x02;		//下一筆31Byte Bulk-Out
		}
	}
}


void Fill_EndpointDataTable_Into_ExRamBase(void)
{
	U8	i;

   	for(i=0;i<Device_DescriptorTable_Length;i++)
	{	// 18 bytes
	    EndPoint0Table[Device_DescriptorTable_Begine+i]=Device_DescriptorTable[i];
	}

	for(i=0;i<iManufacturer_DescriptorTable_Length;i++)
	{   // 26 bytes
		EndPoint0Table[iManufacturer_DescriptorTable_Begine+i]=iManufacturer_DescriptorTable[i];
	}

	for(i=0;i<iProduct_DescriptorTable_Length;i++)
	{	// 48 bytes
		EndPoint0Table[iProduct_DescriptorTable_Begine+i]=iProduct_DescriptorTable[i];
	}
		
	for(i=0;i<iSerialNumber_DescriptorTable_Length;i++)
	{	// 28 bytes
		EndPoint0Table[iSerialNumber_DescriptorTable_Begine+i]=iSerialNumber_DescriptorTable[i];
	}
		
    for(i=0;i<Configuration_DescriptorTable_Length;i++)
	{	// 32 bytes
		EndPoint0Table[Configuration_DescriptorTable_Begine+i]=Configuration_DescriptorTable[i];
	}

	for(i=0;i<iLANGID_DescriptorTable_Length;i++)
	{	//  4 bytes
		EndPoint0Table[iLANGID_DescriptorTable_Begine+i]=iLANGID_DescriptorTable[i];
	}

	for(i=0;i<iConfiguraiton_DescriptorTable_Length;i++)
	{	// 10 bytes
		EndPoint0Table[iConfiguraiton_DescriptorTable_Begine+i]=iConfiguraiton_DescriptorTable[i];
	}

	for(i=0;i<iDeviceQualifierString_DescriptorTable_Length;i++)
	{	// 10 bytes
		EndPoint0Table[iDeviceQualifierString_DescriptorTable_Begine+i]=iDeviceQualifierString_DescriptorTable[i];
	}

	for(i=0;i<iInterface_DescriptorTable_Length;i++)
	{	// 10 bytes
		EndPoint0Table[iInterface_DescriptorTable_Begine+i]=iInterface_DescriptorTable[i];
	}
	
	for(i=0;i<Other_Speed_Configuration_DescriptorTable_Length;i++)
	{	// 32 bytes
		EndPoint0Table[Other_Speed_Configuration_DescriptorTable_Begine+i]=Other_Speed_Configuration_DescriptorTable[i];
	}
	
	for(i=0;i<DeviceQualifier_DescriptorTable_Length;i++)
	{	// 10 bytes
		EndPoint0Table[DeviceQualifier_DescriptorTable_Begine+i]=DeviceQualifier_DescriptorTable[i];
	}
}



