#include "..\header\SPDA2K.h"
#include "..\header\variables.h"
#include "..\header\USB_MSDC.h"

void Save_CBW_Packet(void) 
{
	xdata	U8	i;
	//  解參數	
    while(XBYTE[0xB5A1]&0x02); 
    XBYTE[0xB5D2]&=0xDD;				//Disable Bulk-Out中斷
    gc_dCBW_Tag[0]=USB_BUF_Bank0[4];
    gc_dCBW_Tag[1]=USB_BUF_Bank0[5];
    gc_dCBW_Tag[2]=USB_BUF_Bank0[6];
    gc_dCBW_Tag[3]=USB_BUF_Bank0[7];
	gdw_dCBW_DataXferLength.BY[0]=USB_BUF_Bank0[11];	// MSB  gdw_dCBW_DataXferLength
	gdw_dCBW_DataXferLength.BY[1]=USB_BUF_Bank0[10];	//      是用來表示CBW 的資料傳輸長度
	gdw_dCBW_DataXferLength.BY[2]=USB_BUF_Bank0[ 9];	//
	gdw_dCBW_DataXferLength.BY[3]=USB_BUF_Bank0[ 8];	// LSB
	gb_bmCBWFlag=USB_BUF_Bank0[12]>>7;					// CBW的資料傳輸方向 '1' =>  in,Device => Hhost
	gc_bCBWLUN = (USB_BUF_Bank0[13]&0x0F);				// 決定目前是操作那一台磁碟機
	//決OS支援多槽的功能的功能嗎  ==> yes(1)    ==> No(2)
	switch(gc_bCBWLUN)
	{
		case 0x00 :
			if(gb_CardDetect^gb_CardExist)
			{
				if(gb_Start_Stop_Flag==0)
				{
					if(!SD_Identification_Flow())
					{
						gb_CardDetect=0; 
			    		gb_CardExist=0;
					    gb_Start_Stop_Flag=0;
						break;	
					}
					else
					{
						gb_MediaChanged=1;
						gb_CardChangeStage=1;
						gb_CardExist=1;
					}
				}
			}			
		break;
	}

	for(i=0;i<10;i++)
	{
		gc_COMD_IN[i]=USB_BUF_Bank0[i+15];
	}
    XBYTE[0xB5E9]=0x02;
}      


