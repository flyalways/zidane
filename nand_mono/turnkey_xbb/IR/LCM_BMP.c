#include "..\Header\SPDA2K.h"
#include "..\header\PROTOTYP.h"
#include "..\IR\LCM_BMP.h"
#include "..\..\libsource\header\UI_config.h"

#if (LCM_SIZE==0)
	#include "..\IR\LCM_BMP_128x64.c"
#endif

#if (LCM_SIZE==1)
	#include "..\IR\LCM_BMP_96x32.c"
#endif

#if (LCM_SIZE == 2)
	#include "..\IR\LCM_BMP_160x120.c"
#endif

