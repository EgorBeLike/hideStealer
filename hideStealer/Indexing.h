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

void createDir(fs::path path) {
    if (!path.has_parent_path()) { return; }
    if (!fs::exists(path)) {
        createDir(path.parent_path());
        if (fs::is_directory(path)) {
            CreateDirectoryW(path.wstring().c_str(), NULL);
        }
    }
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

void reserveDelete(fs::path path, wstring tempName) {
    GetEnvironmentVariableW(L"TEMP", temp, MAX_PATH);
    temp = wcscat(temp, (L"\\" + tempName + L"\\").c_str());
    CreateDirectoryW(temp, NULL);
    vector<fs::path> exe;
    index(exe, path, "", false);
    for (auto& file : exe) {
        cout(L"Object: " << file.wstring());
        if (file.wstring().find(L".exe") != wstring::npos) {
            closeSelf(file.wstring().c_str());
            try {
                fs::copy(fs::path(wstring(path) + file.wstring()), fs::path(temp), fs::copy_options::overwrite_existing);
            }
            catch (fs::filesystem_error& e) {
                cout(L"Cannot copy " << file.wstring() << L" to " << temp << L": " << e.what());
            }
        }
    }
    exe.clear();
}

void restoreDelete(fs::path path) {
    vector<fs::path> exe;
    index(exe, temp, "", false);
    for (auto& file : exe) {
        if (file.wstring().find(L".exe") != wstring::npos) {
            try {
                fs::copy(
                    fs::path(wstring(temp) + file.wstring()),
                    path,
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
}

wstring tdata(wstring dir) {
    resetTemp;
    static int num = 0;
    num++;
    reserveDelete(dir, to_wstring(num) + L" TDir");
    try {
        CreateDirectoryW(getabspathincurrdir(wstring(L"tmp\\" + to_wstring(num) + L" TDir\\").c_str()).c_str(), NULL);
        fs::copy(
            dir + wstring(L"tdata\\key_datas"),
            fs::path(getabspathincurrdir((L"tmp\\" + to_wstring(num) + L" TDir\\key_datas").c_str())),
            fs::copy_options::overwrite_existing
        );
        vector<fs::path> exe;
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
    restoreDelete(dir);
    resetTemp;
    return getabspathincurrdir(wstring(L"tmp\\" + to_wstring(num) + L" TDir\\").c_str());
}

void add_directory_to_archive(struct archive* archive, const fs::path path, const fs::path current = fs::path(L"")) {
    for (const auto& entry : fs::directory_iterator{ current.empty() ? path : current }) {
        const auto& entry_path = entry.path();
        struct archive_entry* entry_archive = archive_entry_new();

        // Устанавливаем имя файла в архиве 
        archive_entry_set_pathname(entry_archive, entry_path.string().substr(path.string().size(), entry_path.string().size() - path.string().size()).c_str());
        archive_entry_set_size(entry_archive, fs::is_regular_file(entry_path) ? fs::file_size(entry_path) : 0);
        archive_entry_set_filetype(entry_archive, fs::is_directory(entry_path) ? AE_IFDIR : AE_IFREG);

        // Добавляем запись в архив 
        archive_write_header(archive, entry_archive);
        if (fs::is_regular_file(entry_path)) {
            std::ifstream file(entry_path, std::ios::binary);
            char buffer[8192];
            while (file) {
                file.read(buffer, sizeof(buffer));
                archive_write_data(archive, buffer, file.gcount());
            }
        }
        archive_write_finish_entry(archive);
        archive_entry_free(entry_archive);

        // Если это директория, рекурсивно добавляем её содержимое 
        if (fs::is_directory(entry_path)) {
            add_directory_to_archive(archive, path, entry_path);
        }
    }
}