
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


/*  P3  */
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
sbit TF2   = 0xCF;
sbit EXF2  = 0xCE;
sbit RCLK  = 0xCD;
sbit TCLK  = 0xCC;
sbit EXEN2 = 0xCB;
sbit TR2   = 0xCA;
sbit C_T2  = 0xC9;
sbit CP_RL2= 0xC8;

        sbit    ACC0    = ACC^0 ;
        sbit    ACC1    = ACC^1 ;
        sbit    ACC2    = ACC^2 ;
        sbit    ACC3    = ACC^3 ;
        sbit    ACC4    = ACC^4 ;
        sbit    ACC5    = ACC^5 ;
        sbit    ACC6    = ACC^6 ;
        sbit    ACC7    = ACC^7 ;

#ifdef CAR_64
sbit SD_Detect=P1^1;
#endif

#ifdef EVB_128
sbit SD_Detect=P1^7;
#endif

