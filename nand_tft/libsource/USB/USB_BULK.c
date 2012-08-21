#include "SPDA2K.h"

UBYTE gc_PseudoCard_Flag=0;

void Save_CBW_Packet(void) 
{
	U8	i;
	//  �ѰѼ�	
	while (USB_REG[0xA1] & 0x02)
	{
		USB_PlugDetect();
		if(!gbt_USB_Detected) break;
	} 
    USB_REG[0xD2]      &= 0xDD;				//Disable Bulk-Out���_
    dCBW_Tag[0]=USB_BUF_Bank0[4];
    dCBW_Tag[1]=USB_BUF_Bank0[5];
    dCBW_Tag[2]=USB_BUF_Bank0[6];
    dCBW_Tag[3]=USB_BUF_Bank0[7];
	gdw_dCBW_DataXferLength.BY[0]=USB_BUF_Bank0[11];                         	// MSB  gdw_dCBW_DataXferLength
	gdw_dCBW_DataXferLength.BY[1]=USB_BUF_Bank0[10];                         	//      �O�ΨӪ��CBW ����ƶǿ����
	gdw_dCBW_DataXferLength.BY[2]=USB_BUF_Bank0[ 9];                         	//
	gdw_dCBW_DataXferLength.BY[3]=USB_BUF_Bank0[ 8];                         	// LSB
	                                                                     		//                   '0' ==>out, Host  => Device
	gbt_bmCBWFlag=USB_BUF_Bank0[12]>>7;       							        // CBW����ƶǿ��V '1' =>  in,Device => Hhost
	gc_bCBWCBLength=USB_BUF_Bank0[14]&0x1F;                                     // ��CBW��command set����                      
	gc_bCBWLUN = (USB_BUF_Bank0[13]&0x0F);                                      // �M�w�ثe�O�ާ@���@�x�Ϻо�
	//�MOS�䴩�h�Ѫ��\�઺�\���  ==> yes(1)    ==> No(2)

	switch(gc_bCBWLUN){
		case 0x00:
			gc_CardType =0x01;
		break;

		case 0x01 :
			gc_CardType = 0x02;
			if(gc_CardDetect^(gc_CardExist&0x02))
			{
				if(!(gc_Start_Stop_Flag&gc_CardType))
				{
					if(!SD_Identification_Flow())
					{
						gc_CardDetect &= 0xFD; 
					    gc_CardExist &=0xFD;
					    gc_Start_Stop_Flag &= 0xFD;
						gc_PseudoCard_Flag = 1;
						break;	
					}
					else
					{
						gbt_StorageMedia_has_changed =1;
						gc_CardChangeStage |= 0x02; // SD_Card change flag
						gc_CardExist |=0x02;
					}
				}
			}			
		break;
		
		default:
			gc_CardType =0x01;
		break;
	}
	// ��16bytes��9byte���s�_�ӡA�ø�command
#if 0
	COMD_IN[0] = USB_BUF_Bank0[15];
    COMD_IN[1] = USB_BUF_Bank0[16];
    COMD_IN[2] = USB_BUF_Bank0[17];
    COMD_IN[3] = USB_BUF_Bank0[18];
    COMD_IN[4] = USB_BUF_Bank0[19];
    COMD_IN[5] = USB_BUF_Bank0[20];
    COMD_IN[6] = USB_BUF_Bank0[21];
    COMD_IN[7] = USB_BUF_Bank0[22];
    COMD_IN[8] = USB_BUF_Bank0[23];   
    COMD_IN[9] = USB_BUF_Bank0[24];       
#else
	for(i=0;i<10;i++)
	{
		COMD_IN[i] = USB_BUF_Bank0[i+15];
	}
#endif
    USB_REG[0xE9] = 0x02;
}      


