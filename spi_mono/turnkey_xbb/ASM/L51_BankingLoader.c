#include <stdio.h>
#include "..\header\SPDA2K.h"
#include "..\header\variables.h"


//! Entry of the banking loader.
void perform_nand_bank_call(void)
{
#pragma asm
	cjne A, gc_bank_number, ?_need_to_load_nand_bank
	push DPL
	push DPH
	ret

?_need_to_load_nand_bank:
	//Target Bank Number is in register A
	push gc_bank_number
	mov  gc_bank_number, A

	//push the bank recover function for returning use
	mov  A, #LOW return_from_nand_bank
	push ACC
	mov  A, #HIGH return_from_nand_bank
	push ACC
	//push target address
	push DPL
	push DPH
 	//push PSW and IE
	push PSW
	mov  PSW, #18	//use register bank3
	push IE
	clr  EA			//disable IRQ

	call load_nand_bank

	//recover IE and PSW
	pop  IE
	pop  PSW
	ret
#pragma endasm
}

//! Exit of the banking loader.
void return_from_nand_bank(void)
{
#pragma asm
	mov  A, gc_bank_number	//A is current bank
	pop  gc_bank_number		//gc_bank_number is return bank
	push PSW
	cjne A, gc_bank_number, ?_recover_need_load_nand
	pop PSW
	ret
?_recover_need_load_nand:
	mov  PSW, #18 			//use register bank3
	push IE
	clr  EA					//disable IRQ

	call load_nand_bank

	//pop PSW and IE
	pop  IE
	pop  PSW
	ret
#pragma endasm
}

//! NAND banking loader.
#define SECTORS_PER_BANK  	24		// 12K
#define	BANK0_START_ADDR	0x60	// 24K
#define	BANK1_START_ADDR	0xC0	// 48K
#define	BANK_SIZE			0x30	// 12K


void load_nand_bank(void) using 3
{
	xdata	U8	tc_clock;
	xdata	U8	tc_B400;
	xdata	U16	tw_addr;
 
	tc_clock=gc_clock_mode;
	set_clock_mode(CLOCK_MODE_SPI);
	tc_B400=XBYTE[0xB400]; 
         
	if (gc_bank_number == 0)
	{
		tw_addr=BANK0_START_ADDR;
	}
	else
	{
		tw_addr=BANK1_START_ADDR+(gc_bank_number-1)*BANK_SIZE;
	}

	//dbprintf("%bx ",gc_bank_number); 

	//reset spi interface 
	XBYTE[0xB400]|=0x20;
	XBYTE[0xB400]=0x04; 
	while(1)
	{
		gc_SPI_Addr1=(U8)(tw_addr);
		gc_SPI_Addr2=(U8)(tw_addr>>8);
		SPI_Load_Code_To_DRAM(SECTORS_PER_BANK);

		if(gb_ReloadProgram==0)
		{
			break;
		}
		else
		{
			//reset spi interface 
			XBYTE[0xB400]|=0x20;
			XBYTE[0xB400]=0x04; 
			gb_ReloadProgram=0;
		}
	}
	set_clock_mode(tc_clock); 
	XBYTE[0xB400]=tc_B400;
}


void SPI_Load_Code_To_DRAM(U8 u_Seccnt)
{
	data	U8	i;
	data	bit	tc_Retry=0;
  
	gc_ProgRAM_Index=BANK0_START_ADDR;

    while(u_Seccnt)
	{
        SPI_ENABLE(); 
        SPI_Send_Byte(0x03);  //CMD
        SPI_Send_Byte(gc_SPI_Addr2);  
        SPI_Send_Byte(gc_SPI_Addr1);  
        SPI_Send_Byte(0x00);  

		XBYTE[0xB304]=0x09;		//Reset DMA
		XBYTE[0xB304]=0x08;		//Normal DMA
	    SET_DEVICE_READ_TO_DRAM();

	    SPI_REG[0x10] = 0x0F;						// after DMA, SPI FIFO has to be cleared
		Trigger_Data_Transfer_Done();

		if(tc_Retry==0)
		{
			tc_Retry=1;
		}
		else
		{
		    gc_SPI_Addr1+=0x02;
			if(gc_SPI_Addr1==0x00)
			{
				gc_SPI_Addr2++;
			}
		    gc_ProgRAM_Index+=0x02;	// ProgRAM Index
			u_Seccnt--;  
		}
	    SPI_DISABLE();
		for(i=0;i<20;i++);
    }
} 