void Decode_CBW_Command(void) 
{
	switch (gc_COMD_IN[0]) 
	{
		case 0x28:
			if(gb_CardExist)
			{					
				if(Check_LBA_ID())
				{                           
     	    		gc_USBStage =BulkInReadSectorsStage;      
     	    	    gc_bCSWStatus=0x00;                       
     	    	    gc_RequestSenseCode  = 0xFF;              
     	    	}                                             
     	    	else
				{
     	    	    gc_USBStage =BulkInReadBufferStage;      
   	    	    	gc_bCSWStatus=0x01;                       
                   	gc_RequestSenseCode  = 0x07;    
     	    	}    	           
			}
			else
			{
				LBA.BY[0]=0x00;                               
            	LBA.BY[1]=0x00;                               
            	LBA.BY[2]=0x00;                               
            	LBA.BY[3]=0x00;                               
                TotalXferPageNum.BY[0]=gc_COMD_IN[7];            
            	TotalXferPageNum.BY[1]=gc_COMD_IN[8];            
                gc_USBStage =BulkInReadBufferStage;    	    	
     	    	gc_bCSWStatus=0x01;                           
     	    	gc_RequestSenseCode  = 0x00;                  
			}
		break;

		case 0x2A:
			if(gb_CardExist) 
			{
				if(Check_LBA_ID())
				{
 	    			gc_USBStage=BulkOutWriteSectorsStage;       
 	    	    	gc_bCSWStatus=0x00;                         
 	    	    	gc_RequestSenseCode  = 0xFF;                
     	    	}                                               
     	    	else
				{                                           
     	    	    gc_USBStage=BulkOutWriteBufferStage;        
  	    	    	gc_bCSWStatus=0x01;                         
                   	gc_RequestSenseCode  = 0x07;
     	    	}    	           
			}
			else
			{
				LBA.BY[0]=0x00;                                 
            	LBA.BY[1]=0x00;                                 
            	LBA.BY[2]=0x00;                                 
            	LBA.BY[3]=0x00;                                 
                TotalXferPageNum.BY[0]=gc_COMD_IN[7];            
            	TotalXferPageNum.BY[1]=gc_COMD_IN[8];              
                gc_USBStage=BulkOutWriteBufferStage;            
     	    	gc_bCSWStatus=0x01;                             
     	    	gc_RequestSenseCode  = 0x00;                    
			}
		break;

		case 0x00:
			if(gb_CardExist)
			{
				gc_bCSWStatus = 0x00;                  						
				gc_RequestSenseCode = 0xFF ;           						
			}
			else
			{
				gc_bCSWStatus = 0x01;                  						
				gc_RequestSenseCode = 0x00 ;           						
			}			
		
			if(gb_MediaChanged) 
			{
				if(gb_CardChangeStage)
				{  // Media change
					gc_bCSWStatus = 0x01;
					gc_RequestSenseCode = 0x01 ; 
				}
			}
			gc_USBStage=CSWStage;
		break;

		case 0x03:
			SetRequestSenseData();
			gc_USBStage=BulkInResponseStage;
			gc_bCSWStatus = 0x00;
		break;

		case 0x12:
			SetResponseINQUIRYData();
			gc_USBStage=BulkInResponseStage;
			gdw_dCBW_DataXferLength.WD[1]=36;
			gc_bCSWStatus=0x00;
		break;

		case 0x5A:
		case 0x1A:
			if (gc_COMD_IN[2] != 0x3F) 
			{
				gb_StallWhichEndPoint=0;
				gc_USBStage=BulkStallStage;
				gc_bCSWStatus=0x01;
				gc_RequestSenseCode=0x03;
			}
			else 
			{
				if(gc_COMD_IN[0]==0x1A) 
				{
					USB_BUF_Bank0[0]=0x03;
					USB_BUF_Bank0[1]=0x00;
					USB_BUF_Bank0[2]=0x00;
					USB_BUF_Bank0[3]=0x00;
				}
				else
				{
					USB_BUF_Bank0[0]=0x00;
					USB_BUF_Bank0[1]=0x07;
					USB_BUF_Bank0[2]=0x00;
					USB_BUF_Bank0[3]=0x00;
				}                                                                                                      
				USB_BUF_Bank0[4]=0x00;
				USB_BUF_Bank0[5]=0x00;
				USB_BUF_Bank0[6]=0x00;
				USB_BUF_Bank0[7]=0x00;
				gc_USBStage=BulkInResponseStage;
				gb_XferNormalResponseData_ModeSense=ModeSenseResponseData;
				gc_bCSWStatus=0x00;
			}
		break;
		
		case 0x1E:
			if(gc_COMD_IN[4]&0x01) 
			{
				gc_bCSWStatus=0x01;
				gc_RequestSenseCode=0x03;
			}
			else 
			{
				gc_bCSWStatus=0x00;
			}
			gc_USBStage=CSWStage;
		break;
		
		case 0x1B:
			gc_USBStage=CSWStage;
			gc_bCSWStatus=0x00;
			if(gc_COMD_IN[4]&0x01)
			{
		    	gb_CardExist=1;
		    	gb_Start_Stop_Flag=0;
			}                       			
			else
			{
				gb_CardExist=0;
				gb_Start_Stop_Flag=1;
			}
		break;
		
		case 0x23: 
			if(gb_CardExist)
			{
				SetReadFormatCapacity();
				gc_USBStage=BulkInResponseStage;
     	    	gc_bCSWStatus=0x00;
				gc_RequestSenseCode=0xFF;
     	    }
     	    else
			{
     	    	gb_StallWhichEndPoint=0;
				gc_USBStage=BulkStallStage;
				gc_bCSWStatus=0x01;
			    gc_RequestSenseCode=0x00;
     	    }
     	    if(gb_MediaChanged) 
			{  							
				if(gb_CardChangeStage)
				{  // Media change
					gc_bCSWStatus = 0x01;     
					gc_RequestSenseCode = 0x01 ; 
				}				
			}
		break;

		case 0x25:
			if(gb_CardExist)
			{
				SetReadCapacityData();
				gc_USBStage=BulkInResponseStage;
     	    	gc_bCSWStatus=0x00;
				gc_RequestSenseCode=0xFF;
     	    }
     	    else
			{
     	    	gb_StallWhichEndPoint=0;
				gc_USBStage=BulkStallStage;
				gc_bCSWStatus=0x01;
			    gc_RequestSenseCode=0x00;
     	    }
     	    if(gb_MediaChanged) 
			{  							
				if(gb_CardChangeStage)
				{  // Media change
					gc_bCSWStatus = 0x01;     
					gc_RequestSenseCode = 0x01 ; 
				}				
			}	    
		break;

		case 0x2F:
            gc_USBStage=CSWStage;
            gc_bCSWStatus=0x00;
            gc_RequestSenseCode=0xFF;
   	    break;

        case 0x2E:
            TotalXferPageNum.BY[0]=gc_COMD_IN[7];            
            TotalXferPageNum.BY[1]=gc_COMD_IN[8];             
     	    gc_USBStage = BulkOutWriteBufferStage;
     	    gc_bCSWStatus=0x00;                     
     	    gc_RequestSenseCode = 0xFF;                   	    	       
   	    break;

        default:
			// 此時應Stall EndPoint1/2才對吧
			if(gb_bmCBWFlag) 
			{
				gb_StallWhichEndPoint=0;    // for bulk in stall
			}
			else
			{
				gb_StallWhichEndPoint=1;    // for bulk in stall
			}
			gc_USBStage=BulkStallStage;                   // '7'
			gc_bCSWStatus=0x01;
			gc_RequestSenseCode=0x02;                     // 不明原因錯誤，設OPcode不支援
		break;
	}
}


void SetResponseINQUIRYData(void) 
{
	xdata	U8	i;

	for(i=0;i<36;i++)
	{
		USB_BUF_Bank0[i]=Standard_INQUIRY_Format_SDMMC[i];
	}
}


