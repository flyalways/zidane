#include "SPDA2K.h"

void Control_Transfer_Process(void) 
{
	if (gbt_EP0SETUP_int_flag){	                         //EP0SETUP
		USB_REG[0xC0] &= 0xFE;
   		Read_Setup_Packet();	                 //Get the 8-byte setup packet and decode these commands   		 		
   		gbt_EP0SETUP_int_flag = 0;
   		return;
   	}
	if (gbt_EP0TX_int_flag){								// EP0TX     
		while(USB_REG[0xA0]&0x01)
		{
			USB_PlugDetect();
			if(!gbt_USB_Detected) break;
		}
   		USB_REG[0xC0] &= 0xFD;   		
   		gbt_EP0TX_int_flag = 0;	
   		return;
   	}
   	if (gbt_EP0RX_int_flag){	                         // EP0RX    IN Packet    // DEVICE <===== HOST   			
		while(USB_REG[0xA0]&0x02)
		{
			USB_PlugDetect();
			if(!gbt_USB_Detected) break;
		} 
     	USB_REG[0xC0] &= 0xFB;		
		gbt_EP0RX_int_flag = 0;
		return;
	}
	if (gbt_USBRST_int_flag) 
	{                             // USB Reset & Software Reset   	
		gbt_USBRST_int_flag = 0;   	                
		gbt_Have_set_configuration= 0;
		GLOBAL_REG[0x02]|= 0x04;	  //S/W RST USB Module
		GLOBAL_REG[0x02] = 0x02;							// 081015 Joyce fix don't reset DSP
		USB_I2C();	    
	    if(gbt_Force_USB_FullSpeed)
		{
	    	USB_REG[0x50]    = 0x01;	//USB 1.1
	    }
		else
		{
	    	USB_REG[0x50]    = 0x00;	//Default USB 2.0
	    }	
		USB_REG[0x27] = 0x00;   // 080304 Jay fix for new fpga code	 
		USB_REG[0xC0]   &= 0xF7; 
		USB_REG[0xE9]    = 0xFF;		//Clear ALL USB Buffer.
		USB_REG[0xEA]    = 0xFF;
		USB_REG[0xA0]   &= 0xFC;
		USB_REG[0xD2]    = 0x02;		//因為Reset完後要收Host端的Command.		
		USB_REG[0xA1]    = 0x02;
		return;
	}

	if (gbt_USB_CONFIG_Chg_int_flag) 
	{
		gbt_USB_CONFIG_Chg_int_flag = 0;
		USB_REG[0xC0] = 0x4F;
		if (USB_REG[0xB1] & 0xF0) 
		{
			USB_REG[0xD0] |= 0x10;
		}
		gbt_Have_set_configuration = 0;
		return;
	}

	if (gbt_SUSPEND_int_flag) 
	{			//Suspend status
		gbt_SUSPEND_int_flag = 0;
		USB_REG[0xC0] = 0x4F;
		if (USB_REG[0xB1] & 0xF0) 
		{
			USB_REG[0xD0] |= 0x10;
		}

		if(gbt_USB_Connect_Flag)
		{  // 081208 Jay add
		    gbt_USER_Remove_Flag=1;
		}	
		return;
   	}
	if (gbt_RESUME_int_flag) 
	{			//Resume status
        gbt_RESUME_int_flag   = 0;
		USB_REG[0xC0]  = 0xEF;	//Clear Suspend event
		USB_REG[0xD0] &= 0xEF;	//Disable Suspend INT
		USB_REG[0xD0] |= 0x80;	//Enable Resume INT
                gbt_USB_Connect_Flag    = 0;  // 081208 Jay add
	        gbt_USER_Remove_Flag    = 0;// 081208 Jay
   		return;
	}
}