void Decode_CBW_Command(void) 
{
	switch (COMD_IN[0]) 
	{
		case 0x28:							            //Read(10)
			if( gc_CardExist&gc_CardType)
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
					if(gbt_RW_UsbBuf)
					{  // 090520 Jay add
						gc_bCSWStatus=0x00;                       
     	    	    	gc_RequestSenseCode  = 0xFF;
					}
					else
					{  
	     	    	    gc_bCSWStatus=0x01;                       
	                    gc_RequestSenseCode  = 0x07;              
     	    		}    	           
				}
			}
			else
			{
				LBA.BY[0]=0x00;                               
            	LBA.BY[1]=0x00;                               
            	LBA.BY[2]=0x00;                               
            	LBA.BY[3]=0x00;                               
                TotalXferPageNum.BY[0]=COMD_IN[7];            
            	TotalXferPageNum.BY[1]=COMD_IN[8];            
                gc_USBStage =BulkInReadBufferStage;    	    	
     	    	gc_bCSWStatus=0x01;                           
     	    	gc_RequestSenseCode  = 0x00;                  
			}
		break;

		case 0x2A:							            //write(10)
			if(gc_CardExist&gc_CardType) 
			{
				if(Check_LBA_ID())
				{
					{
     	    			gc_USBStage=BulkOutWriteSectorsStage;       
     	    	    	gc_bCSWStatus=0x00;                         
     	    	    	gc_RequestSenseCode  = 0xFF;                
     	    		}                                               
     	    	}                                               
     	    	else
				{
     	    	    gc_USBStage=BulkOutWriteBufferStage;        
					if(gbt_RW_UsbBuf)
					{  // 090520 Jay add
						gc_bCSWStatus=0x00;                       
     	    	    	gc_RequestSenseCode  = 0xFF;
					}
					else
					{  
     	    	 	   gc_bCSWStatus=0x01;                         
                    	gc_RequestSenseCode  = 0x07;              
     	    		}    	           
				}
			}
			else
			{
				LBA.BY[0]=0x00;                                 
            	LBA.BY[1]=0x00;                                 
            	LBA.BY[2]=0x00;                                 
            	LBA.BY[3]=0x00;                                 
                TotalXferPageNum.BY[0]=COMD_IN[7];            
            	TotalXferPageNum.BY[1]=COMD_IN[8];              
                gc_USBStage=BulkOutWriteBufferStage;            
     	    	gc_bCSWStatus=0x01;                             
     	    	gc_RequestSenseCode  = 0x00;                    
			}
		break;

		case 0x00:					                                      	// �iTest Unit Ready�j
			if(gc_CardExist&gc_CardType)
			{
				gc_bCSWStatus = 0x00;                  						
				gc_RequestSenseCode = 0xFF ;           						
			}
			else
			{
				gc_bCSWStatus = 0x01;                  						
				gc_RequestSenseCode = 0x00 ;           						
			}			
			if (gbt_StorageMedia_has_changed) 
			{  							
				if(gc_CardChangeStage&gc_CardType)
				{  // Media change
					gc_bCSWStatus = 0x01;
					gc_RequestSenseCode = 0x01 ; 
				}
			}
			gc_USBStage = CSWStage;                        // '7'  �����Ǧ^CSW Stage
		break;

		case 0x03:							                //Request Sense Command
			SetRequestSenseData();
			gc_USBStage = BulkInResponseStage;				//'4'
			gc_bCSWStatus = 0x00;
		break;

		case 0x04:							            	// Format Unit		
			gc_bCSWStatus = 0x00;                          //  DeviceOK(�� card out)
			gc_RequestSenseCode = 0xFF;                    //  normal                        
			gc_USBStage = CSWStage;                        // '7'  �����Ǧ^CSW Stage
		break;     		

		case 0x12:							            	//INQUIRY Command
			SetResponseINQUIRYData();                   	// ��n�^��Host����Ƴ]�n�A����N��host�V�ڭn���
			gc_USBStage = BulkInResponseStage;              // '4'
			gdw_dCBW_DataXferLength.WD[1] = 36;
			gc_bCSWStatus = 0x00;                            // Command Passed ��BulkOnly P.15�����U�ؽX������
		break;

		case 0x5A:
		case 0x1A:							                    //Mode sense(6)  Type:Option			
			if (COMD_IN[2] != 0x3F) 
			{
				gbt_StallWhichEndPoint = StallEndPoint1;        // for bulk in stall
				gc_USBStage = BulkStallStage;                  // '7'
				gc_bCSWStatus = 0x01;                          // Command Failed ��BulkOnly P.15�����U�ؽX������
				gc_RequestSenseCode = 0x03;                    // ���Ѽ�OPcode���䴩
			}
			else 
			{
				if (COMD_IN[0] == 0x1A) 
				{		                //mode sense(6)��Header�u�n4 Byte
					USB_BUF_Bank0[0] = 0x03;
					USB_BUF_Bank0[1] = 0x00;
					USB_BUF_Bank0[2] = 0x00;
					switch(gc_CardType)
					{
						case 0x01:// Flash
							if (gc_WriteProof) 
							{
								USB_BUF_Bank0[2] = 0x80;
							}
						break;							

						case 0x02:// SD/MMC
						break;
					}				
					USB_BUF_Bank0[3] = 0x00;
				}	
				else 
				{	//mode sense(10)��Header�n8��Byte
					USB_BUF_Bank0[0] = 0x00;
					USB_BUF_Bank0[1] = 0x07;
					USB_BUF_Bank0[2] = 0x00;
					USB_BUF_Bank0[3] = 0x00;
					switch(gc_CardType)
					{
						case 0x01:// Flash
							if (gc_WriteProof) 
							{
								USB_BUF_Bank0[3] = 0x80; 
							}
						break;							

						case 0x02:// SD/MMC
						break;
					}				
				}                                                                                                      
				USB_BUF_Bank0[4] = 0x00;          
				USB_BUF_Bank0[5] = 0x00;          
				USB_BUF_Bank0[6] = 0x00;          
				USB_BUF_Bank0[7] = 0x00;          
				gc_USBStage = BulkInResponseStage;
				gbt_XferNormalResponseData_or_ModeSense = ModeSenseResponseData;
				gc_bCSWStatus = 0x00;
			}
		break;
		
		case 0x1E:                                          // �iPrevent allow medium removal�j
			if (COMD_IN[4] & 0x01) 
			{
				gc_bCSWStatus = 0x01;                          //  �Y�]�w��0x01�A�hwindows�|�h�X���~���T��(���o�O���`��)
				gc_RequestSenseCode = 0x03;
			}
			else 
			{
				gc_bCSWStatus = 0x00;
			}
			gc_USBStage = CSWStage;                            	// '7'  �����Ǧ^CSW Stage
		break;
		
		case 0x1B:                                          	// �iStart Stop UNIT�j
		    gc_USBStage=CSWStage;                   			// '7'  �����Ǧ^CSW Stage 
		    gc_bCSWStatus=0x00;                     			//  �Y�]�w��0x01�A�hwindows�|reject error���T��(���o�O���`��)
		    if(COMD_IN[4]&0x01)
			{                				//Start Unit	
		    	gc_CardExist |= gc_CardType;
		    	gc_Start_Stop_Flag &=~gc_CardType;
		     }                       			
			else
			{
		    	gc_CardExist &= ~gc_CardType;
		    	gc_Start_Stop_Flag |=gc_CardType;
		     }
		      break;
		
		case 0x23: 
			if(gc_CardExist&gc_CardType)
			{					
				SetReadFormatCapacity();
				gc_USBStage = BulkInResponseStage;				// '4'                 
     	    	gc_bCSWStatus = 0x00;				            //DeviceOK(�� card out)
				gc_RequestSenseCode = 0xFF;			        	//normal
     	    }
     	    else
			{
     	    	gbt_StallWhichEndPoint = StallEndPoint1;        // for bulk in stall
				gc_USBStage = BulkStallStage;                   // '7'
				gc_bCSWStatus = 0x01;				            //Command Passed ��BulkOnly P.15�����U�ؽX������
			    gc_RequestSenseCode = 0x00;			        	//�d�����s�b     
     	    }
     	    if(gbt_StorageMedia_has_changed) 
			{
				if(gc_CardChangeStage&gc_CardType)
				{  // Media change
					gc_bCSWStatus = 0x01;     
					gc_RequestSenseCode = 0x01 ; 
				}				
			}
			break;

		case 0x25:
			if(gc_CardExist&gc_CardType)
			{
				SetReadCapacityData();				        
				gc_USBStage = BulkInResponseStage;				// '4'                 
     	    	gc_bCSWStatus = 0x00;				       
				gc_RequestSenseCode = 0xFF;			        	//normal
     	    }
     	    else
			{
     	    	gbt_StallWhichEndPoint = StallEndPoint1;        // for bulk in stall
				gc_USBStage = BulkStallStage;                   // '7'
				gc_bCSWStatus = 0x01;				            //Command Passed ��BulkOnly P.15�����U�ؽX������
			    gc_RequestSenseCode = 0x00;			        	//�d�����s�b     
     	    }
     	    if (gbt_StorageMedia_has_changed) 
			{  							
				if(gc_CardChangeStage&gc_CardType)
				{  // Media change
					gc_bCSWStatus = 0x01;     
					gc_RequestSenseCode = 0x01 ; 
				}				
			}	    
		break;

        case 0x2F:
            gc_USBStage=CSWStage;
            gc_bCSWStatus=0x00;                     // Command Passed ��BulkOnly P.15�����U�ؽX������
            gc_RequestSenseCode = 0xFF;             // normal
   	    break;

        case 0x2E:
            TotalXferPageNum.BY[0]=COMD_IN[7];            
            TotalXferPageNum.BY[1]=COMD_IN[8];             
     	    gc_USBStage = BulkOutWriteBufferStage;
     	    gc_bCSWStatus=0x00;                     
     	    gc_RequestSenseCode = 0xFF;                   	    	       
     	    break;

        case 0x35:
            gc_USBStage=CSWStage;                   // '7'  �����Ǧ^CSW Stage 
      	    gc_bCSWStatus=0x01;                     //  �Y�]�w��0x01
            gc_RequestSenseCode = 0x02;             // ������]���~�A�]OPcode���䴩        	
            break;

        case 0x3C:
     		TotalXferPageNum.BY[0]=COMD_IN[7];            
            TotalXferPageNum.BY[1]=COMD_IN[8];            
     	    gc_USBStage = BulkInReadBufferStage ;
     	    gc_bCSWStatus=0x00;                           
     	    gc_RequestSenseCode  = 0xFF;       	      
     	    break;

		case 0xFE:{	// ���f�禡
				if (COMD_IN[1] != 0xFE) {
			       	gc_USBStage = BulkStallStage;               // '7'
			       	gc_bCSWStatus = 0x01;                       // Command Failed ��BulkOnly P.15�����U�ؽX������
			       	gc_RequestSenseCode = 0x02;                 // ������]���~�A�]OPcode���䴩                               
				}
				else {
			       	SelfTestCmd();           
		       		gc_USBStage = CSWStage;                     // ��Ƴ��ǧ��F,�i�J�U�@stage==>Comand Status Wrapper
				}
				break;
		}
		case 0x55:{
      			gc_USBStage=CSWStage;                          // ��Ƴ��ǧ��F,�i�J�U�@stage==>Comand Status Wrapper
      			gc_bCSWStatus=0x00;                            //  DeviceOK(�� card out)
                gc_RequestSenseCode = 0xFF ;                   //  normal
      			break;
      	}
		case 0x43:{  //CD ROM											//Read TOC/PMA-ATIP
			if(COMD_IN[1]==0x02){
				SetTOC_PMA_ATIP_Data(36);					// ��n�^��Host����Ƴ]�n�A����N��host�V�ڭn���
				gdw_dCBW_DataXferLength.WD[1] = 36;
			}
			else{
				SetTOC_PMA_ATIP_Data(12);
				gdw_dCBW_DataXferLength.WD[1] = 12;
			}
			gc_USBStage = BulkInResponseStage;				//'4'
			gc_bCSWStatus = 0x00;
			break;
		}     	
        default:{
               if (gbt_bmCBWFlag) {
               		gbt_StallWhichEndPoint = StallEndPoint1;    // for bulk in stall
               }
               else{
               		gbt_StallWhichEndPoint = StallEndPoint2;    // for bulk in stall
               }
               gc_USBStage = BulkStallStage;                   // '7'
               gc_bCSWStatus = 0x01;                           // Command Failed ��BulkOnly P.15�����U�ؽX������
               gc_RequestSenseCode = 0x02;                     // ������]���~�A�]OPcode���䴩
               break;
        }
	}
}


