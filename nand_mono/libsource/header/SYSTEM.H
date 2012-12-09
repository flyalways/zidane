#ifndef __SYSTEM_H__
#define __SYSTEM_H__

        sfr P0    = 0x80;          //!< I/O Port 0
        sfr P1    = 0x90;          //!< I/O Port 1
        sfr P2    = 0xA0;          //!< I/O Port 2
        sfr P3    = 0xB0;          //!< I/O Port 3
        sfr PSW   = 0xD0;          //!< Program Status Word
        sfr ACC   = 0xE0;          //!< Accumulator
        sfr B     = 0xF0;
        sfr SP    = 0x81;          //!< Stack Pointer
        sfr DPL   = 0x82;          //!< Data Pointer Low
        sfr DPH   = 0x83;          //!< Data Pointer High
        sfr PCON  = 0x87;          //!< Power Control Register
        sfr TCON  = 0x88;          //!< Timer/Counter Control Register
        sfr TMOD  = 0x89;          //!< Timer/Counter Mode Control Register
        sfr TL0   = 0x8A;          //!< Timer0 Low Byte
        sfr TL1   = 0x8B;          //!< Timer1 Low Byte
        sfr TH0   = 0x8C;          //!< Timer0 High Byte
        sfr TH1   = 0x8D;          //!< Timer1 High Byte
        sfr CKCON = 0x8E;		   //!< clock control
		//sfr MemStretch = 0x8E;
        sfr IE    = 0xA8;          //!< Interrupt Enable Register
        sfr IP    = 0xB8;          //!< Interrupt Priority Register
        sfr SCON  = 0x98;          //!< Serial Port Control Register
        sfr SBUF  = 0x99;          //!< Serial Port Buffer Register

        sfr	RamPage	= 0x9B;

        sfr T2CON = 0xC8;          //!< Timer2 control
        sfr RCAP2L = 0xCA;         //!< Timer2 captured low byte
        sfr RCAP2H = 0xCB;         //!< Timer2 captured high byte
        sfr TL2   = 0xCC;          //!< Timer2 low byte
        sfr TH2   = 0xCD;          //!< Timer2 high byte


/*----------------------------------------------------------------------*/
/*  BIT Registers                                                       */
/*----------------------------------------------------------------------*/
/*  PSW  */
        sbit CY    = 0xD7;         //!< Carry
        sbit AC    = 0xD6;         //!< Auxiliary Carry
        sbit F0    = 0xD5;
        sbit RS1   = 0xD4;         //!< Register Bank Select 1.
        sbit RS0   = 0xD3;         //!< Register Bank Select 0.
        sbit OV    = 0xD2;         //!< Overflow
        sbit P     = 0xD0;         //!< Odd Parity in ACC

/*  TCON  */
        sbit TF1   = 0x8F;         //!< Timer1 Overflow.
        sbit TR1   = 0x8E;         //!< Timer1 Start.
        sbit TF0   = 0x8D;         //!< Timer0 Overflow.
        sbit TR0   = 0x8C;         //!< Timer0 Start.
        sbit IE1   = 0x8B;         //!< INT1 Detected.
        sbit IT1   = 0x8A;         //!< INT1 Trigger Mode, '0':Level Mode.
        sbit IE0   = 0x89;         //!< INT0 Detected.
        sbit IT0   = 0x88;         //!< INT0 Trigger Mode, '0':Level Mode.

/*  IE  */
        sbit EA    = 0xAF;         //!< Enable(1)/Disable(0) All INT
        sbit ET2   = 0xAD;         //!< Enable Timer2 OV/CAP INT
        sbit ES    = 0xAC;         //!< Enable Serial Port INT
        sbit ET1   = 0xAB;         //!< Enable Timer1 INT
        sbit EX1   = 0xAA;         //!< Enable INT1 INT
        sbit ET0   = 0xA9;         //!< Enable Timer0 INT
        sbit EX0   = 0xA8;         //!< Enable INT0 INT

/*  IP  */
        sbit PT2   = 0xBD;         //!< Timer2 INT Priority
        sbit PS    = 0xBC;         //!< Serial Port INT Priority
        sbit PT1   = 0xBB;         //!< Timer1 INT Priority
        sbit PX1   = 0xBA;         //!< INT1 Priority
        sbit PT0   = 0xB9;         //!< Timer0 INT Priority
        sbit PX0   = 0xB8;         //!< INT0 Priority

