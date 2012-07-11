;********************************************************
;* three byte pointer (L)(H)(TYPE)                      *
;* type:                                                *
;*      00:     256 BYTE RAM                            *
;*      01:     64K XRAM                                *
;*      FE:     256 XRAM                                *
;*      OTHER:  64K PROG ROM                            *
;* pototype:                                            *
;*    int dbprintf(char *fmt, ...);                     *
;* R1(L), R2(H), R3(TYPE) is a pointer to fmt           *
;* and other argument is in                             *
;* ellipsis_40?041                                      *
;*                                                      *
;********************************************************
;#define DECIMAL
;NAME	DBPRINTF
;********************************************************
;* Module for printf version 1.01a                      *
;* Created by Tracker Liao, SUNPLUS                     *
;*                           Created --  2005/10/07     *
;*                           Updated --  2005/11/22     *
;* version 1.01b             Updated --  2005/11/22     *
;********************************************************
;-- 2005/11/22
;   Add %x, %X, %p, %P, %bx, %Bx, %lx, %Lx, %c, %s     
;   not support floating point/decimal/octal, and flag, width, precision yet
;-- 2005/11/23
;   re-write the program to prevent interrupt update DPTR1 problem
;

DPL1        EQU     084H
DPH1        EQU     085H
DPS         EQU     086H

?PR?_dbprintf?DBPRINT   SEGMENT CODE 
?XD?_dbprintf?DBPRINT   SEGMENT XDATA OVERLAYABLE
?C_INITSEG              SEGMENT CODE  

        PUBLIC  ?_dbprintf?BYTE,    _dbprintf
        
        RSEG    ?XD?_dbprintf?DBPRINT
?_dbprintf?BYTE:
        str?040:   DS   3               ;// save fmt pointer
        ORG 3 
ellipsis_40?041:   DS   40              ;// va_list

;---------------------------------------------------------
; inputs:
; R3,R2,R1  : fmt pointer,          R3:type, R2,R1: pointer <from input>
; output
; R7,R6     : character output counts
;
; Working registers
; R3        : fmt pointer type
; DPTR1     : fmt pointer       
; R4        : data type             {b B l L} + {d u o x X f e E g G c s p}
; R5        : va_list pointer index
; DPTR2     : temperary pointer       
; R2,R0,B   : temperart register
; R7 R6     : character counter out to console <for return value>

        RSEG    ?PR?_dbprintf?DBPRINT
_dbprintf:
        CLR     A;
        MOV     R7,     A;      ;// clear output character counter 
        MOV     R6,     A;
        MOV     R5,     #03;    ;// reset va_list index
        MOV     DPS,    A;
        MOV     DPH,    R2;
        MOV     DPL,    R1;
        MOV     A,      R3;
dbprintfloop:
        CALL    GetDataR3;
        JZ      L_Done;
        CJNE    A,#'%',outloop;
        CALL    GetDataR3;
        CJNE    A,#'%',analysisfmt;
outloop:        
        CALL    cout;
        JMP     dbprintfloop;
analysisfmt:
        MOV     R4, #02H;       ;// default is 2 bytes        
        CLR     F0;
        JMP     L_Startanalysis;        
L_GetNextCmd:      
        MOV     A,      DPL1;
        MOV     DPL,    A;
        MOV     A,      DPH1;
        MOV     DPH,    A;  
        CALL    GetDataR3;
L_Startanalysis:
        ;character in ACC
        ;format
        ;% [flags ] [ width ] [. precision ] [{b | B | l | L} ] type
        ;flags:         '+','-',' ','#'
        ;width:         decimal number, '*'
        ;.percision:    decimal number, '*'
        ;b,B,l,L:       
        ;type:          d u o x X f e E g G c s p
        ;currently, we only use the following option
        ;
        ;%lx,%x,%bx,%c,%d,%2Bx,%1x,%2bx,%ld,%bd,%s,%2u,%bu,%.bx,%3bx,%S,%p
        ; that is, type:x,X,S,s,c,C,d,D,u,U,p
        
        // check width/precision...
        CJNE    A,#'0', $+3;
        JC      L_CheckToken;
        CJNE    A,#'9'+1, $+3;
        JC      L_GetNextCmd;       ;// skip 0~9 numbers
L_CheckToken:        
        // Check flags/modifier/type
        MOV     R0,     A;          ;// temperary save current command
        MOV     A,      DPH;
        MOV     DPH1,   A;          ;// backup DPTR1 
        MOV     A,      DPL;
        MOV     DPL1,   A;  
        MOV     R2,     #0;
        MOV     DPTR,   #T_Type;
L_SearchCmdLoop:
        MOV     A,      R2;
        MOVC    A,      @A+DPTR
        JZ      L_NotCmd;
        XRL     A,      R0;
        JZ      L_Found;
        INC     R2;
        JMP     L_SearchCmdLoop;