void SetResponseINQUIRYData(void) 
{ 
	U8	i;

	switch(gc_CardType)
	{  // gc_CardType  bit 0 : flash  bit 1: SD/MMC
		case 0x02 :
			for(i=0;i<36;i++)
			{	// �з�InQuiry���׬� 36 bytes
				USB_BUF_Bank0[i]=Standard_INQUIRY_Format_SDMMC[i];   // COM_IN[4]=0x24=36
    		}       
		break;

		default:
    	   	for(i=0;i<36;i++)
			{	// �з�InQuiry���׬� 36 bytes
				if((gbt_enableCDROM==1)&&(gc_bCBWLUN==2))
				{
					USB_BUF_Bank0[i]=CDROM_User_INQUIRY_Format[i];   // COM_IN[4]=0x24=36
				}
				else
				{
	      	 		USB_BUF_Bank0[i]=Standard_INQUIRY_Format[i];   // COM_IN[4]=0x24=36
    	   		}         	      		
    	   	}         	      		
    	break;
     }		
}

/***************************************************************************
* NAME:         void SetRequestSenseData(void)
* DESCRIPTION:
*               SCSI command OPcode0x03
*      //      0     1     2     3     4     5     6     7     8     9  
*      //   0x70, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0C, 0x00, 0x00, 
*      //     10    11    12    13    14    15    16    17
*      //   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,   //byte7=0x0C==>�p��0x00 USB HOST�|�����`
***************************************************************************/
void SetRequestSenseData(void) {

	U8	i;

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
			gbt_StorageMedia_has_changed=0;
			gc_CardChangeStage&=~gc_CardType;
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

		case 0x88:
			USB_BUF_Bank0[ 2] = 0x00;      			    	//Medium Error      	  	 
			USB_BUF_Bank0[12] = gc_ST_ErrorCode;            //Vender  CMD Status		  
		break;      	         
	}	
	gc_RequestSenseCode=0xFF;
}