void Read_Setup_Packet(void) 
{
	//Read(Get) 8-byte setup packet from FIFO
	//Endpoint 0 Data Port Register = 0x0C
   	ControlCmd.RequestType         =  USB_REG[0xF0];               // get 1st byte
   	ControlCmd.Direction           =  ControlCmd.RequestType & 0x80;// xfer Direction(IN, OUT)
	ControlCmd.Type                =  ControlCmd.RequestType & 0x60;// type(Standard, Class, Vendor)
   	ControlCmd.Object              =  ControlCmd.RequestType & 0x03;// Device, Interface, Endpoint
	ControlCmd.Request             =  USB_REG[0xF1];               // get 2st byte
    ControlCmd.Value_L             =  USB_REG[0xF2];               // get 3st byte
    ControlCmd.Value_H             =  USB_REG[0xF3];               // get 4th byte  
    ControlCmd.Index_L             =  USB_REG[0xF4];               // get 5th byte
   	ControlCmd.Index_H             =  USB_REG[0xF5];               // get 6th byte
   	ControlCmd.Length.BY[1]        =  USB_REG[0xF6];               // get 7th byte
   	ControlCmd.Length.BY[0]        =  USB_REG[0xF7];               // get 8th byte  		
	if (ControlCmd.Type == 0x00) {        		                // standard command
  	      USB_REG[0xEA]=0x01;
  	      StandardCommand();
		gbt_USB_Connect_Flag =1; //090521 Jay add for usb connect
  	}	
  	else if (ControlCmd.Type == 0x10){                              // class command
                   //ClassCommand();
    }              
    else if (ControlCmd.Type == 0x20){                         // vendor command
      	         USB_REG[0xEA]=0x01; 
		VendorCommand();									// GetMaxLogicUnitNumber is 0xA1 0xFE 0 0 0 0 0x01 0
    }
  	else{
   		gc_CX_Status_flag |= CX_STL;                       // Invalid(bad) command, Return EP0_STALL flag 
  	}
}


void StandardCommand(void) 
{
	switch (ControlCmd.Request) 
	{                      // by Standard Request codes   	
   		case 0:		                         
   			get_status();                    // get status
   			gc_CX_Status_flag |= CX_DONE;       // EP0_Done
   			break;                           // EP0 Control Register's address --> 0x0B
		case 1:		    
   			clear_feature();                 // clear feature
			gc_CX_Status_flag |= CX_DONE;
			break;
   		case 2:		    
   			gc_CX_Status_flag |= CX_DONE;       // Reserved for future use
   			break;
   		case 3:		    
			set_feature();
			break;
   		case 4:		                         // Reserved for future use
			gc_CX_Status_flag |= CX_DONE;
			break;
   		case 5:
   		  	set_address();    		 // set address
			gc_CX_Status_flag |= CX_DONE;
			break;
		case 6:		   	
			get_descriptor();                // get descriptor
			gc_CX_Status_flag |= CX_DONE;
			break;
        case 7:		   
            set_descriptor();                // set descriptor
			gc_CX_Status_flag |= CX_DONE;
			break;
		case 8:		   
			get_configuration();             // get configuration 
			gc_CX_Status_flag |= CX_DONE;
			break;
		case 9:		   
			set_configuration();             // set configuration
			gc_CX_Status_flag |= CX_DONE;
			break;
		case 10:   	                         
        	get_interface();                 // get interface
			gc_CX_Status_flag |= CX_DONE;
			break;
      	case 11:	   
			set_interface();                 // set interface
			gc_CX_Status_flag |= CX_DONE;
			break;
		case 12:	                         
			synch_frame();                   // synch frame
			gc_CX_Status_flag |= CX_DONE;
			break;
		default:
   			gc_CX_Status_flag |= CX_STL;        // Invalid(Bad) command, Return EP0_STALL flag
   	}
}


