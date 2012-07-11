
#ifndef __RAMBASE_H__
#define __RAMBASE_H__

/*======================= Register =========================*/
#define GLOBAL_REG                  ((volatile _XBYTE *)(0xB000))	//!< Register base 0xb000
#define CPU_REG                     ((volatile _XBYTE *)(0xB100))	//!< Register base 0xb100
#define DSP_REG                     ((volatile _XBYTE *)(0xB200))	//!< Register base 0xb200
#define DMA_REG                     ((volatile _XBYTE *)(0xB300))	//!< Register base 0xb300
#define DEVICE_REG                  ((volatile _XBYTE *)(0xB400))	//!< Register base 0xb400
#define USB_REG                     ((volatile _XBYTE *)(0xB500))	//!< Register base 0xb500
#define Mini_HOST_REG               ((volatile _XBYTE *)(0xB600))	//!< Register base 0xb600
#define	FLASH_REG		            ((volatile _XBYTE *)(0xB800))	//!< Register base 0xb800
#define ECC_REG                     ((volatile _XBYTE *)(0xB8A0))  	//!< Register base 0xb8a0
#define IR_REG                     	((volatile _XBYTE *)(0xBA00))  	//!< Register base 0xba00
#define DSP3_REG                    ((volatile _XBYTE *)(0xBE00))	//!< Register base 0xbe00
/*==========================================================*/
            

// SPDA26xx flash  plan
//----only for  build mapping table use---------------//
#define 	AutoMap_BlockStatus		((volatile unsigned char xdata *)0x8000)	//default 4096 byte
#define 	AutoMap_SingleBlock 	((volatile unsigned int  xdata *)0x9000)	//default 1024 byte
#define     AutoMap_DMT 			((volatile unsigned int  xdata *)0x9400)	//default 1024 byte
#define 	AutoMap_BadBlock     	((volatile unsigned int	 xdata *)0x9800)	//default 1024 byte
#define 	AutoMap_BMT         	((volatile unsigned int  xdata *)0x9C00) 	//default 1024 byte
#define     AutoMap_Temp        	((volatile unsigned int  xdata *)0xA000) 	//default 1024 byte

//--- only for  creat mapping table  use -------------//
#define 	Source_Table			((volatile unsigned int	xdata *)0x9000)		//default 1024 byte
#define 	ExtErrBlock   			((volatile unsigned int	xdata *)0x9400) 	//default 32 byte   
#define     DmyErrBlock             ((volatile unsigned int	xdata *)0x9420)		//default 32 byte   
#define     RanErrBlock  			((volatile unsigned int	xdata *)0x9440)		//default 32 byte   
#define     FastErrBlock      		((volatile unsigned int	xdata *)0x9460)		//default 32 byte
#define 	SwapTemp_Table   		((volatile unsigned int	xdata *)0x9600) 	//default 512 byte
 
//-----Normal use -----------------------------------------------//


//-- only for Logical mapping Table use ------------- //
// MAP  table 的單位最大為1024 byte 其中包含了 maping table與 wear leveling table 相關index

#define RAM_0 					0xA000
#define MAP_0					((volatile unsigned int	xdata *)(RAM_0+0x0000))	//default Max is 241 Word	                   
#define WEARRAM_BASE_0            0xA300
#define WEARTAB_0         		((volatile unsigned char xdata *)(WEARRAM_BASE_0+0x0000))//default Max is 256 Byte   
#define RAM_1 					0xA400	
#define MAP_1					((volatile unsigned int	xdata *)(RAM_1+0x0000))	//default Max is 241 Word                   

#define WEARRAM_BASE_1            0xA700
#define WEARTAB_1         		((volatile unsigned char xdata *)(WEARRAM_BASE_1+0x0000))//default Max is 256 Byte   

//--------------- data offset addr ---------------------------//
#define MAP						(0x0000)	//default Max is 240 Word
#define INVERTINX				(0x00F0)	//default Max is 1 Word  //BY[0]:Now use E Inx  BY[1]:Now use R Inx 	 
#define EBK                  	(0x00F1)	//default Max is 2 Word	 
#define DBK						(0x00F3)	//default Max is 2 Word	
#define RBK						(0x00F5)	//default Max is 1 Word	
#define SBK						(0x00F6)	//default Max is 1 Word	
#define RPAGEINX				(0x00F7)	//default Max is 1 Word	
#define EINX					(0x00F8)	//default Max is 2 Word	
#define Q1_INX					(0x00FA)	//default Max is 2 Word	//BY[0]:Next Space inx BY[1]:Next D_SBlock  
#define Q2_INX					(0x00FC)	//default Max is 2 Word	//BY[0]:E_SBlock   BY[1] : E_SPage(No data page)
//#define ZINX                 	(0x00FE)	//default Max is 2 Word //BY[0]: HBlock power index BY[1] :Extern power inde
#define PWR_INX                	(0x00FE)	//default Max is 2 Word //BY[0]: HBlock power index BY[1] :Extern power inde 

