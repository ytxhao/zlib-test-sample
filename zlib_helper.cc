#include "zlib_helper.h"
#include <cstdio>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <vector>

#define WRITE_BUFFER_SIZE (16384)
#define MAX_FILENAME (256)

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
            std::cout << "collect file failed." << std::endl;
        }
        zipClose(zip_file, nullptr);
    } else {
        std::cout << "create zip file failed." << std::endl;
    }

    return ret;
}

bool ZlibHelper::CheckExistFile(const std::string &filename) {
    bool ret = false;
    struct stat info{};
    if (stat(filename.c_str(), &info) != 0) {
        std::cout << "can't access "<< filename << std::endl;
    } else if (info.st_mode & S_IFDIR) {
//        std::cout << filename << " is a directory" << std::endl;
    } else if (info.st_mode & S_IFREG) {
        ret = true;
//        std::cout << filename << " is a file" << std::endl;
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
            std::vector<std::string> output_files;
            std::vector<std::string> output_dies;
            ret = GetAllFiles(zip_file, file_path, file_path, output_files,output_dies);
        } else if (CheckExistFile(file_path)) { // 如果是文件
            unsigned int last_dir_index = file_path.rfind('/');
            if (last_dir_index == -1) {
                ret = AddFileToZip(zip_file, file_path, file_path, false);
            } else {
                std::string file_name_in_zip = file_path.substr(last_dir_index + 1);
                ret = AddFileToZip(zip_file, file_name_in_zip, file_path, false);
            }
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
//        std::cout << dir << " is a directory" << std::endl;
    } else if (info.st_mode & S_IFREG) {
//        std::cout << dir << " is a file" << std::endl;
    } else {
        std::cout << dir << " is not exit!" << std::endl;
    }

    return ret;
}

bool ZlibHelper::GetAllFiles(zipFile zip_file, const std::string &input_dir, const std::string &home_dir, std::vector<std::string> &output_files,
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
    dirent *ent = nullptr;
    bool is_empty_dir = true;
    while ((ent = readdir(open_dir)) != nullptr) {
        struct stat st{};
        if (strcmp(ent->d_name,".") == 0 || strcmp(ent->d_name,"..") == 0) {
            continue;
        }
        std::string name = input_dir + std::string("/") + std::string(ent->d_name);
        stat(name.c_str(), &st);
        if (S_ISDIR(st.st_mode)) {
            output_dirs.push_back(name);
            GetAllFiles(zip_file, name, home_dir, output_files, output_dirs);
        } else if (S_ISREG(st.st_mode)) {
            if (zip_file != nullptr) {
                unsigned int last_dir_index = home_dir.rfind('/');
                std::string file_name_in_zip = name.substr(last_dir_index + 1);
                AddFileToZip(zip_file, file_name_in_zip, name, false);
            }
            output_files.push_back(name);
        }
        if ((ent->d_type == DT_DIR) || (ent->d_type == DT_REG)) {
            is_empty_dir = false;
        }
    }
    if (is_empty_dir) {
        if (zip_file != nullptr) {
            unsigned int last_dir_index = home_dir.rfind('/');
            std::string file_name_in_zip = input_dir.substr(last_dir_index + 1);
            AddFileToZip(zip_file, file_name_in_zip, input_dir, true);
        }
        std::cout << input_dir << " is empty directory" << std::endl;
    }
    closedir(open_dir);
    return true;
}

bool ZlibHelper::AddFileToZip(zipFile zip_file, const std::string &file_name_in_zip, const std::string &file_path, bool is_dir) {

    if (zip_file == nullptr || file_name_in_zip.empty()) {
        return false;
    }
    int err = 0;
    zip_fileinfo zinfo = {0};
    tm_zip  tmz = {0};
    zinfo.tmz_date = tmz;
    zinfo.dosDate = 0;
    zinfo.internal_fa = 0;
    zinfo.external_fa = 0;
    std::string new_file_name = file_name_in_zip;
    if (is_dir) {
        new_file_name = file_name_in_zip + "/";
    }
    err = zipOpenNewFileInZip(zip_file, new_file_name.c_str(), &zinfo, NULL, 0, NULL, 0, NULL, Z_DEFLATED, Z_DEFAULT_COMPRESSION);
    if (err != ZIP_OK) {
        std::cout << "AddFileToZip file: " << file_name_in_zip << " err:"<< err << std::endl;
        return false;
    }

    if (!is_dir) {
        FILE * fin = fopen(file_path.c_str(), "rb");
        if (fin != nullptr) {
            size_t size_read;
            do {
                err = ZIP_OK;
                unsigned char buf[WRITE_BUFFER_SIZE];
                size_read = fread(buf,1,WRITE_BUFFER_SIZE,fin);
                if (size_read < WRITE_BUFFER_SIZE) {
                    if (feof(fin) == 0) {
                        std::cout << "error in reading " << file_path << std::endl;
                        err = ZIP_ERRNO;
                    }
                }
                if (size_read > 0) {
                    err = zipWriteInFileInZip(zip_file, buf,(unsigned)size_read);
                    if (err < 0) {
                        std::cout << "error in writing " << file_path << " in the zipfile" << std::endl;
                    }
                }
            } while ((err == ZIP_OK) && (size_read > 0));
        } else {
            err = ZIP_ERRNO;
            std::cout << "error in opening  " << file_path << " for reading" << std::endl;
        }
    }

    zipCloseFileInZip(zip_file);
    return true;
}

int ZlibHelper::AddFileTime(const std::string &file, tm_zip *tmzip, unsigned long *dt) {
    return 0;
}
