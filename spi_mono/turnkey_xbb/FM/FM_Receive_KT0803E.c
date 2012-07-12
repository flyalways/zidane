#include "..\Header\SPDA2K.h"
#include "..\DSP\dspuser.h"
#include "..\header\variables.h"
#include "FM_receiveH.h"

#define kt0830w_address 0x6E                // kt0830–¥µÿ÷∑
#define kt0830r_address 0x6F                // kt0830∂¡µÿ÷∑


/////////////////////////////
U8 KT0830E_Read_ID(void)
{
	U8 temp_Read[2];
	
	//temp_Read = I2C_Word_Read(kt0830r_address,0x00);           //Read Manufactory ID 
//	I2C_ReadFM_AR1000(kt0830w_address,kt0830r_address,reg_add,&temp_Read);
	I2C_Read_nbyte(kt0830w_address,kt0830r_address,temp_Read,2);
dbprintf("temp_Read[0]=%bx,temp_Read[1]=%bx\n",temp_Read[0],temp_Read[1]);
	return temp_Read[1];
}