#define R_HPAGE					(0x0100)	//default Max is 32 Word // 表示Hblock 中的HPage
#define R_PAGE					(0x0120)	//default Max is 32 Word // 相對於R block  中 py page
#define D_HPAGE					(0x0140)	//default Max is 32 Word // BY[0] : E_SBlock  BY[1]: D_SBlock 
#define D_PAGE              	(0x0160)	//default Max is 32 Word // BY[0] : Strat page BY[1]: End page(No data page)

#define HCNT					(0x0000)	//default Max is 240 Byte
#define ECNT					(0x00F1)	//default Max is 2 Byte
#define DCNT					(0x00F3)	//default Max is 2 Byte
#define RCNT					(0x00F5)	//default Max is 1 Byte	
#define SCNT					(0x00F6)	//default Max is 1 Byte
#define FCNT					(0x00F7)	//default Max is 1 Byte
#define RINX					(0x00F8)	//default Max is 4 Byte  // 表示R block 中相包含的HBlock 	
#define WINX                    (0x00FF)	//default Max is 1 Byte  

	
            




//-------------  SPARE  ----------------------//
#define	Small_Good			0x00
#define	Small_AddressH		0x01
#define	Small_AddressL		0x02
#define	Small_Last_BI		0x03

//==================    USB 相關設定 =====================//
	#define USB_BUF_Bank0_LOW	0xAA	 
	#define USB_BUF_Bank0_HIGH	0xA8		
	#define USB_BUF_Bank0		((volatile _XBYTE *)(USB_BUF_Bank0_HIGH*256))
	#define USB_BUF_Bank1_LOW	0xAE 		
	#define USB_BUF_Bank1_HIGH	0xAC	
	#define USB_BUF_Bank1		((volatile _XBYTE *)(0xAC00))			
	#define EndPoint0Table		((volatile _XBYTE *)(0xAE00))		// 242 Byte USB Data  	
//===================== BUF 相關設定 ====================//  070627 JAY 
#define	X_BYTE			((volatile unsigned char	xdata *)0x0000)
#define	X_WORD			((volatile unsigned int		xdata *)0x0000)
#define	XBYTE_8000		((volatile unsigned char	xdata *)0x8000)  //X
#define	XBYTE_8200		((volatile unsigned char	xdata *)0x8200)  //Y
#define	XBYTE_8400		((volatile unsigned char	xdata *)0x8400)  //Z1
#define	XBYTE_8600		((volatile unsigned char	xdata *)0x8600)  //Z2 
#define	XBYTE_8800		((volatile unsigned char	xdata *)0x8800)  //Z3
#define	XBYTE_8A00		((volatile unsigned char	xdata *)0x8A00)  //Z4
#define	XBYTE_8C00		((volatile unsigned char	xdata *)0x8C00)  //Z5
#define	XBYTE_8E00		((volatile unsigned char	xdata *)0x8E00)  //Z6
#define	XBYTE_9000		((volatile unsigned char	xdata *)0x9000)  //Z7
#define	XBYTE_9100		((volatile unsigned char	xdata *)0x9100)   
#define	XBYTE_9200		((volatile unsigned char	xdata *)0x9200)  //Z8
#define	XBYTE_9300		((volatile unsigned char	xdata *)0x9300)  
#define	XBYTE_9400		((volatile unsigned char	xdata *)0x9400)  //Z9
#define	XBYTE_9500		((volatile unsigned char	xdata *)0x9500)  
#define	XBYTE_9600		((volatile unsigned char	xdata *)0x9600)  //Z10
#define	XBYTE_9700		((volatile unsigned char	xdata *)0x9700)  
#define	XBYTE_9800		((volatile unsigned char	xdata *)0x9800)  //Z11
#define	XBYTE_9900		((volatile unsigned char	xdata *)0x9900) 
#define	XBYTE_9A00		((volatile unsigned char	xdata *)0x9A00)  //Z12
#define	XBYTE_9B00		((volatile unsigned char	xdata *)0x9B00)
#define	XBYTE_9C00		((volatile unsigned char	xdata *)0x9C00)  //Z14
#define	XBYTE_9D00		((volatile unsigned char	xdata *)0x9D00)
#define	XBYTE_9E00		((volatile unsigned char	xdata *)0x9E00)  //Z15
#define	XBYTE_9F00		((volatile unsigned char	xdata *)0x9F00)
                         
