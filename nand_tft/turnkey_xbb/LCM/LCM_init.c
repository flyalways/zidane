#include "SPDA2K.h"
#include "dos\fs_struct.h"
#include "Memalloc.h"
#include "..\LCM\model_define.h"
#include "..\LCM\LCM.h"
#include "..\LCM\LCM_func.h"


void TFT_init(void);

#ifdef LD5414C
#include "..\LCM\LD5414\LD5414_init.c"	
#elif defined SL9163

#include "..\LCM\SL9163\SL9163_init.c"
#elif defined S6D0134
#include "..\LCM\S6D0134\S6D0134_init.c"
#elif defined TM7735
#include "..\LCM\TM7735\TM7735_init.c"

#elif defined SL8210
#include "..\LCM\SL8210\SL8210_init.c"		
#endif