/*********************************************************************************
* NAME:         void SetReadFormatCapacity(void)
* DESCRIPTION:
*********************************************************************************/
void SetReadFormatCapacity(void) {	
	SetPartitionCapacity();

	USB_BUF_Bank0[ 0] = 0x00;
	USB_BUF_Bank0[ 1] = 0x00;
	USB_BUF_Bank0[ 2] = 0x00;
	USB_BUF_Bank0[ 3] = 0x08;
	USB_BUF_Bank0[ 8] = 0x02;						//Format Media
	USB_BUF_Bank0[ 9] = 0x00;
	USB_BUF_Bank0[10] = 0x02;						//0x00000200 ==> 512 bytes
	USB_BUF_Bank0[11] = 0x00;
	switch(gc_CardType)
	{
		case 0x02:			
			USB_BUF_Bank0[ 4] = SD_Capacity.BY[0];					//MSB
			USB_BUF_Bank0[ 5] = SD_Capacity.BY[1];					//
			USB_BUF_Bank0[ 6] = SD_Capacity.BY[2];					//
			USB_BUF_Bank0[ 7] = SD_Capacity.BY[3];					//LSB ��@�A�]���q�@��_
			break;

		default :
			if(gbt_Support_Dummy_Capacity)
			{  // 090416 add for dummy capacity
				if(gbt_enableCDROM && (gc_bCBWLUN==2))
				{//09/04/24,joyce for CD-ROM
			        USB_BUF_Bank0[ 4] = Capacity.BY[0];				//MSB
				    USB_BUF_Bank0[ 5] = Capacity.BY[1];				//
				    USB_BUF_Bank0[ 6] = Capacity.BY[2];				//
				    USB_BUF_Bank0[ 7] = Capacity.BY[3];				//LSB ��@�A�]���q�@��_
				}
				else
				{
			        USB_BUF_Bank0[ 4] = Dummy_Capacity.BY[0];				//MSB
				    USB_BUF_Bank0[ 5] = Dummy_Capacity.BY[1];				//
				    USB_BUF_Bank0[ 6] = Dummy_Capacity.BY[2];				//
				    USB_BUF_Bank0[ 7] = Dummy_Capacity.BY[3];				//LSB ��@�A�]���q�@��_
				}						
			}
		    else
			{
				USB_BUF_Bank0[ 4] = Capacity.BY[0];					//MSB
				USB_BUF_Bank0[ 5] = Capacity.BY[1];					//
				USB_BUF_Bank0[ 6] = Capacity.BY[2];					//
				USB_BUF_Bank0[ 7] = Capacity.BY[3];					//LSB ��@�A�]���q�@��_
			}										
			break;
	}
}										
										
