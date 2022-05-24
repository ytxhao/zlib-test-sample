#ifndef ZLIB_HELPER_H_
#define ZLIB_HELPER_H_

#include <iostream>
#include <string>
#include "zip.h"
#include "unzip.h"
#include "zlib.h"

class ZlibHelper {
public:
    ZlibHelper();
    ~ZlibHelper();

    static bool CreateZipFromDir(const std::string &dir, const std::string &zip_name);
    static bool GetAllFiles(const std::string &input_dir, std::vector<std::string>& output_files, std::vector<std::string>& output_dirs);

private:
    static bool CheckExistFile(const std::string &filename);

    static bool CheckExistDir(const std::string &dir);

    static bool CollectFileToZip(zipFile zip_file, const std::string &file_path);

};

#endif