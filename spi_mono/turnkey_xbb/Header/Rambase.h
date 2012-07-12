
#ifndef __RAMBASE_H__
#define __RAMBASE_H__

/*======================= Register =========================*/
#define GLOBAL_REG		((volatile _XBYTE *)(0xB000))	//!< Register base 0xb000
#define CPU_REG			((volatile _XBYTE *)(0xB100))	//!< Register base 0xb100
#define DSP_REG			((volatile _XBYTE *)(0xB200))	//!< Register base 0xb200
#define DMA_REG			((volatile _XBYTE *)(0xB300))	//!< Register base 0xb300
#define DEVICE_REG		((volatile _XBYTE *)(0xB400))	//!< Register base 0xb400
#define USB_REG			((volatile _XBYTE *)(0xB500))	//!< Register base 0xb500
#define Mini_HOST_REG	((volatile _XBYTE *)(0xB600))	//!< Register base 0xb600
#define	FLASH_REG		((volatile _XBYTE *)(0xB800))	//!< Register base 0xb800
#define ECC_REG         ((volatile _XBYTE *)(0xB8A0))  	//!< Register base 0xb8a0
#define IR_REG          ((volatile _XBYTE *)(0xBA00))  	//!< Register base 0xba00
#define SPI_REG         ((volatile _XBYTE *)(0xBB00))   //!< Register base 0xbb00 
#define DSP3_REG        ((volatile _XBYTE *)(0xBE00))	//!< Register base 0xbe00
//==================    USB 相關設定 =====================//
	#define USB_BUF_Bank0		((volatile _XBYTE *)(0xA800))	 
	#define USB_BUF_Bank0_HIGH	0xA8		
	#define USB_BUF_Bank1		((volatile _XBYTE *)(0xAC00))		
	#define USB_BUF_Bank1_HIGH	0xAC					
	#define EndPoint0Table		((volatile _XBYTE *)(0x8A00))		// 242 Byte USB Data  	
          
#define gc_UserDataBuf				((volatile _XBYTE *)(0xAC00))
#define gc_PlayRecordDataBuf		((volatile _XBYTE *)(0xAE00))
#define gc_RecordDataBuf			((volatile _XBYTE *)(0x8E00))
#define	gc_FATTempBuffer			((volatile _XBYTE *)(0x8C00))
#define gc_UserDataBufHB			0xAC
#define gc_PlayRecordDataBufHB		0xAE
#define gc_RecordDataBufHB			0x8E
#define gc_FATTempBufferHB			0x8C
#define	gc_DirLCMtempBuf 			((volatile _XBYTE *)(0x8000))

#endif //!__RAMBASE_H__
