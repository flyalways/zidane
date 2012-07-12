#include "..\Header\SPDA2K.h"
#include "..\DSP\dspuser.h"
#include "..\header\variables.h"
#include "FM_receiveH.h"
#ifdef Mode_FM_CL6017
	#include "..\fm\FM_Receive_CL6017.c"
#elif defined Mode_FM_RDA5807P
	#include "..\fm\FM_Receive_RDA5807P.c"
#elif defined Mode_FM_QN8035
	#include "..\fm\FM_Receive_QN8035.c"
#endif