void get_status(void)
{
	U8	i;
	
	switch (ControlCmd.Object)
	{ //Judge which recipient type is at first    
		case 0 ://Device
			//Return 2-byte's Device status (Bit1:Remote_Wakeup, Bit0:Self_Powered) to Host
			//Notice that the programe sequence of RecipientStatus
			//Bit1:Device_Remote_Wakeup(RMWKUP)", Main Control Register's address --> 0x00		
			if(USB_REG[0xC0]&0x40)
			{
				if(Configuration_DescriptorTable[0x07] & 0x40)
				{
					i=3;
				}
				else
				{
					i=2;
			    }
			}
			else
			{
				if(Configuration_DescriptorTable[0x07] & 0x40)
				{
					i=1;
				   }
				else
				{
					i=0;
				}
			}
			USB_REG[0x00]=i; 
			USB_REG[0x00]=0x00;	
		break;   
       		
 		case 1 ://Interface
			//Return 2-byte Interface status to Host Endpoint 0 Data Port Register = 0x0C
			USB_REG[0x00] = 0x00;//Zero_packet;         //Low Byte
			USB_REG[0x00] = 0x00;//Zero_packet;         //High Byte
		break;
      		
		case 2 ://Endpoint
			switch(ControlCmd.Index_L)
			{
				case 0x01:	//OUT
				break;	        			
     			
				case 0x81:	//IN
					USB_REG[0x00] = ((USB_REG[0xE8] & 0x04) >> 2);        //Low Byte
					USB_REG[0x00] = 0x00;//Zero_packet;	                //High Byte
				break;

				case 0x02:	//OUT
					USB_REG[0x00] = ((USB_REG[0xE8] & 0x08) >> 3);        //Low Byte
					USB_REG[0x00] = 0x00;//Zero_packet;	                //High Byte
				break;        			
      			
				case 0x82:	//IN
				break;

				case 0x83:
					USB_REG[0x00] = ((USB_REG[0xE8] & 0x10) >> 4);
					USB_REG[0x00] = 0x00;
				break;

				default:
					gc_CX_Status_flag |= CX_STL;
				break;
        	}
        break;

		default :
   			gc_CX_Status_flag |= CX_STL;
		break;
	}
}


void clear_feature(void)
{
    switch (ControlCmd.Value_L){    
    	case 0 :   		
        	//Clear "Endpoint_Halt", Turn off the "STALL" bit in Endpoint Control Function Register
        	switch(ControlCmd.Index_L){        	
        		case 0x01:	//OUT,No Used
        			break;
        		case 0x81:	//IN
        			//Clear Toggle Bit
        			//IN:TX Endpoint 1 Config and Status Register's address --> 0x41
        			//Bit 4 : CLRTG_EP1TX
        			//Bit 3 : STALL_EP1TX
       				USB_REG[0xE9] |= 0x01;     // Reset Toggle Seeting
       				USB_REG[0xEB] &= 0xFB;     // Clear Stall Bit
       				USB_REG[0xE9] &= 0xFE;     // Disable Toggle Seeting
        			break;
        		        	
			case 0x02:	//OUT
        			//Clear Toggle Bit
        			//OUT:RX Endpoint 2 Config and Status Register's address --> 0x63
        			//Bit 4 : CLRTG_EP2RX
        			//Bit 3 : STALL_EP2RX 
       			        USB_REG[0xE9] |= 0x02;     // Reset Toggle Seeting
       			        USB_REG[0xEB] &= 0xF7;     // Clear Stall Bit
       			        USB_REG[0xE9] &= 0xFD;     // Reset Toggle Seeting
        			break;
        		case 0x82:	//IN
        			break;
        		case 0x83:
        			USB_REG[0xE9] |= 0x04;     // Reset Toggle Seeting
       				USB_REG[0xEB] &= 0xEF;     // Clear Stall Bit
       				USB_REG[0xE9] &= 0xFB;     // Disable Toggle Seeting        			
        			break;
        		default:
        			// Invalid(bad) command, Return EP0_STALL flag
				// EndPoint 0 Control Register's address --> 0x2C
  				gc_CX_Status_flag |= CX_STL;
			}
                break;
            		
	case 1 :// Device Remote Wakeup
      		// Clear "Device_Remote_Wakeup", Turn off the"RMWKUP" bit in Main Control Register
      		// Main Control Register's address --> 0x00
        	// Bit 0 : RMWKUP
        	USB_REG[0xC0] &= 0xBF; 
        	gc_CX_Status_flag |= CX_DONE;
        	break;
	case 2 :// Test Mode
		// The Test_Mode feature cannot be cleared by the ClearFeature() request.
		// Invalid(bad) command, Return EP0_STALL flag
		// EndPoint 0 Control Register's address --> 0x0B
   		gc_CX_Status_flag |= CX_STL;
	       	break;
        default :
        	// Invalid(bad) command, Return EP0_STALL flag
        	// EndPoint 0 Control Register's address --> 0x0B
   			gc_CX_Status_flag |= CX_STL;
	}
}


