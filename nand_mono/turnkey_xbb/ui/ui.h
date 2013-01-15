/******************************************************************************
* Interface provided to other components.
* 
* If you want to call some function implemented in ui module, you need to
* include this header file.
*
* Created: 2012/08/27
******************************************************************************/
#ifndef _UI_H_
#define _UI_H_

extern void ui_init(void);
extern void ui_disp_hello(void);
extern void ui_test(void);
extern void ui_show_err(void);
extern void ui_show_song_pos(uint16);
extern void ui_show_song_num_total(uint16);
extern void ui_show_song_num (uint16 song_pos, uint16 num);
extern void ui_show_music_pause(void);
extern void ui_show_nofile(void);
extern void ui_show_song_time(uint16);
extern void ui_show_music_basic(void);
extern void ui_show_task_phase (uint8 task_phase);
extern uint8 ui_show_file_name
(
    uint8 *DataBuf,
    uint8 tc_ISNOrUnicode,
    uint8 nByte,
    uint8 DispOnOff
);
extern void ui_show_vol(uint8);
void ui_show_dbg (char *str);
void ui_fm_init (void);
void ui_fm_refresh (void);
void ui_clear_screen(void);


#endif
