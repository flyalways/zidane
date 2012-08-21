#include "SPDA2K.h"
#include "dos\fs_struct.h"
#include "Memalloc.h"
#include "..\LCM\model_define.h"
#include "..\LCM\LCM.h"
#include "..\LCM\LCM_func.h"


void TFT_init(void);

#ifdef LD5414C
#include "..\LCM\LD5414\LD5414_init.c"
#elif defined S6D0134
#include "..\LCM\S6D0134\S6D0134_init.c"
#elif defined S6D0133
#include "..\LCM\S6D0133\S6D0133_init.c"
#elif defined CS1783
#include "..\LCM\CS1783_18H\CS1783_init.c"	
#elif defined SL9163
#include "..\LCM\SL9163\SL9163_init.c"
#elif defined SL8210
#include "..\LCM\SL8210\SL8210_init.c"	
#elif defined LZ7669
#include "..\LCM\LZ7669\LZ7669_init.c"
#elif defined S6B33B9
#include "..\LCM\S6B33B9\S6B33B9_init.c"
#elif defined ST7735R
#include "..\LCM\ST7735R\ST7735R_init.c"
#elif defined HD66766
#include "..\LCM\HD66766\HD66766_init.c"
#elif defined HD66773R
#include "..\LCM\HD66773R\HD66773R_init.c"
#elif defined S6B33B9_New
#include "..\LCM\S6B33B9_New\S6B33B9_New_init.c"
#elif defined S6B33BC
#include "..\LCM\S6B33BC\S6B33BC_init.c"
#elif defined S6B33BF
#include "..\LCM\S6B33BF\S6B33BF_init.c"
#endif

