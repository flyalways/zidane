void DSP_MIPSSelect(U8 DSP_MIPS);
U8 DSP_MIPSRead(void);
void DSP_ClearReady(void);
U8 DSP_SearchSunplusKey(void);
U8 DSP_Download(U8 tc_CurrentIMID, U8 tc_CurrentPMID);
U8 DSP_PlayInit(void);
void DSP_SurroundDisable(void);
U8  DSP_PauseCmd(void);
U8  DSP_StopCmd(void);
void DSP_EQ_Cmd(U8 tc_EQType);
void DSP_SpectrumOn(void);
U8 DSP_ResetCmd(void);

