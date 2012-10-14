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

void ui_init(void);
void ui_disp_hello(void);
void ui_test(void);
void ui_show_err(void);
void ui_show_song_pos(uint16);
void ui_show_song_num_total(uint16);
void ui_show_song_num (uint16 song_pos, uint16 num);
void ui_show_music_pause(void);
void ui_show_nofile(void);
void ui_show_song_time(uint16);
void ui_show_music_basic(void);
void ui_show_task_phase (uint8 task_phase);
uint8 ui_show_file_name
(
    uint8 *DataBuf,
    uint8 tc_ISNOrUnicode,
    uint8 nByte,
    uint8 DispOnOff
);

#endif
