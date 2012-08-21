#include "..\header\SPDA2K.h"
#include "..\header\variables.h"


struct 
{
	U8 RequestType;
	U8 Direction;			//	1	Data transfer direction: IN, OUT
	U8 Type;				//		Request Type: Standard, Class, Vendor
	U8 Object;			//		Recipient: Device, Interface, Endpoint,other
	U8 Request;			//	2	Refer to Table 9-3
	U8 Value_L;			//	3
	U8 Value_H;			//	4
	U8 Index_L;			//	5
	U8 Index_H;			//	6
	unionU16 Length;		//		.BY[0]  Hi  byte
} ControlCmd;

void Read_Setup_Packet(void) 
{
	//Read(Get) 8-byte setup packet from FIFO
	//Endpoint 0 Data Port Register = 0x0C
	ControlCmd.RequestType=XBYTE[0xB5F0];
	ControlCmd.Type=ControlCmd.RequestType&0x60;
	ControlCmd.Object=ControlCmd.RequestType&0x03;
	ControlCmd.Request=XBYTE[0xB5F1];
	ControlCmd.Value_L=XBYTE[0xB5F2];
	ControlCmd.Value_H=XBYTE[0xB5F3];
	ControlCmd.Index_L=XBYTE[0xB5F4];
//	ControlCmd.Index_H=XBYTE[0xB5F5];
	ControlCmd.Length.BY[1]=XBYTE[0xB5F6];
	ControlCmd.Length.BY[0]=XBYTE[0xB5F7];
	if(ControlCmd.Type==0x00) 
	{        		                // standard command
		XBYTE[0xB5EA]=0x01;
		StandardCommand();
	}	
	else if(ControlCmd.Type==0x20)
	{                         // vendor command
		XBYTE[0xB5EA]=0x01; 
		if(ControlCmd.Request==0xFE)
		{
			XBYTE[0xB500]=0;				                              
			gc_CX_Status_flag|=CX_DONE;       // EP0_Done
		}    
	}
  	else
	{
		gc_CX_Status_flag|=CX_STL;                       // Invalid(bad) command, Return EP0_STALL flag 
  	}
}


void StandardCommand(void) 
{
	switch(ControlCmd.Request) 
	{                      // by Standard Request codes   	
		case 1:		    
   			clear_feature();
   			gc_CX_Status_flag|=CX_DONE;
		break;
        		
		case 6:		   	
			get_descriptor();
   			gc_CX_Status_flag|=CX_DONE;
		break;

		default :
   			gc_CX_Status_flag|=CX_DONE;
		break;
	}
}


void clear_feature(void)
{
    switch (ControlCmd.Value_L)
	{    
    	case 0 :   		
			switch(ControlCmd.Index_L)
			{
        		case 0x81:	//IN
       				XBYTE[0xB5E9] |= 0x01;     // Reset Toggle Seeting
       				XBYTE[0xB5EB] &= 0xFB;     // Clear Stall Bit
       				XBYTE[0xB5E9] &= 0xFE;     // Disable Toggle Seeting
        		break;
        		        	
				case 0x02:	//OUT
					XBYTE[0xB5E9] |= 0x02;     // Reset Toggle Seeting
					XBYTE[0xB5EB] &= 0xF7;     // Clear Stall Bit
					XBYTE[0xB5E9] &= 0xFD;     // Reset Toggle Seeting
				break;

        		case 0x83:
        			XBYTE[0xB5E9]|=0x04;     // Reset Toggle Seeting
       				XBYTE[0xB5EB]&=0xEF;     // Clear Stall Bit
       				XBYTE[0xB5E9]&=0xFB;     // Disable Toggle Seeting        			
        		break;

				default:
					gc_CX_Status_flag|=CX_STL;
				break;
        	}
                break;
            		
		case 1 :// Device Remote Wakeup
			XBYTE[0xB5C0]&=0xBF; 
			gc_CX_Status_flag|=CX_DONE;
		break;
        			
		default :
   			gc_CX_Status_flag|=CX_STL;
		break;
	}
}