L_Found:
        MOV     DPTR,   #T_TypeNum;
        MOV     A,      R2;
        MOVC    A,      @A+DPTR;
        JZ      L_JumpFunc;
        CJNE    A,#80H, L_Update?;
        SETB    F0;
        JMP     L_JumpFunc;
L_Update?:
        MOV     R4,     A;        
L_JumpFunc:
        MOV     DPTR,   #T_TypeFunc;
        MOV     A,      R2;
        MOVC    A,      @A+DPTR;
        MOV     B,      A;
        RL      A;
        ADD     A,      B;              ;// A=A*3;
        MOV     DPTR,   #T_JumpFunc;
        JMP     @A+DPTR;
;-----------------------------------------------------
L_Done:
;        POP     DPS;            ;// restore DPS
        RET;
;-----------------------------------------------------
L_NotCmd:
        MOV     A,      R0;
L_DispC:        
        CALL    cout;
L_EndCmd:        
        MOV     A,      DPL1;
        MOV     DPL,    A;
        MOV     A,      DPH1;
        MOV     DPH,    A;
        JMP     dbprintfloop;
;-----------------------------------------------------
L_DispCharacter:
        CALL    Get_va_list
        JMP     L_DispC;
;-----------------------------------------------------
L_DispHexdecimal:
L_phexloop:        
        CALL    Get_va_list;
        CALL    phex2;
        DJNZ    R4,     L_phexloop;
        JMP     L_EndCmd;          
;-----------------------------------------------------
L_DispString:
        CALL    Get_va_list;
        MOV     R4,     A;
        CALL    Get_va_list;
        MOV     R0,     A;
        CALL    Get_va_list;
        MOV     DPL,    A;
        MOV     R1,     A;
        MOV     DPH,    R0;
        ORL     A,      R0;
        JNZ     L_dispStringLoop;
        MOV     DPTR,   #T_Null;
L_pstr:
        MOV     R4,     #0FFH;  
L_DispStringLoop:
        MOV     A,      R4;
        CALL    GetData;        
        JZ      L_EndCmd;
        CALL    cout;
        JMP     L_DispStringLoop;
;-----------------------------------------------------
L_DispPointer:
        CALL    Get_va_list;
        ADD     A,      #2;
        ANL     A,      #03H;
        MOV     DPTR,   #T_PointerType;
        JNB     F0,     L_notUpper;
        MOV     DPTR,   #T_PointerTypeUpper;
L_notUpper:
        MOVC    A,      @A+DPTR;
        CALL    cout;
        MOV     A,      #':';
        CALL    cout;
        CALL    Get_va_list;
        CALL    phex2;
        CALL    Get_va_list;
        CALL    phex2;
        JMP     L_EndCmd;
;-----------------------------------------------------
#ifdef DECIMAL
L_DispFloating:
        MOV     R4,     #4;
        JMP     L_DispOctal     
;-----------------------------------------------------
L_DispDecimal:
        JNB     F0, L_Positive;
        CALL    Get_va_list;
        ANL     A,      #80H;
        JZ      L_Positive;
        CALL    Negative_it;
        MOV     A,      #'-';
        CALL    cout;
L_Positive:
        CALL    ConvertToDecimal;
        CLR     F0;
L_DispDecimalLoop:
        CALL    Get_va_list
        JNZ     L_Dispout;
        JNB     F0, L_Next;
L_Dispout:
        SETB    F0;
        CALL    phex2;
L_Next:
        DJNZ    R4, L_DispDecimalLoop;
        JMP     L_EndCmd;
;-----------------------------------------------------
#else
L_DispFloating:
        MOV     R4,     #4;
L_DispDecimal:
#endif
L_DispOctal:
        CALL    Get_va_list;
        DJNZ    R4, L_DispOctal;
        MOV     DPTR,   #T_NoSupport;
        JMP     L_pstr;
;-----------------------------------------------------
Get_va_list:
        MOV     DPTR,   #str?040;
        MOV     A,      R5;
        ADD     A,      DPL;
        MOV     DPL,    A;
        JNC     L_NotOv?;
        INC     DPH;
L_NotOv?:        
        INC     R5;
        MOVX    A,      @DPTR;
        RET;
;-----------------------------------------------------
; initial pointer=buf+R5+1, R4 bytes
; result, pointer=buf+R5+1, neg(buf+R5[0...R4])  
#ifdef DECIMAL
Negative_it:
        MOV     A,     R4;
        MOV     R0,     A;      ;// 
        DEC     A;
        ADD     A,      DPL;
        MOV     DPL,   A;
        JNC     L_Negative;
        INC     DPH;
L_Negative:
        SETB    C;
L_Negativeloop:
        MOVX    A,      @DPTR;
        XRL     A,      #0FFH;
        ADDC    A,      #0;
        MOVX    @DPTR,  A;
        DJNZ    R0,     L_Dec?;
        RET;
L_Dec?:
        MOV     A,      DPL;       ;// DEC DPTR1
        JNZ     $+4
        DEC     DPH;
        DEC     DPL;
        JMP     L_Negativeloop;
