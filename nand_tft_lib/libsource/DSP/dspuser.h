//===========================================================
//   Project Name:	SPDA23xx
//   File Name:	DSPUser.h
//   Built Date:	2008-04-23
//   Revision History:
//   Rev      Date           Author       Comment
//   1.0      2008-04-23  joseph.hsieh    created
//============================================================


#ifndef	_DSPUSER_H_
#define _DSPUSER_H_

enum {
	DSP_CLK_CTRL = 0x01,
	SRAM2T_CTRL = 0x02
};

//==============================================================
// Function Prototype
//==============================================================

void DSP_MIPSSelect(U8 DSP_MIPS);
U8 DSP_MIPSRead(void);
//void DSP_Sleep(U8 tc_Flag); //20081003 mark
void DSP_WakeUp(U8 tc_Flag);
void DSP_ClearReady(void);
U8 DSP_SearchSunplusKey(void);
U8 DSP_Download(U8 tc_CurrentIMID, U8 tc_CurrentPMID);
U8 DSP_PlayInit(void);
void DSP_SurroundDisable(void);
void DSP_SurroundEnable(void);
U8  DSP_PauseCmd(void);
U8  DSP_StopCmd(void);
void DSP_EQ_Cmd(U8 tc_EQType);
void DSP_SpectrumOn(void);
U8 DSP_ResetCmd(void); //Jimi 080522

#endif	// _DSPUSER_H_