/*  P1  */
//      sbit COLOR_G            = P1^0;     // COLOR_G
        sbit LOCK_SWITCH        = P1^1;     // LOCK switch
        sbit LCD_RS             = P1^2;     // LCD RS(A0)
        sbit LCD_CLK	        = P1^3;     // LCD CLK
        sbit LCD_SI             = P1^4;     // LCD Data
        sbit P1_6               = P1^6;     // LED
        sbit P1_7               = P1^7;     // LED

		#ifdef CAR_64
        // For our hw design with SPDA2635A, we use micro SD card, where
        // the /CD signal and DAT3 share the same pin, XFMGPIO_16.
        // I don't know how to define a pin for SD_Detect, but just keep
        // it the same with P1^7 in EVB_128. Because P1^7 is not pulled out
        // in 64 pin SPDA2635A.
		sbit SD_Detect          = P1^7;     //
		#elif defined(EVB_128)  
        sbit SD_Detect          = P1^7;     //
		#endif

        sbit P1_3               = P1^3;     //(JC)H0612 for alternative play key
     
/*  P3  */
        sbit COLOR_R = P3^0;      // COLOR_R
        sbit P3_0   = P3^0;			//!< RS232 TX
        sbit P3_1   = P3^1;
        
        sbit RD     = 0xB7;
        sbit WR     = 0xB6;
        sbit T1     = 0xB5;
        sbit T0     = 0xB4;
        sbit INT1   = 0xB3;
        sbit INT0   = 0xB2;
        sbit TXD    = 0xB1;
        sbit RXD    = 0xB0;

/*  SCON  */
        sbit SM0   = 0x9F;
        sbit SM1   = 0x9E;
        sbit SM2   = 0x9D;
        sbit REN   = 0x9C;
        sbit TB8   = 0x9B;
        sbit RB8   = 0x9A;
        sbit TI    = 0x99;
        sbit RI    = 0x98;

/*  T2CON  */
        sbit TF2   = 0xCF;              //!< Timer2 overflow flag
        sbit EXF2  = 0xCE;              //!< Timer2 external flag
        sbit RCLK  = 0xCD;              //!< Timer2 receiver flag
        sbit TCLK  = 0xCC;              //!< Timer2 transmit flag
        sbit EXEN2 = 0xCB;              //!< Timer2 external enable
        sbit TR2   = 0xCA;              //!< Timer2 run control
        sbit C_T2  = 0xC9;              //!< Timer2 counter/interval timer select bit
        sbit CP_RL2= 0xC8;              //!< Timer2 capture/reload flag

/*======================================================================*/
/* bit definition of accumulator                                        */
/*======================================================================*/
        sbit    ACC0    = ACC^0 ;
        sbit    ACC1    = ACC^1 ;
        sbit    ACC2    = ACC^2 ;
        sbit    ACC3    = ACC^3 ;
        sbit    ACC4    = ACC^4 ;
        sbit    ACC5    = ACC^5 ;
        sbit    ACC6    = ACC^6 ;
        sbit    ACC7    = ACC^7 ;


/*
;##########################################################################
;  Informations about 8051 timer and counter
;##########################################################################
;TMOD bit |  7    6    5    4  |  3    2    1    0  | Mode 0: 8 bits C/T
;         |        CT1         |         CT0        | Mode 1: 16 bits C/T
;         | GATE C/T  M1   M0  | GATE C/T  M1   M0  | Mode 2: 8 bits
;        *|  0    1    0    1  |  0    0    0    1  |         Auto Reload
;         | counter 1 mode 1   | timer 0   mode 1   |
;
;TCON bit |  7    6    5    4  |  3    2    1    0  | IE0.1:Set by Hardware
;         | TF1  TR1  TF0  TR0 | IE1  IT1  IE0  IT0 | IT0.1:Edge Trigger
;        *|  0    0    0    1  |  0    1    0    1  | TF0.1:Int.Req.Flag
;         |   T/C running      | falling edge trig. | TR0.1:Run Control
;
;IE   bit |  7    6    5    4  |  3    2    1    0  | ET012: Internal C/T
;         | EA   NU   ET2  ES  | ET1  EX1  ET0  EX0 | EX01: Exernal Interrupt
;        *|  1    X    0    0  |  0    0    1    1  | ES: Serial Port
;  *(initial value)
;-------------------------------------------------------------------------
; In 4XS CD-ROM System
;       Timer 0 : always be watch dog timer ( 16 bit, mode 1 )
;       Timer 1 : 1. calibration => timer
;                 2. seeking     => counter
;##########################################################################
*/

#endif //!__SYSTEM_H__
