#pragma once

#include "Global.h"

// Starts a other application
bool startExe(LPCWSTR app_path, LPWSTR params, DWORD& pid)
{
    STARTUPINFOW si{};
    PROCESS_INFORMATION pi{};

    wstring cmdline = L"\"" + wstring(app_path) + L"\" \"" + wstring(params) + L"\"";
    bool success = CreateProcessW(NULL,    // имя исполняемого модуля
        (LPWSTR)cmdline.c_str(), // строка параметров запускаемой программы 
        NULL,    // структура SECURITY_ATTRIBUTES процесса
        NULL,    // структура SECURITY_ATTRIBUTES потока   
        FALSE,   // флаг наследования текущего процесса  
        0,       // флаги способов создания процесса   
        NULL,    // указатель на блок среды     
        NULL,    // текущий диск и каталог  
        &si,     // структура STARTUPINFO  
        &pi);    // структура PROCESS_INFORMATION   

    // ожидание закрытия процесса
    if (success)
    {
        pid = pi.dwProcessId;
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
    }
    else pid = 0x0;
    return success;
}

// Check if the application is already running
vector<DWORD> checkSelf(wchar_t* self) {
    vector<DWORD> ret;
    HANDLE hProcessSnap = 0;
    PROCESSENTRY32W pe32 = PROCESSENTRY32W();

    pe32.dwSize = sizeof(PROCESSENTRY32W);
    hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hProcessSnap == NULL or
        hProcessSnap == INVALID_HANDLE_VALUE or
        hProcessSnap == (HANDLE)ERROR_ACCESS_DENIED or
        hProcessSnap == (HANDLE)ERROR_PARTIAL_COPY or
        hProcessSnap == (HANDLE)ERROR_BAD_LENGTH) {
        return ret;
    }

    if (Process32FirstW(hProcessSnap, &pe32)) {
        do {
            if (wcscmp(pe32.szExeFile, self) == 0) {
                ret.push_back(pe32.th32ProcessID);
            }
        } while (Process32NextW(hProcessSnap, &pe32));
    }

    CloseHandle(hProcessSnap);
    return ret;
}

// Close if the application is already running
DWORD closeSelf(wchar_t* self) {
    HANDLE hProcessSnap = 0;
    PROCESSENTRY32W pe32 = PROCESSENTRY32W();

    pe32.dwSize = sizeof(PROCESSENTRY32W);
    hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hProcessSnap == (HANDLE)NULL or
        hProcessSnap == (HANDLE)INVALID_HANDLE_VALUE or
        hProcessSnap == (HANDLE)ERROR_ACCESS_DENIED or
        hProcessSnap == (HANDLE)ERROR_PARTIAL_COPY or
        hProcessSnap == (HANDLE)ERROR_BAD_LENGTH) {
        return hProcessSnap == (HANDLE)NULL ? 1 : (DWORD)hProcessSnap;
    }

    if (Process32FirstW(hProcessSnap, &pe32)) {
        do {
            if (wcscmp(pe32.szExeFile, self) == 0) {
                if (!TerminateProcess(OpenProcess(PROCESS_TERMINATE, FALSE, pe32.th32ProcessID), 0)) return GetLastError();
			}
        } while (Process32NextW(hProcessSnap, &pe32));
    }

    CloseHandle(hProcessSnap);
    return 0;
}

// Alternative to checkSelf(wchar_t*), but for constants
vector<DWORD> checkSelf(const wchar_t* self) {
    wchar_t* target = new wchar_t[MAX_PATH];
    return checkSelf(wcscpy(target, self));
}

// Alternative to closeSelf(wchar_t*), but for constants
DWORD closeSelf(const wchar_t* self) {
    wchar_t* target = new wchar_t[MAX_PATH];
    return closeSelf(wcscpy(target, self));
}

int cmdClose(const char* self) {
    return system((string("taskkill /f /IM \"") + self + "\"").c_str());
}