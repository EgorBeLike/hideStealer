#pragma once

#include "Global.h"

#define ifDebug(func)             if(ISDEBUG) func                                 // Execute function if debug mode is enabled
#define ifNDebug(func)            if(!ISDEBUG) func                                // Execute function if debug mode is disabled
#define cout(x, y, color, text)   if(LOGGING) log_file << text << L"\n"; \
								  SetCurPos(x, y); SetCol(color); \
								  ifDebug(wcout << text);                          // Print data if debug mode is enabled
#define acout(x, y, color, text)  if(ADVANCEDDEBUG) cout(x,y,color,text)           // Print data if advance debug mode is enabled
#define isna(obj)                 ((obj) ? obj : "N/A")                            // Return "N/A" if object is null

enum Color {
    BLACK           = 0,
    FDBLUE          = FOREGROUND_BLUE,
    FDGREEN         = FOREGROUND_GREEN,
    FDCYAN          = FOREGROUND_GREEN | FOREGROUND_BLUE,
    FDRED           = FOREGROUND_RED,
    FDMAGENTA       = FOREGROUND_RED | FOREGROUND_BLUE,
    FDYELLOW        = FOREGROUND_RED | FOREGROUND_GREEN,
    FDGRAY          = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE,
    FGRAY           = FOREGROUND_INTENSITY,
    FBLUE           = FOREGROUND_INTENSITY | FOREGROUND_BLUE,
    FGREEN          = FOREGROUND_INTENSITY | FOREGROUND_GREEN,
    FCYAN           = FOREGROUND_INTENSITY | FOREGROUND_GREEN | FOREGROUND_BLUE,
    FRED            = FOREGROUND_INTENSITY | FOREGROUND_RED,
    FMAGENTA        = FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_BLUE,
    FYELLOW         = FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN,
    FWHITE          = FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE,
    BDBLUE          = BACKGROUND_BLUE,
    BDGREEN         = BACKGROUND_GREEN,
    BDCYAN          = BACKGROUND_GREEN | BACKGROUND_BLUE,
    BDRED           = BACKGROUND_RED,
    BDMAGENTA       = BACKGROUND_RED | BACKGROUND_BLUE,
    BDYELLOW        = BACKGROUND_RED | BACKGROUND_GREEN,
    BDGRAY          = BACKGROUND_RED | BACKGROUND_GREEN | BACKGROUND_BLUE,
    BGRAY           = BACKGROUND_INTENSITY,
    BBLUE           = BACKGROUND_INTENSITY | BACKGROUND_BLUE,
    BGREEN          = BACKGROUND_INTENSITY | BACKGROUND_GREEN,
    BCYAN           = BACKGROUND_INTENSITY | BACKGROUND_GREEN | BACKGROUND_BLUE,
    BRED            = BACKGROUND_INTENSITY | BACKGROUND_RED,
    BMAGENTA        = BACKGROUND_INTENSITY | BACKGROUND_RED | BACKGROUND_BLUE,
    BYELLOW         = BACKGROUND_INTENSITY | BACKGROUND_RED | BACKGROUND_GREEN,
    BWHITE          = BACKGROUND_INTENSITY | BACKGROUND_RED | BACKGROUND_GREEN | BACKGROUND_BLUE,
};