#define	XWORD_8000		((volatile unsigned int	xdata *)0x8000)    //X 
#define	XWORD_8200		((volatile unsigned int	xdata *)0x8200)    //Y 
#define	XWORD_8400		((volatile unsigned int	xdata *)0x8400)    //Z1
#define	XWORD_8600	    ((volatile unsigned int	xdata *)0x8600)    //Z2
#define	XWORD_8800		((volatile unsigned int	xdata *)0x8800)    //Z3
#define	XWORD_8A00		((volatile unsigned int	xdata *)0x8A00)    //Z4
#define	XWORD_8C00		((volatile unsigned int	xdata *)0x8C00)    //Z5
#define	XWORD_8E00		((volatile unsigned int	xdata *)0x8E00)    //Z6
#define	XWORD_9000		((volatile unsigned int	xdata *)0x9000)    //Z7
#define	XWORD_9100		((volatile unsigned int	xdata *)0x9100)
#define	XWORD_9200		((volatile unsigned int	xdata *)0x9200)    //Z8   
#define	XWORD_9300		((volatile unsigned int	xdata *)0x9300)         
#define	XWORD_9400		((volatile unsigned int	xdata *)0x9400)    //Z9   
#define	XWORD_9500		((volatile unsigned int	xdata *)0x9500)         
#define	XWORD_9600		((volatile unsigned int	xdata *)0x9600)    //Z10  
#define	XWORD_9700		((volatile unsigned int	xdata *)0x9700)         
#define	XWORD_9800		((volatile unsigned int	xdata *)0x9800)    //Z11  
#define	XWORD_9900		((volatile unsigned int	xdata *)0x9900)         
#define	XWORD_9A00		((volatile unsigned int	xdata *)0x9A00)    //Z12  
#define	XWORD_9B00		((volatile unsigned int	xdata *)0x9B00)         
#define	XWORD_9C00		((volatile unsigned int	xdata *)0x9C00)    //Z14  
#define	XWORD_9D00      ((volatile unsigned int	xdata *)0x9D00)         
#define	XWORD_9E00      ((volatile unsigned int	xdata *)0x9E00)    //Z15  
#define	XWORD_9F00      ((volatile unsigned int	xdata *)0x9F00)


              
#define	XLWORD_9000		((volatile unsigned long	xdata *)0x9000)
#define	XLWORD_9100		((volatile unsigned long	xdata *)0x9100)
#define	XLWORD_9200		((volatile unsigned long	xdata *)0x9200)
#define	XLWORD_9300		((volatile unsigned long	xdata *)0x9300)
#define	XLWORD_9400		((volatile unsigned long	xdata *)0x9400)
#define	XLWORD_9500		((volatile unsigned long	xdata *)0x9500)
#define	XLWORD_9600		((volatile unsigned long	xdata *)0x9600)
#define	XLWORD_9700		((volatile unsigned long	xdata *)0x9700)
#define	XLWORD_9800		((volatile unsigned long	xdata *)0x9800)
#define	XLWORD_9900		((volatile unsigned long	xdata *)0x9900)
#define	XLWORD_9A00		((volatile unsigned long	xdata *)0x9A00)
#define	XLWORD_9B00		((volatile unsigned long	xdata *)0x9B00)
#define	XLWORD_9C00		((volatile unsigned long	xdata *)0x9C00)
#define	XLWORD_9D00		((volatile unsigned long	xdata *)0x9D00)
#define	XLWORD_9E00		((volatile unsigned long	xdata *)0x9E00)
#define	XLWORD_9F00		((volatile unsigned long	xdata *)0x9F00)


//********************* File system *********************//
#define	gc_DIRtableHB				0x88
#define gc_TempFlashBufHB           0x90 
#define gc_UserDataBufHB			0xAC
#define gc_PlayRecordDataBufHB		0xAE
#define	gc_DirLCMtempBufHB			0xAA
#define gc_UserDataBuf				((volatile _XBYTE *)(gc_UserDataBufHB*256))
#define gc_PlayRecordDataBuf		((volatile _XBYTE *)(gc_PlayRecordDataBufHB*256))
#define	gc_DIRtable					((volatile _XBYTE *)(gc_DIRtableHB*256)) 
#define	gc_DirLCMtempBuf 			((volatile _XBYTE *)(gc_DirLCMtempBufHB*256))
#define gc_TempFlashBuf				((volatile _XBYTE *)(gc_TempFlashBufHB*256))
#define gc_TempFlashBuf2			((volatile _XBYTE *)(0x9200))
/* Mark@20080313 */
#define gc_MotionJpegOutBuf			((_XBYTE *)(0x8800))
#endif //!__RAMBASE_H__
