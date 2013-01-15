/******************************************************************************
* UI interface for outside modules.
*
* If you want to show something with UI, you should call some function in this
* file.
*
* Created: 2012/08/27
******************************************************************************/
#include "SPDA2K.H"
#include "ui.h"
#include "ui_impl.h"
#include "../IR/LCM_BMP.h"
#include "../IR/lcm_bmp_driver.h"

code ui_bound_t ui_bound_g = {0,0,0,0};

//-----------------------------------------------------------------------------
// ui_init
//
// Description: it's actually LCM initialization. The reason we put it here and
//              call it ui_init is, common area is really comstrained so I can't
//              put it at lcm_bmp_driver.c but ui module.
//
// Created: 2012/07/13
//-----------------------------------------------------------------------------
void ui_init (void)
{
    ui_init_impl();
}

//-----------------------------------------------------------------------------
// ui_disp_hello
//
// Description: say hello to user at the startup process.
//
// Created: 2012/08/27
//-----------------------------------------------------------------------------
void ui_disp_hello (void)
{
    ui_disp_hello_impl();
}

//-----------------------------------------------------------------------------
// ui_show_music_basic
//
// Description: show the music infrastructure menu.
// 
// Created: 2012/10/04
//-----------------------------------------------------------------------------
void ui_show_music_basic(void)
{
    ui_show_music_basic_impl();
}

//-----------------------------------------------------------------------------
// ui_show_err
//
// Created: 2012/10/04
//-----------------------------------------------------------------------------
void ui_show_err (void)
{
    ui_show_err_impl();
}

//-----------------------------------------------------------------------------
// ui_show_song_pos
//
// Description: at music menu, show which song we are playing, i.e. song's number
//              from 1.
//
// Created: 2012/10/04
//-----------------------------------------------------------------------------
void ui_show_song_pos (uint16 song_pos)
{
    ui_show_song_pos_impl(song_pos);
}

//-----------------------------------------------------------------------------
// ui_show_song_num_total
//
// Description: show how many songs are there under current directory.
//
// Created: 2012/10/04
//-----------------------------------------------------------------------------
void ui_show_song_num_total (uint16 num)
{
    ui_show_song_num_total_impl(num);
}

//-----------------------------------------------------------------------------
// ui_show_song_num
//
// Description: show song position and how many songs are there under current
//              directory.
//
// Created: 2012/10/04
//-----------------------------------------------------------------------------
void ui_show_song_num (uint16 song_pos, uint16 num)
{
    ui_show_song_num_impl (song_pos, num);
}

//-----------------------------------------------------------------------------
// ui_show_music_pause
//
// Description: show pause status at music menu.
//
// Created: 2012/10/04
//-----------------------------------------------------------------------------
void ui_show_music_pause(void)
{
    ui_show_music_pause_impl();
}

//-----------------------------------------------------------------------------
// ui_show_nofile
//
// Description: show people there is no file under current dir.
//
// Created: 2012/10/04
//-----------------------------------------------------------------------------
void ui_show_nofile(void)
{
    ui_show_nofile_impl();
}

//-----------------------------------------------------------------------------
// ui_show_time_play
//
// Description: show the latest time of current song. We don't use hour as the
//              unit.
//
// Created: 2012/10/04
//-----------------------------------------------------------------------------
void ui_show_song_time (uint16 time)
{
    ui_show_song_time_impl (time);
}

//-----------------------------------------------------------------------------
// ui_show_task_phase
//
// Description: show the current task status like play or pause, etc.
//
// Created: 2012/10/14
//-----------------------------------------------------------------------------
void ui_show_task_phase (uint8 task_phase)
{
    ui_show_task_phase_impl (task_phase);   
}

//-----------------------------------------------------------------------------
// Re-write the original function to display the file names. So I keep the original
// parameters although they looks ugly.
//
// param: uint8 *name: pinter to the unicode name data or isn name data.
//        uint8 is_isn_unicode: 1 is isn; 0 is unicode.
//        uint8 num: the number of byte of the name data. It should be even.
//        uint8 is_on_off: 1 is on; 0 is off. I doubt if we will use 0.
//
// Created: 2012/10/14
//-----------------------------------------------------------------------------
uint8 ui_show_file_name
(
    uint8 *name,
    uint8 is_isn_unicode,
    uint8 num,
    uint8 is_on_off
)
{
    return (ui_show_file_name_impl (name, is_isn_unicode, num, is_on_off));
}

//-----------------------------------------------------------------------------
// ui_show_vol
//
// Description: show system vol
//
// Created: 2012/11/27
//-----------------------------------------------------------------------------
void ui_show_vol (uint8 vol)
{
    ui_show_vol_impl (vol);
}

//-----------------------------------------------------------------------------
// ui_show_dbg
//
// Description: show debug info.
//
// Created: 2012/12/02
//-----------------------------------------------------------------------------
void ui_show_dbg (char *str)
{
    ui_disp_dbg_impl (str);
}

///----------------------------------------------------------------------------
/// FM ui job initilization.
///
/// Display the FM ui outline.
///
/// @date 2013/01/13
///----------------------------------------------------------------------------
void ui_fm_init (void)
{
    ui_fm_init_impl ();
}

///----------------------------------------------------------------------------
/// FM ui main place to refresh the screen.
///
/// @date 2013/01/14
///----------------------------------------------------------------------------
void ui_fm_refresh (void)
{
    ui_fm_refresh_impl();
}

///----------------------------------------------------------------------------
/// Clear the screen.
///
/// @date 2013/01/15
///----------------------------------------------------------------------------
void ui_clear_screen(void)
{
    ui_clear_screen_impl();
}

//-----------------------------------------------------------------------------
// ui_test
//
// Description: Wrapper function to do some test about ui. It will be an infinite
//              loop.
//
// Created: 2012/08/27
//-----------------------------------------------------------------------------
#if (UI_TEST_ONLY == FEATURE_ON)
void ui_test(void)
{
    ui_test_impl();
}
#endif