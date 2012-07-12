#include <string.h>
#include <intrins.h>
#include "SPDA2K.h"

void  USB_I2C(void)
{
	XBYTE[0xB00E] = 0x00 ; 						// disable I2C interrupt
	XBYTE[0xB00A] = 0x15 ; 						
	XBYTE[0xB00D] = 0x0F ; 						
	XBYTE[0xB00B] = 0x01 ; 						
	 
	XBYTE[0xB00A] = 0x10 ; 						
	XBYTE[0xB00D] = 0x20 ; 						
	XBYTE[0xB00B] = 0x01 ; 
	USB_I2C_ID = 0x02;
	USB_I2C_Addr = 0x0D;	 						
    USB_I2C_Wdata = 0xA0; 
    I2C_M_Tx(USB_I2C_ID,USB_I2C_Addr,USB_I2C_Wdata);
    I2C_M_Rx(USB_I2C_ID,USB_I2C_Addr);  
}

void I2C_M_Tx(U8 ID,U8 Addr,U8 Wdata ) 
{
	XBYTE[0xB00A] = 0x11 ; 						
	XBYTE[0xB00D] = 0xD0 ; 						
	XBYTE[0xB00B] = 0x01 ;
	
	XBYTE[0xB00A] = 0x13 ; 						
	XBYTE[0xB00D] = ID ; 						
	XBYTE[0xB00B] = 0x01 ;
	
	XBYTE[0xB00A] = 0x11 ; 						
	XBYTE[0xB00D] = 0xF0 ; 						
	XBYTE[0xB00B] = 0x01 ; 
	I2C_Wait_Intpend();
	
	XBYTE[0xB00A] = 0x13 ; 						
	XBYTE[0xB00D] = Addr ; 						
	XBYTE[0xB00B] = 0x01 ;
	I2C_Clr_Int();
	I2C_Wait_Intpend();
	
	XBYTE[0xB00A] = 0x13 ; 						
	XBYTE[0xB00D] = Wdata ; 						
	XBYTE[0xB00B] = 0x01 ;
	I2C_Clr_Int();
	I2C_Wait_Intpend(); 
	
	XBYTE[0xB00A] = 0x11 ; 						
	XBYTE[0xB00D] = 0xD0 ; 						
	XBYTE[0xB00B] = 0x01 ;
}


void I2C_Clr_Int(void )
{
	XBYTE[0xB00A] = 0x10 ; 						
	XBYTE[0xB00D] = 0x20 ; 						
	XBYTE[0xB00B] = 0x01 ;
}


void I2C_Wait_Intpend(void )
{
	while(!(XBYTE[0xB00F]&0x01));
}


void I2C_M_Rx(U8 ID,U8 Addr) 
{
	XBYTE[0xB00A] = 0x11 ; 						
	XBYTE[0xB00D] = 0xD0 ; 						
	XBYTE[0xB00B] = 0x01 ;
	
	XBYTE[0xB00A] = 0x13 ; 						
	XBYTE[0xB00D] = ID ; 						
	XBYTE[0xB00B] = 0x01 ;
	
	XBYTE[0xB00A] = 0x11 ; 						
	XBYTE[0xB00D] = 0xF0 ; 						
	XBYTE[0xB00B] = 0x01 ; 
	I2C_Wait_Intpend();
	
	XBYTE[0xB00A] = 0x13 ; 						
	XBYTE[0xB00D] = Addr ; 						
	XBYTE[0xB00B] = 0x01 ;
	I2C_Clr_Int();
	I2C_Wait_Intpend();
	
	XBYTE[0xB00A] = 0x11 ; 						
	XBYTE[0xB00D] = 0xD0 ; 						
	XBYTE[0xB00B] = 0x01 ;
	
	XBYTE[0xB00A] = 0x11 ; 						
	XBYTE[0xB00D] = 0x90 ; 						
	XBYTE[0xB00B] = 0x01 ;
	
	XBYTE[0xB00A] = 0x13 ; 						
	XBYTE[0xB00D] = ID ; 						
	XBYTE[0xB00B] = 0x01 ;
	
	XBYTE[0xB00A] = 0x11 ; 						
	XBYTE[0xB00D] = 0xB0 ; 						
	XBYTE[0xB00B] = 0x01 ;
	I2C_Wait_Intpend();
	I2C_Clr_Int();
	I2C_Wait_Intpend();
	
	XBYTE[0xB00A] = 0x11 ; 						
	XBYTE[0xB00D] = 0xD0 ; 						
	XBYTE[0xB00B] = 0x01 ;
	
	XBYTE[0xB00A] = 0x13 ;
	XBYTE[0xB00B] = 0x10 ;
	USB_I2C_Rdata  =XBYTE[0xB00C];	
}


void USB_On(void)
{
	/*------------ Speed Up USB CLK -----------*/  
	set_clock_mode(CLOCK_MODE_USB);                      
	USB_Mode();
}