/*****************************************************************************
* NAME:         void set_feature(void)
* DESCRIPTION:
*               Setup -- Standard Command -- set feature (0x02)
*****************************************************************************/
void set_feature(void){
    switch (ControlCmd.Value_L){ //Judge which recipient is at first    
    	case 0 :   		// Endpoint Halt
        	//Set "Endpoint_Halt", Turn on the "STALL" bit in Endpoint Control Function Register
        	switch(ControlCmd.Index_L){        	
        		case 0x01:	//OUT
        			//OUT:RX Endpoint 1 Config and Status Register's address --> 0x61
        			//Bit 2 : STALL_EP1RX
        			break;
			case 0x81:	//IN
        			//N:TX Endpoint 1 Config and Status Register's address --> 0x41
					//Bit 2 : STALL_EP1RX/
        			USB_REG[0xE8] |= 0x04;
					gc_CX_Status_flag |= CX_DONE;
        			break;
			case 0x02:	//OUT
        			//OUT:RX Endpoint 2 Config and Status Register's address --> 0x63
        			//Bit 2 : STALL_EP2RX
        			USB_REG[0xE8] |= 0x08;
					gc_CX_Status_flag |= CX_DONE;
	       			break;
			case 0x82:	//IN
        			//IN:TX Endpoint 2 Config and Status Register's address --> 0x43
        			//Bit 2 : STALL_EP2RX
        			break;
        		case 0x83:
        			USB_REG[0xE8] |= 0x10;
					gc_CX_Status_flag |= CX_DONE;
        			break;
        		default:
        			// Invalid(bad) command, Return EP0_STALL flag
				// EndPoint 0 Control Register's address --> 0x0B
   				gc_CX_Status_flag |= CX_STL;
   				return;
        	}
                break;
           	
 	case 1 ://Device Remote Wakeup
      		//Set "Device_Remote_Wakeup", Turn on the"RMWKUP" bit in Mode Register
      		//Main Control Register's address --> 0x00
        	//Bit 0 : RMWKUP
        	USB_REG[0xC0] |= 0x40; 
        	gc_CX_Status_flag |= CX_DONE;     // EP0_Done
            break;

	default :
        	// Invalid(bad) command, Return EP0_STALL flag
        	// EndPoint 0 Control Register's address --> 0x0B
			gc_CX_Status_flag |= CX_STL;   
    }
}


void set_address(void){  // HW做掉了
  	//the legal device address is from 0 to 127
        if (ControlCmd.Value_L > 127){        
    	      // Illegal device address
    	      // Invalid(bad) command, Return EP0_STALL flag
	      // EndPoint 0 Control Register's address --> 0x0B
   		 gc_CX_Status_flag |= CX_STL;
   		 return;
        }
        // Write this 2-byte gc_Device_Address into "Address Register"
        // Address Register's address --> 0x01
       	if (gbt_Have_set_configuration){
              gc_Device_Address = ControlCmd.Value_L;
              gbt_Have_set_configuration = 1;
        }
        else{
        	gc_Device_Address = ControlCmd.Value_L;
        }     
}


