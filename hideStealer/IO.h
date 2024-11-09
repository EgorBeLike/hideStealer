#pragma once

#include "Global.h"

#define ifDebug(func)  if(ISDEBUG) func                                                       // Execute function if debug mode is enabled
#define ifNDebug(func) if(!ISDEBUG) func                                                      // Execute function if debug mode is disabled
#define cout(text)     if(LOGGING) log_file << text << L"\n"; ifDebug(wcout << text << "\n")  // Print data if debug mode is enabled
#define acout(text)    if(ADVANCEDDEBUG && ISDEBUG) { \
	if(LOGGING) log_file << text << L"\n"; wcout << text << "\n"; }                           // Print data if advance debug mode is enabled
#define isna(obj)      ((obj) ? obj : "N/A")                                                  // Return "N/A" if object is null
#define newline        cout("")                                                               // Print new line if debug mode is enabled

// Print file data if debug mode is enabled
void printFile(const wchar_t* filename) {
	ifNDebug(return);
	wifstream f(filename);
	f.imbue(locale(".utf-8"));
	if (f.is_open() && f.good() && !f.eof()) { for (wstring line; getline(f, line);) { acout(line); } }
	else { cout(L"File isn't opened: is open = " << f.is_open() << L"; good = " << f.good() << L"; EOF = " << f.eof()); }
	f.close();
}