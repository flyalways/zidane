#include "..\Header\SPDA2K.h"
#include "..\header\PROTOTYP.h"
#include "..\IR\LCM_BMP.h"

//============================================================================
void LCM_set_address(U8 page, U8 col)
{
	LCM_write_command(page|0xb0);
	LCM_write_command((col>>4)|0x10);
	LCM_write_command(col&0x0f);
}
//============================================================================