void get_descriptor(void) 
{
	U8	i;

	//Decide the display descriptor length(range)
	if (USB_REG[0x02] & 0x01) 
	{   
		gbt_USBHostIsHighSpeed = 1;
	}
	else 
	{
		gbt_USBHostIsHighSpeed = 0;
	}	
	
	if (gbt_USBHostIsHighSpeed) 
	{
	   	EndPoint0Table[Device_DescriptorTable_Begine        +  2] = 0x00;
	   	EndPoint0Table[Device_DescriptorTable_Begine        +  3] = 0x02; // USB 2.0
	   	EndPoint0Table[Configuration_DescriptorTable_Begine + 22] = 0x00;
	   	EndPoint0Table[Configuration_DescriptorTable_Begine + 23] = 0x02; // for endpoint 1 , MaxPacket 512 bytes
       	EndPoint0Table[Configuration_DescriptorTable_Begine + 29] = 0x00;
       	EndPoint0Table[Configuration_DescriptorTable_Begine + 30] = 0x02; // for endpoint 1 , MaxPacket 512 bytes
    }
    else
	{
        EndPoint0Table[Other_Speed_Configuration_DescriptorTable_Begine+22] = 0x00;
        EndPoint0Table[Other_Speed_Configuration_DescriptorTable_Begine+23] = 0x02;
        EndPoint0Table[Other_Speed_Configuration_DescriptorTable_Begine+29] = 0x00;
        EndPoint0Table[Other_Speed_Configuration_DescriptorTable_Begine+30] = 0x02;
    }    
    
    switch (ControlCmd.Value_H) 
	{
		case 1:		//Device descriptor
			for(i=Device_DescriptorTable_Begine;i<Device_DescriptorTable_End;i++) 
			{			
				USB_REG[0x00]=EndPoint0Table[i];
				ControlCmd.Length.WD--;
				if (ControlCmd.Length.WD == 0) 
				{
					break;
				}
			}	                
		break;
		
		case 2:	 	//Configuration descriptor
			for(i=Configuration_DescriptorTable_Begine;i<Configuration_DescriptorTable_End;i++) 
			{
				USB_REG[0x00]=EndPoint0Table[i];
				ControlCmd.Length.WD--;
				if (ControlCmd.Length.WD == 0) 
				{
					break;
				}
			}
		break;
	
		case 3:		//String descriptor
			switch(ControlCmd.Value_L) 
			{			
				case 0x00:
					for(i=iLANGID_DescriptorTable_Begine;i<iLANGID_DescriptorTable_End;i++) 
					{                                        
						USB_REG[0x00]=EndPoint0Table[i];
						ControlCmd.Length.WD--;
						if (ControlCmd.Length.WD == 0)
						{  
							break;
						}		                                
					}
				break;

				case 0x10:
					for(i=iManufacturer_DescriptorTable_Begine;i<iManufacturer_DescriptorTable_End;i++) 
					{                                        
						USB_REG[0x00]=EndPoint0Table[i];
						ControlCmd.Length.WD--;
						if (ControlCmd.Length.WD== 0)
						{  
							break;
						}
					}
				break;
				
				case 0x20:
					for(i=iProduct_DescriptorTable_Begine;i<iProduct_DescriptorTable_End;i++) 
					{                                        
						USB_REG[0x00]=EndPoint0Table[i];
						ControlCmd.Length.WD--;
						if (ControlCmd.Length.WD== 0)
						{  
							break;
						}
					}
				break;
				
				case 0x30:
					gc_USB_ADDR =(gc_USB_ADDR&0x7F);
					for(i=iSerialNumber_DescriptorTable_Begine;i< iSerialNumber_DescriptorTable_End;i++) 
					{                                        
						if(i==(iSerialNumber_DescriptorTable_End-3))
						{
                        	//SN 需介於 0x30~0x39 / 0x41 ~ 0x49 
                            USB_REG[0x00]= (gc_USB_ADDR/100)+0x30;                                               	//
                        }
                        else if(i==(iSerialNumber_DescriptorTable_End-2))
						{
                        	if((gc_USB_ADDR&0x7F) <= 0x09 )
							{
                        		USB_REG[0x00] =0x30;
                        	}
                        	else
							{                       	
                        		USB_REG[0x00]= ((gc_USB_ADDR%100)/10)+0x40;
                        	}
                        }
                        else if(i == (iSerialNumber_DescriptorTable_End-1))
						{ 
                        	USB_REG[0x00]= (gc_USB_ADDR%10)+0x30;     
                        }                                                                          	//
                        else
						{
							USB_REG[0x00]=EndPoint0Table[i];
                        }						
						ControlCmd.Length.WD--;
						if (ControlCmd.Length.WD== 0)
						{  
							break;
						}
					}
				break;

				case 0x40:
					for(i=iConfiguraiton_DescriptorTable_Begine;i<iConfiguraiton_DescriptorTable_End;i++) 
					{                                        
						USB_REG[0x00]=EndPoint0Table[i];
						ControlCmd.Length.WD--;
						if (ControlCmd.Length.WD== 0)
						{  
							break;
						}
					}
				break;

				case 0x50:
					for(i=iDeviceQualifierString_DescriptorTable_Begine;i<iDeviceQualifierString_DescriptorTable_End;i++) 
					{                                        
						USB_REG[0x00]=EndPoint0Table[i];
						ControlCmd.Length.WD--;
						if (ControlCmd.Length.WD== 0)
						{  
							break;
						}
					}
				break;

				case 0x60:
					for(i=iInterface_DescriptorTable_Begine;i<iInterface_DescriptorTable_End;i++)
					{                                        
						USB_REG[0x00]=EndPoint0Table[i];
						ControlCmd.Length.WD--;
						if (ControlCmd.Length.WD == 0) 
						{  
							break;
						}
					}
				break;
			}
		break;	

		case 4:		//Interface descriptor
			//It cannot be accessed individually, it must follow "Configuraton"
		break;
		
		case 5: 	//Endpoint descriptor
			//It cannot be accessed individually, it must follow "Configuraton"
		break;
		
		case 6:		//Device_Qualifier descritor
			for(i=DeviceQualifier_DescriptorTable_Begine;i<DeviceQualifier_DescriptorTable_End;i++) 
			{
				USB_REG[0x00]=EndPoint0Table[i];
				ControlCmd.Length.WD--;
				if (ControlCmd.Length.WD == 0) 
				{  
					break;
				}
			}
		break;

		case 7:   	// Other_Speed_Configuration
			for(i=Other_Speed_Configuration_DescriptorTable_Begine;i<Other_Speed_Configuration_DescriptorTable_End;i++)
			{                        
				USB_REG[0x00]=EndPoint0Table[i];
				ControlCmd.Length.WD--;
				if (ControlCmd.Length.WD== 0)
				{  
					break;
				}
			}
		break;

		case 8:        // INTERFACE_POWER
		break;

		default:
			// Invalid(bad) command, Return EP0_STALL flag 
			// EndPoint 0 Control Register's address --> 0x0B
			gc_CX_Status_flag |= CX_STL;
		break;
	}
}


