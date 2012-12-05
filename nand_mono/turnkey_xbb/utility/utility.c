///----------------------------------------------------------------------------
/// @file
/// @details All kinds of utilities across the project including debug, etc.
/// @date 2012/12/03
///----------------------------------------------------------------------------
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include "utility.h"
#include "SPDA2K.H"
#include "../ui/ui.h"  // Get the ui handle to display the string.

// Define how we output the string:
//  1. To the terminal, you can use printf;
//  2. To the lcd, you can use your handle...
//#define DBG_OUT(string) printf (string)
#define DBG_OUT(string) ui_show_dbg(string)

char xdata dbg_buf[DBG_MAX_LENGTH];

///----------------------------------------------------------------------------
/// The idea here is:
///  1. To build a whole string people will see.
///  2. Output the string with a handle you want including the terminal, lcd...
///
/// Because Keil C51 doesn't support the io functions with size info. So there
/// could be overflow problem. Just be careful about the size.
///
/// @date 2012/12/02
///----------------------------------------------------------------------------
void dbg_msg (const char *fmt, ...)
{
    va_list ap;

    // REVISIT!!!
    // It's so strange. memset() must be put after <va_list ap;>. If not, if it
    // is put at the begining of this function, compile will fail. It's wierd.
    // Clear the buffer each time before using it.
    memset(dbg_buf, 0, DBG_MAX_LENGTH);

    va_start(ap, fmt);
    
    // Build the string into my buffer
    vsprintf(dbg_buf, fmt, ap);

    va_end(ap);
    
    // Hook my handle to output the string.
    DBG_OUT(dbg_buf);   
}