void SetRequestSenseData(void) 
{
	xdata	U8	i;

	for(i=1;i<18;i++)
	{
		USB_BUF_Bank0[i]=0x00;
	}
	USB_BUF_Bank0[0]=0x70;
	USB_BUF_Bank0[7]=0x0A;

	switch(gc_RequestSenseCode) 
	{
		case 0x00:	// 02 3A 00 - Medium not present
			USB_BUF_Bank0[ 2]=0x02;
			USB_BUF_Bank0[12]=0x3A;
		break;

		case 0x01:	// 06 28 00 - Media changed
			USB_BUF_Bank0[ 2]=0x06;
			USB_BUF_Bank0[12]=0x28;
			gb_MediaChanged=0;
			gb_CardChangeStage=0;
		break;
		
		case 0x02:	// 05 20 00 - Invalid command operaion code
			USB_BUF_Bank0[ 2]=0x05;
			USB_BUF_Bank0[12]=0x20;
		break;

		case 0x03:	// 05 24 00 - Invalid field in command packet
			USB_BUF_Bank0[ 2]=0x05;
			USB_BUF_Bank0[12]=0x24;
		break;          

		case 0x05:	// 07 27 00 - Write protected media
			USB_BUF_Bank0[ 2]=0x07;
			USB_BUF_Bank0[12]=0x27;
		break;

		case 0x06:	// 03 11 00 - Unrecovered read error
			USB_BUF_Bank0[ 2]=0x03;
			USB_BUF_Bank0[12]=0x11;
		break;

		case 0x07:	// 05 21 00 - Logical block address out of range
			USB_BUF_Bank0[ 2]=0x05;
			USB_BUF_Bank0[12]=0x21;
		break;

		case 0x08:	// 03 03 00 - Write fault
			USB_BUF_Bank0[ 2]=0x03;
			USB_BUF_Bank0[12]=0x03;
		break;
	}	
	gc_RequestSenseCode=0xFF;
}


void SetReadFormatCapacity(void)
{
	USB_BUF_Bank0[ 0]=0x00;
	USB_BUF_Bank0[ 1]=0x00;
	USB_BUF_Bank0[ 2]=0x00;
	USB_BUF_Bank0[ 3]=0x08;
	USB_BUF_Bank0[ 4]=SD_Capacity.BY[0];
	USB_BUF_Bank0[ 5]=SD_Capacity.BY[1];
	USB_BUF_Bank0[ 6]=SD_Capacity.BY[2];
	USB_BUF_Bank0[ 7]=SD_Capacity.BY[3];
	USB_BUF_Bank0[ 8]=0x02;
	USB_BUF_Bank0[ 9]=0x00;
	USB_BUF_Bank0[10]=0x02;
	USB_BUF_Bank0[11]=0x00;
}


void SetReadCapacityData(void)
{
	SD_Capacity.LW--;
	USB_BUF_Bank0[0]=SD_Capacity.BY[0];
	USB_BUF_Bank0[1]=SD_Capacity.BY[1];
	USB_BUF_Bank0[2]=SD_Capacity.BY[2];
	USB_BUF_Bank0[3]=SD_Capacity.BY[3];
	USB_BUF_Bank0[4]=0x00;
	USB_BUF_Bank0[5]=0x00;
	USB_BUF_Bank0[6]=0x02;
	USB_BUF_Bank0[7]=0x00;
	SD_Capacity.LW ++;
}


bit Check_LBA_ID(void) 
{
	if ((gc_COMD_IN[0] == 0x28) || (gc_COMD_IN[0] == 0x2A)) 
	{
		LBA.BY[0] = gc_COMD_IN[2];
		LBA.BY[1] = gc_COMD_IN[3];
		LBA.BY[2] = gc_COMD_IN[4];
		LBA.BY[3] = gc_COMD_IN[5];      
		TotalXferPageNum.BY[0] = gc_COMD_IN[7];
		TotalXferPageNum.BY[1] = gc_COMD_IN[8];
	}
	else 
	{
		LBA.BY[0] = 0x00;
		LBA.BY[1] = gc_COMD_IN[1]& 0x0F;
		LBA.BY[2] = gc_COMD_IN[2];
		LBA.BY[3] = gc_COMD_IN[3];      
		TotalXferPageNum.BY[0] = 0x00;    
		TotalXferPageNum.BY[1] = gc_COMD_IN[4];
	}

	if(LBA.LW>=SD_Capacity.LW)
	{
		LBA.LW=SD_Capacity.LW;
		return 0;		
	}
	else
	{
		return 1;
	}
}


void StallBulkRW(void) 
{
	if(gb_StallWhichEndPoint==0) 
	{  // for bulk in stall
		XBYTE[0xB5EB]=0x01;
		while(XBYTE[0xB5EE]==0);
		XBYTE[0xB5E8]=0x04;//in stall
	}
	else //if gb_StallWhichEndPoint==1) 
	{  // for bulk out stall
		XBYTE[0xB5EB]=0x02;
		while(XBYTE[0xB5EE]==0);
		XBYTE[0xB5E8]=0x08;//out stall
	}
}