/***************************************************************************
* NAME:         void SetPartitionCapacity(void)
* DESCRIPTION:
*                �@ 8 bytes
***************************************************************************/
void SetPartitionCapacity(void)
{                
  	// �Ѧ��M�w���Φ��X��partitioncapacity	
	// �_�h�N���@���Flash ���@�� disk 
	if(gbt_enableCDROM && (gc_bCBWLUN==2))//09/04/24,joyce
	{
		Capacity.LW=Capacity_CDROM.LW;
	}
	else
	{
		Capacity.LW=Capacity_Remove.LW;
	}
}

/***************************************************************************
* NAME:         void SetReadCapacityData(void)
* DESCRIPTION:
*                �@ 8 bytes
***************************************************************************/
void SetReadCapacityData(void) 
{
	SetPartitionCapacity();							//�p��ұo��partitionsize�|��bLBA.LW��
	USB_BUF_Bank0[ 4] = 0x00;
	USB_BUF_Bank0[ 5] = 0x00;
	USB_BUF_Bank0[ 6] = 0x02;
	USB_BUF_Bank0[ 7] = 0x00;

	switch(gc_CardType)
	{
		case 0x02:		
			SD_Capacity.LW--;
			USB_BUF_Bank0[ 0] = SD_Capacity.BY[0];					//MSB
			USB_BUF_Bank0[ 1] = SD_Capacity.BY[1];					//
			USB_BUF_Bank0[ 2] = SD_Capacity.BY[2];					//
			USB_BUF_Bank0[ 3] = SD_Capacity.BY[3];					//LSB ��@�A�]���q�@��_			
			SD_Capacity.LW ++;
			break;
		default :											
		    if(gbt_Support_Dummy_Capacity)
			{   // 090416 add for dummy capacity
				if(gbt_enableCDROM && (gc_bCBWLUN==2))
				{//09/04/24,joyce for CD-ROM
				    Capacity.LW--;
			        USB_BUF_Bank0[ 0] = Capacity.BY[0];				//MSB
				    USB_BUF_Bank0[ 1] = Capacity.BY[1];				//
				    USB_BUF_Bank0[ 2] = Capacity.BY[2];				//
				    USB_BUF_Bank0[ 3] = Capacity.BY[3];				//LSB ��@�A�]���q�@��_
				    Capacity.LW++;
				}
				else{
			        Dummy_Capacity.LW--;
			        USB_BUF_Bank0[ 0] = Dummy_Capacity.BY[0];				//MSB
				    USB_BUF_Bank0[ 1] = Dummy_Capacity.BY[1];				//
				    USB_BUF_Bank0[ 2] = Dummy_Capacity.BY[2];				//
				    USB_BUF_Bank0[ 3] = Dummy_Capacity.BY[3];				//LSB ��@�A�]���q�@��_											
				    Dummy_Capacity.LW++;
				}
		    }
		    else
			{											
				Capacity.LW--;
				USB_BUF_Bank0[ 0] = Capacity.BY[0];					//MSB
				USB_BUF_Bank0[ 1] = Capacity.BY[1];					//
				USB_BUF_Bank0[ 2] = Capacity.BY[2];					//
				USB_BUF_Bank0[ 3] = Capacity.BY[3];					//LSB ��@�A�]���q�@��_											
				Capacity.LW++;
			}
			break;
	}	
}


