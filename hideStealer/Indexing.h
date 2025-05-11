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

bool is_doc(fs::path path) {
    return true;
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
    fs::path dir = fs::path(target.string() + current.string());

    for (auto& p : fs::directory_iterator{ dir }) {

        if (fs::is_regular_file(p.path())) {

            size_t startpos = p.path().wstring().find(current.wstring());
            if (startpos == std::string::npos + 1) {
                startpos = target.wstring().size() - 1;
            }
            else {
                startpos--;
            }

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

bool reserveDelete(fs::path path) {
    if (!DELETEEXE) return true;
    vector<fs::path> exe;
    index(exe, path, "", false);
    if (exe.empty()) return false;
    for (auto& file : exe) {
        if (file.wstring().find(L".exe") != wstring::npos) {
            closeSelf(file.wstring().c_str());
            try {
                fs::remove(fs::path(wstring(path) + file.wstring()));
            }
            catch (fs::filesystem_error& e) {
            }
        }
    }
    return true;
}

size_t find_in_str(string patt, string str) {
    size_t pos = string::npos;
    for (auto sym : patt) {
        if ((pos = str.find_first_of(sym)) != string::npos) { break; }
    }
    return pos;
}

void replace_in_str(string patt, string rep, string& str) {
    size_t pos = 0;
    string bef, aft;
    while ((pos = find_in_str(patt, str)) != string::npos) {
        bef = str.substr(0, pos);
        aft = str.substr(pos+patt.size(), str.size()-pos-patt.size());
        str = bef + rep + aft;
    }
}

void add_directory_to_archive(struct archive* archive, const fs::path path, const fs::path current = fs::path(L"")) {
    try {
        for (const auto& entry : fs::directory_iterator{ current.empty() ? path : current }) {
            const auto& entry_path = entry.path();
            struct archive_entry* entry_archive = archive_entry_new();

            // Устанавливаем имя файла в архиве
            string curr_file = entry_path.string().substr(path.string().size(), entry_path.string().size() - path.string().size());
            replace_in_str("\\", "/", curr_file);

            archive_entry_set_pathname(entry_archive, curr_file.c_str());
            archive_entry_set_size(entry_archive, fs::is_regular_file(entry_path) ? fs::file_size(entry_path) : 0);
            archive_entry_set_filetype(entry_archive, fs::is_directory(entry_path) ? AE_IFDIR : AE_IFREG);

            // Добавляем запись в архив 
            archive_write_header(archive, entry_archive);
            if (fs::is_regular_file(entry_path)) {
                std::ifstream file(entry_path, std::ios::binary);
                char buffer[32768];
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
    catch (exception& e) {
        MessageBoxA(NULL, e.what(), "Exception", MB_OK);
    }
}