void set_descriptor(void)
{
#if 0
	U8	i;

	switch (ControlCmd.Value_H)
	{
		case 1:		// device descriptor
			for(i=Device_DescriptorTable_Begine;i<Device_DescriptorTable_End;i++)
			{
				EndPoint0Table[i]=USB_REG[0x00];
				ControlCmd.Length.WD--;
				if(ControlCmd.Length.WD == 0)
				{
					break;
				}
			}
		break;

		case 2:	 	// configuration descriptor
			for(i=Configuration_DescriptorTable_Begine;i<Configuration_DescriptorTable_End;i++) 
			{                        
				EndPoint0Table[i]=USB_REG[0x00];
				ControlCmd.Length.WD--;
				if ( ControlCmd.Length.WD == 0)
				{
					break;
				}
			}
		break;

		case 3:   	// string descriptor
			switch (ControlCmd.Value_L) 
			{
				case 0x00:
			 		for(i=iLANGID_DescriptorTable_Begine;i<iLANGID_DescriptorTable_End;i++)
					{                                        
						EndPoint0Table[i]=USB_REG[0x00];
						ControlCmd.Length.WD--;
						if ( ControlCmd.Length.WD == 0)
						{
							break;
						}
					}
				break;

				case 0x10:
			 		for(i=iManufacturer_DescriptorTable_Begine;i<iManufacturer_DescriptorTable_End;i++)
					{                                        
						EndPoint0Table[i]=USB_REG[0x00];
						ControlCmd.Length.WD--;
						if ( ControlCmd.Length.WD == 0)
						{
							break;
						}
					}
				break;

				case 0x20:
					for(i=iProduct_DescriptorTable_Begine;i<iProduct_DescriptorTable_End;i++)
					{                                        
						EndPoint0Table[i]=USB_REG[0x00];
						ControlCmd.Length.WD--;
						if ( ControlCmd.Length.WD == 0)
						{
							break;
						}
					}
				break;
			 	
				case 0x30:
			 		for(i=iSerialNumber_DescriptorTable_Begine;i<iSerialNumber_DescriptorTable_End;i++)
					{                                        
						EndPoint0Table[i]=USB_REG[0x00];
			  			ControlCmd.Length.WD--;
               			if ( ControlCmd.Length.WD == 0)
						{
               				break;
               			}
	                }
		 		break;

			 	case 0x40:
			 		for(i=iConfiguraiton_DescriptorTable_Begine;i<iConfiguraiton_DescriptorTable_End;i++)
					{                                        
						EndPoint0Table[i]=USB_REG[0x00];
	        			ControlCmd.Length.WD--;
               			if ( ControlCmd.Length.WD == 0)
						{
		               		break;
		               	}
					}
		 		break;

			 	case 0x50:
			 		for(i=iDeviceQualifierString_DescriptorTable_Begine;i<iDeviceQualifierString_DescriptorTable_End;i++)
					{                                        
						EndPoint0Table[i]=USB_REG[0x00];
	        			ControlCmd.Length.WD--;
               			if ( ControlCmd.Length.WD == 0)
						{
		               		break;
		               	}
					}
				break;

				case 0x60:
					for(i=iInterface_DescriptorTable_Begine;i<iInterface_DescriptorTable_End;i++)
					{                                        
						EndPoint0Table[i]=USB_REG[0x00];
	        			ControlCmd.Length.WD--;
               			if ( ControlCmd.Length.WD == 0)
						{
		               		break;
		               	}
			}
			break;	
			}
		break;	

		case 4:   	// interface descriptor
        		//It cannot be accessed individually, it must follow "Configuraton"
        break;

		case 5:   	// endpoint descriptor
			//It cannot be accessed individually, it must follow "Configuraton"
        break;

		case 6:   	// Device_Qualifier descritor
			for(i=DeviceQualifier_DescriptorTable_Begine;i<DeviceQualifier_DescriptorTable_End;i++)
			{                        
				EndPoint0Table[i]=USB_REG[0x00];
	        	ControlCmd.Length.WD--;
               	if ( ControlCmd.Length.WD == 0)
				{
					break;
				}
			}
		break;

		case 7:   	// Other_Speed_Configuration
			for(i=Other_Speed_Configuration_DescriptorTable_Begine;i<Other_Speed_Configuration_DescriptorTable_End;i++)
			{                        
				EndPoint0Table[i]=USB_REG[0x00];
	        	ControlCmd.Length.WD--;
               	if ( ControlCmd.Length.WD == 0)
				{
					break;
				}
			}
		break;

		case 8:        // INTERFACE_POWER
		break;

		default:
			// Invalid(bad) command, Return EP0_STALL flag 
			// EndPoint 0 Control Register's address --> 0x0B
   			gc_CX_Status_flag |= CX_STL;
		break;
	}//End-of-switch loop//
#endif
}


