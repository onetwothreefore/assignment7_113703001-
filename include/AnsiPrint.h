/* Copyright (c) 1996 by The National Chengchi Univ. All Rights Reserved */

/***************************************************************************
   NAME
     AnsiPrint.h
   PURPOSE
     Print Text with color in Ansi-enabled terminal
   NOTES
     
   AUTHOR
     Tsai-Yen Li (li@cs.nccu.edu.tw)
   HISTORY
     Tsai-Yen Li - Sep 25, 1996: Created.
***************************************************************************/

#ifndef _ANSIPRINT_H
#define _ANSIPRINT_H

#include <string>
#include "unit.h"

std::string AnsiPrint(const char* str,
                      Color fg = NOCHANGE,
                      Color bg = NOCHANGE,
                      bool hi = false,
                      bool blinking = false);

std::string AnsiPrint(const char* str,
                      bool hi = false,
                      bool blinking = false);

#endif  // _ANSIPRINT_H
