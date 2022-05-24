#include "zlib_helper.h"
#include <cstdio>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <vector>


ZlibHelper::ZlibHelper() {

}

ZlibHelper::~ZlibHelper() {

}

bool ZlibHelper::CreateZipFromDir(const std::string &dir, const std::string &zip_name) {
    bool ret = false;
    zipFile zip_file = nullptr;
    int append = APPEND_STATUS_CREATEAFTER;
    if (!CheckExistFile(zip_name)) {
        append = APPEND_STATUS_CREATE;
    }
    zip_file = zipOpen(zip_name.c_str(), append);
    if (zip_file != nullptr) {
        if (CollectFileToZip(zip_file, dir)) {
            ret = true;
        } else {
            ret = false;
            std::cout << "collect  file failed." << std::endl;
        }
        zipClose(zip_file, nullptr);
    } else {
        std::cout << "create zip file failed." << std::endl;
    }

    return ret;
}

bool ZlibHelper::CheckExistFile(const std::string &filename) {
//    bool ret = false;
//    FILE *test_file = nullptr;
//    test_file = fopen(filename.c_str(), "rb");
//    if (test_file == nullptr) {
//        ret = false;
//    } else {
//        ret = true;
//        fclose(test_file);
//    }
    bool ret = false;
    struct stat info{};
    if (stat(filename.c_str(), &info) != 0) {
        std::cout << "can't access "<< filename << std::endl;
    } else if (info.st_mode & S_IFDIR) {
        std::cout << filename << " is a directory" << std::endl;
    } else if (info.st_mode & S_IFREG) {
        ret = true;
        std::cout << filename << " is a file" << std::endl;
    } else {
        std::cout << filename << " is not exit!" << std::endl;
    }
    return ret;
}

bool ZlibHelper::CollectFileToZip(zipFile zip_file, const std::string &file_path) {
    bool ret = false;
    if (zip_file == nullptr || file_path.empty()) {
        std::cout << "zip_file or file_path is null!" << std::endl;
    } else {
        // 如果是目录
        if (CheckExistDir(file_path)) {

        } else if (CheckExistFile(file_path)) { // 如果是文件

        } else {
            ret = false;
        }
    }

    return ret;
}

bool ZlibHelper::CheckExistDir(const std::string &dir) {
    bool ret = false;
    struct stat info{};
    if (stat(dir.c_str(), &info) != 0) {
        std::cout << "can't access "<< dir << std::endl;
    } else if (info.st_mode & S_IFDIR) {
        ret = true;
        std::cout << dir << " is a directory" << std::endl;
    } else if (info.st_mode & S_IFREG) {
        std::cout << dir << " is a file" << std::endl;
    } else {
        std::cout << dir << " is not exit!" << std::endl;
    }

    return ret;
}

bool ZlibHelper::GetAllFiles(const std::string &input_dir, std::vector<std::string> &output_files,
                             std::vector<std::string> &output_dirs) {

    if (input_dir.empty()) {
        return false;
    }
    struct stat s{};
    stat(input_dir.c_str(), &s);
    if (!S_ISDIR(s.st_mode)) {
        return false;
    }
    DIR *open_dir = opendir(input_dir.c_str());
    if (open_dir == nullptr) {
        return false;
    }
    dirent *p = nullptr;
    while ((p = readdir(open_dir)) != nullptr) {
        struct stat st{};
        if (strcmp(p->d_name,".") == 0 || strcmp(p->d_name,"..") == 0) {
            continue;
        }
        std::string name = input_dir + std::string("/") + std::string(p->d_name);
        stat(name.c_str(), &st);
        if (S_ISDIR(st.st_mode)) {
            output_dirs.push_back(name);
            GetAllFiles(name, output_files, output_dirs);
        } else if (S_ISREG(st.st_mode)) {
            output_files.push_back(name);
        }
    }
    closedir(open_dir);
    return true;
}