void SetTOC_PMA_ATIP_Data(U8 tc_data) 
{              
	U8	i;

	for(i=0;i<tc_data;i++)
	{
		USB_BUF_Bank0[i]=TOC_PMA_ATIP_Format[i];
	}
}


bit Check_LBA_ID(void) 
{
	gbt_RW_UsbBuf =0;  // 090520 Jay add
	if ((COMD_IN[0] == 0x28) || (COMD_IN[0] == 0x2A)) 
	{
		LBA.BY[0] = COMD_IN[2];
		LBA.BY[1] = COMD_IN[3];
		LBA.BY[2] = COMD_IN[4];
		LBA.BY[3] = COMD_IN[5];      
		TotalXferPageNum.BY[0] = COMD_IN[7];
		TotalXferPageNum.BY[1] = COMD_IN[8];
	}
	else 
	{
		LBA.BY[0] = 0x00;
		LBA.BY[1] = COMD_IN[1]& 0x0F;
		LBA.BY[2] = COMD_IN[2];
		LBA.BY[3] = COMD_IN[3];      
		TotalXferPageNum.BY[0] = 0x00;    
		TotalXferPageNum.BY[1] = COMD_IN[4];
	}

	switch(gc_CardType)
	{
		case 2:
			if(LBA.LW>= SD_Capacity.LW )
			{
				LBA.LW = SD_Capacity.LW ;
				gc_ErrorCode = ID_NOT_FND;
				return 0;		
			}
			else
			{
				return 1;
			}
		break;
		default:
		    if(gbt_Support_Dummy_Capacity)
			{
		        if(LBA.LW>= Dummy_Capacity.LW)
				{
				    LBA.LW = Dummy_Capacity.LW;
				    gc_ErrorCode = ID_NOT_FND;
				    return 0;		
			    }	
				else if(LBA.LW>=Capacity.LW)
				{
					gbt_RW_UsbBuf=1; //090520 Jay add
					return 0;
				}
			    else
				{
				    return 1;
			    }		        
		    }
		    else
			{
				if(LBA.LW>= Capacity.LW){
					LBA.LW = Capacity.LW;
					gc_ErrorCode = ID_NOT_FND;
					return 0;		
				}	
				else{
					return 1;
				}
			}
			break;
	}
}


void StallBulkRW(void) 
{
	if (gbt_StallWhichEndPoint == StallEndPoint1) {  // for bulk in stall
		     // �ˬd�O�_EndPoint1 ��Stall�O�_�w�g�M���F
			// ���� Device �|���� HOST �o�� clear feature ���_�C
		USB_REG[0xEB] = 0x01;
		while (USB_REG[0xEE] == 0)
		{
			USB_PlugDetect();
			if(!gbt_USB_Detected) break;
		}
		USB_REG[0xE8] = 0x04;//in stall
	}
	if (gbt_StallWhichEndPoint == StallEndPoint2) {  // for bulk out stall
		// �ˬd�O�_EndPoint1 ��Stall�O�_�w�g�M���F
		// fw �A���椤�_�{��
		USB_REG[0xEB] = 0x02;
		while (USB_REG[0xEE] == 0)
		{
			USB_PlugDetect();
			if(!gbt_USB_Detected) break;
		}
		USB_REG[0xE8] = 0x08;//out stall
	}
}