void get_descriptor(void) 
{
	data	U8	i;

	gb_USBHostIsHighSpeed=(bit)(XBYTE[0xB502]&0x01);  
    switch (ControlCmd.Value_H) 
	{
		case 1:		//Device descriptor
			for(i=0;i<18;i++) 
			{			
				if(gb_USBHostIsHighSpeed)
				{
					if(i==2)
					{
						XBYTE[0xB500]=0x00;
					}
					else if(i==3)
					{
						XBYTE[0xB500]=0x02;
					}
					else
					{
						XBYTE[0xB500]=Device_DescriptorTable[i];
					}
				}
				else
				{
					XBYTE[0xB500]=Device_DescriptorTable[i];
				}
				ControlCmd.Length.WD--;
				if (ControlCmd.Length.WD==0) 
				{
					break;
				}
			}	                
		break;

		case 2:	 	//Configuration descriptor
			for(i=0;i<39;i++) 
			{
				if(gb_USBHostIsHighSpeed)
				{
					if((i==22)||(i==29))
					{
						XBYTE[0xB500]=0x00;
					}
					else if((i==23)||(i==30))
					{
						XBYTE[0xB500]=0x02;
					}
					else
					{
						XBYTE[0xB500]=Configuration_DescriptorTable[i];
					}
				}
				else
				{
					XBYTE[0xB500]=Configuration_DescriptorTable[i];
				}
				ControlCmd.Length.WD--;
				if (ControlCmd.Length.WD==0) 
				{
					break;
				}
			}
		break;
	
		case 3:		//String descriptor
			switch(ControlCmd.Value_L) 
			{			
				case 0x00:
					for(i=0;i<4;i++) 
					{                                        
						XBYTE[0xB500]=iLANGID_DescriptorTable[i];
						ControlCmd.Length.WD--;
						if (ControlCmd.Length.WD==0)
						{  
							return;
						}		                                
					}
				break;

				case 0x10:
					for(i=0;i<26;i++) 
					{                                        
						XBYTE[0xB500]=iManufacturer_DescriptorTable[i];
						ControlCmd.Length.WD--;
						if (ControlCmd.Length.WD== 0)
						{  
							break;
						}
					}
				break;
				
				case 0x20:
					for(i=0;i<48;i++) 
					{                                        
						XBYTE[0xB500]=iProduct_DescriptorTable[i];
						ControlCmd.Length.WD--;
						if (ControlCmd.Length.WD== 0)
						{  
							return;
						}
					}
				break;
				
				case 0x30:
					gc_USB_ADDR =(gc_USB_ADDR&0x7F);
					for(i=0;i<25;i++)
					{                                        
	                    XBYTE[0xB500]=iSerialNumber_DescriptorTable[i];
						ControlCmd.Length.WD--;
						if ( ControlCmd.Length.WD == 0)
						{
							return;
		               	}
					}
					//SN »Ý¤¶©ó 0x30~0x39 / 0x41 ~ 0x49 
					XBYTE[0xB500]=0x30;
					if((gc_USB_ADDR&0x7F)<=0x09)
					{
						XBYTE[0xB500]=0x30;
					}
					else
					{                       	
						XBYTE[0xB500]=((gc_USB_ADDR%100)/10)+0x40;
					}
                   	XBYTE[0xB500]=(gc_USB_ADDR%10)+0x30;     
		 		break;
			}
		break;	

		case 6:   	// Device_Qualifier descritor
			for(i=0;i<10;i++) 
			{                        
				XBYTE[0xB500]=DeviceQualifier_DescriptorTable[i];
	        	ControlCmd.Length.WD--;
               	if ( ControlCmd.Length.WD == 0)
				{
					break;
				}
			}
		break;
		
		default:
   			gc_CX_Status_flag |= CX_STL;
		break;
	}
}


 
