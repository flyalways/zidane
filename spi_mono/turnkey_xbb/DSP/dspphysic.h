#ifndef	_DSPPHYSIC_H
#define _DSPPHYSIC_H

#define HIBYTE(var)	(*(unsigned char*)&var)
#define LOBYTE(var)	(*((unsigned char*)&var+1))

//============================================================
//Constant Define
//============================================================
enum {
	DMA_24BIT_MODE = 0,
	DMA_16BIT_MODE = 1
};


#endif	// _DSPPHYSIC_H