;-----------------------------------------------------
ConvertToDecimal:
        
        MOV     A,      DPL;
        ADD     A,      #-2;
        MOV     DPL,    A;
        JC      $+4;
        DEC     DPH;
        CLR     A;
        MOVX    @DPTR,      A;
        INC     DPTR;
        MOVX    @DPTR,      A;
        INC     DPTR;               ;// point to MSB of data
                


        
        DEC     R5;
        DEC     R5;
        DEC     R5;
        INC     R4;
        INC     R4;
        RET
#endif
;-----------------------------------------------------
phex2:
        PUSH    ACC;
        SWAP    A;
        CALL    phex1;
        POP     ACC;
phex1:
        ANL     A,      #0FH;
        JB      F0,     L_Upper;        
        MOV     DPTR,   #T_Hex2Char;
        MOVC    A,      @A+DPTR;
        JMP     cout;
L_Upper:        
        MOV     DPTR,   #T_Hex2CHR;
        MOVC    A,      @A+DPTR;
        JMP     cout;
;-----------------------------------------------------
T_JumpFunc:
        LJMP    L_GetNextCmd;
        LJMP    L_DispDecimal;
        LJMP    L_DispHexdecimal;
        LJMP    L_DispCharacter;
        LJMP    L_DispString;
        LJMP    L_DispOctal;
        LJMP    L_DispPointer;
        LJMP    L_DispFloating;
        LJMP    L_NotCmd;
T_Hex2Char:
        DB      '0123456789abcdef';
T_Hex2CHR:
        DB      '0123456789ABCDEF';                
T_Null:
        DB      '(null)',0;
T_NoSupport:
        DB      '(no func)',0
T_PointerType:
        DB      'xcix';
T_PointerTypeUpper:
        DB      'XCIX';
T_Type:
        DB      '.', '+', '-', ' ', '#', '*';
        DB      'b', 'B', 'l', 'L';
        DB      'u', 'U', 'd', 'D', 'x', 'X', 'c', 'C', 's', 'S', 'o', 'O', 'p', 'P';
        DB      'f', 'F', 'e', 'E', 'g', 'G', 0; 
T_TypeNum:
        DB      00H, 00H, 00H, 00H, 00H, 00H;
        DB      01H, 01H, 04H, 04H;     ;// bit 2..0 bytes of target data, bit3 upper case
        DB      00H, 00H, 80H, 80H, 00H, 80H, 00H, 00H, 00H, 00H, 00H, 00H, 00H, 80H;
        DB      00H, 00H, 00H, 80H, 00H, 80H 
T_TypeFunc:
        DB      00H, 00H, 00H, 00H, 00H, 00H;
        DB      00H, 00H, 00H, 00H;
        DB      01H, 01H, 01H, 01H, 02H, 02H, 03H, 03H, 04H, 04H, 05H, 05H, 06H, 06H;
        DB      07H, 07H, 07H, 07H, 07H, 07H;
;----------------------------------------
;input
;   pointer:    DPTR
;   type:       R3
;output:
;   output:     A
;               DPTR++
;----------------------------------------
GetDataR3:
        MOV     A,      R3;
GetData:
        MOV     R1,     DPL;
L_EndDPTR:
        CJNE    A,#01, L_Not64KXDATA;
        MOVX    A,      @DPTR;
        JMP     L_IncDPTR;
L_Not64KXDATA:
        JNC     L_Not256RAM;
        MOV     A,      @R1;
        JMP     L_IncDPTR;
L_Not256RAM:
        CJNE    A,#0FEH,   L_Not256XDATA;
        MOVX    A,      @R1;
        JMP     L_IncDPTR;
L_Not256XDATA:
        CLR     A;
        MOVC    A,      @A+DPTR;
L_IncDPTR:
        INC     DPTR;
        RET      
;--------------------------------------------------------------------------
;input
;   ACC:        character tobe print, if 13/10 will print LF+CR
;output:
;   NONE;
;stack usage:
;   0
;register:
;   auto increase R7,R6
;--------------------------------------------------------------------------
cout:	
        ;// cout;----------------------------------------------------------
        CJNE    A,#10,  ?L_Norm;
        JNB     TI,     $;      ;// wait until last data is transmitted
        CLR     TI;             ;// clr ti before the mov to sbuf!
        MOV     SBUF,   #13;
?L_Norm:
        JNB     TI,     $;
        CLR     TI    
        MOV     SBUF,   A;      ;// put data to transmit
                                ;// Check if LF then auto add CR
        CJNE    A,#13,  L_Pass?;
        JNB     TI,     $;
        CLR     TI;
        MOV     SBUF,   #10;
L_Pass?:
        ;// end cout -------------------------------------------------------
        
        INC     R7;
        CJNE    R7,#0,  EndCout;
        INC     R6;
EndCout:        
        RET;
	
	END