void get_configuration(void)
{
	// Is it correct to get data in this way?
	// Offset 5, bConfigurationValue of Table 9-10 Standard Configuration Descriptor
	// It has no related define register on our FUSB200 Spec,
	// so we must keep & maintain this value on my program all the time
	// Get the Configuration Value from the descriptor and our register
	
	if (gbt_Have_set_configuration){
	      USB_REG[0x00] = EndPoint0Table[Configuration_DescriptorTable_Begine+5]; //ConfigurationValue;
	}
	else{                       // 若為Address State時的回應
	      USB_REG[0x00] = 0x00;
	}      
	// Endpoint 0 Data Port Register = 0x0C
	// If this value is zero(0), it means hadn't been configurated.
}


void set_configuration(void)
{
	// unsigned char ConfigurationValue;
	// Endpoint 0 Data Port Register = 0x00
	// It has no related define register on our FUSB200 Spec, 
	// so we must keep & maintain this value on my program all the time
	if(ControlCmd.Value_L!=0x00)
	{
	      // 必需符合Configuration_DescriptorTable的對應欄位，方可進入Configured state	
        if(EndPoint0Table[Configuration_DescriptorTable_Begine+5]==ControlCmd.Value_L)
		{		
	           gbt_Have_set_configuration = 1;
	      }
		else
		{
	           //Device should responds with a Request Error
	      }    
	}
	else
	{
	      gbt_Have_set_configuration = 0;
        }
	// Set the Configuration Value into our register
	// Offset 5, bConfigurationValue of Table 9-10 Standard Configuration Descriptor
	// If this value is zero(0), it means hadn't been configurated.
	// Must set(turn on) the after_configuration bit after setting gc_Device_Address and set_configuration
	// After_Config bit(Bit 7) in the Device address register --> 0x01
}

/*****************************************************************************
* NAME:         void get_interface(void)
* DESCRIPTION:
*               Setup -- Standard Command -- get interface (0x0A)
*****************************************************************************/
void get_interface(void){
	// If there exists many interfaces, Interface0,1,2,...N, 
	// You must check & select the specific one
	switch (ControlCmd.Index_L)          //switch by CurrentInterfaceNumber
	{	
		case 0:	// Interface0
			USB_REG[0x00] = EndPoint0Table[Configuration_DescriptorTable_Begine+12];
			// Offset 3, bAlternateSetting of Table 9-12 Standard Interface Descriptor
			break;
		case 1:	// Interface1
			break;
		case 2:	// Interface2
			break;
		default:
			break;
	}
}

/*****************************************************************************
* NAME:         void set_interface(void)
* DESCRIPTION:
*               Setup -- Standard Command -- set interface (0x0B)
*****************************************************************************/
void set_interface(void){
	// If there exists many interfaces, Interface0,1,2,...N, 
	// You must check & select the specific one
	switch (ControlCmd.Index_L)          //switch by CurrentInterfaceNumber
	{	
		case 0: // Interface0
			// Set the AlternateSetting Value into our register
			EndPoint0Table[Configuration_DescriptorTable_Begine+12] = ControlCmd.Value_L;
			// Offset 3, bAlternateSetting of Table 9-12 Standard Interface Descriptor
			// It has no related define register on our FUSB200 Spec, 
			// so we must keep & maintain this value on my program all the time
			break;
		case 1:	// Interface1
			break;		
		case 2:	// Interface2
			break;
		default:
			break;
	}
	// Clear All EPx Toggle Bit
	// OUT:RX Endpoint x Config and Status Register's address --> 0x63
        USB_REG[0xE9] |= 0x02;     // Reset Toggle Seeting
        USB_REG[0xE9] &= 0xFD;     // Reset Toggle Seeting
        // IN:TX Endpoint x Config and Status Register's address --> 0x41
        USB_REG[0xE9] |= 0x01;     // Reset Toggle Seeting
        USB_REG[0xE9] &= 0xFE;     // Disable Toggle Seeting
}

/*****************************************************************************
* NAME:         void synch_frame(void)
* DESCRIPTION:
*               Setup -- Standard Command -- synch frame(0x0C)
*****************************************************************************/
void synch_frame(void){
	//unsigned char EndpointIndex=0, TransferType=0;
	//EndpointIndex =  ControlCmd.Index_L;
	//Endpoint Index, Low Byte
	//Does the Endpoint support Isochronous transfer type? 
	switch(ControlCmd.Index_L){	
		case 1: // EP1
			// TransferType = DescriptorTable[0x27] & 0x03;
			if (0){	// Isochronous
				// Read 2-byte's Frame Number from "Frame Number Register"
				// Frame Number Register's address(Low Byte) ==> 0x04
				// Frame Number Register's address(High Byte) ==> 0x05
			}
			else{    // Not Isochronous			
				// Invalid(bad) command, Return EP0_STALL flag
        		        // EndPoint 0 Control Register's address --> 0x0B
   				gc_CX_Status_flag |=CX_STL;				
				return;
			}
			break;
		case 2: // EP2
			// TransferType = DescriptorTable[0x2E] & 0x03;
			if (0){	// Isochronous			
				// Read 2-byte's Frame Number from "Frame Number Register"
				// Frame Number Register's address(Low Byte) ==> 0x04
				// Frame Number Register's address(High Byte) ==> 0x05
			}
			else{    // Not Isochronous			
				// Invalid(bad) command, Return EP0_STALL flag
        		        // EndPoint 0 Control Register's address --> 0x0B
   				gc_CX_Status_flag |= CX_STL;
				return;
			}
			break;
	}
}


void VendorCommand(void) 
{
	switch (ControlCmd.Request) 
	{
		case 0xFE:
			USB_REG[0x00] = 1;	//0:NO SD DISK  1ㄩSD DISK			     	                         
			gc_CX_Status_flag |= CX_DONE;       // EP0_Done
		break;                           // EP0 Control Register's address --> 0x0B	

		default:
   			gc_CX_Status_flag |= CX_STL;        // Invalid(Bad) command, Return EP0_STALL flag 
		break;
   	} 
} 
