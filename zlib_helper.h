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
    static bool GetAllFiles(zipFile zip_file, const std::string &input_dir, const std::string &home_dir, std::vector<std::string>& output_files, std::vector<std::string>& output_dirs);

    static bool UnzipFile(const std::string& zip_file_path, const std::string& output_path);
    static bool CreatedMultipleDirectory(const std::string& dir);
private:
    static bool CheckExistFile(const std::string &filename);

    static bool CheckExistDir(const std::string &dir);

    static bool CollectFileToZip(zipFile zip_file, const std::string &file_path);

    static bool AddFileToZip(zipFile zip_file, const std::string & file_name_in_zip, const std::string &file_path, bool is_dir);

    static int AddFileTime(const std::string &file, tm_zip *tmz_date, unsigned long *dos_date);

    static int CreateDirectory(const std::string &dir);

    static void ChangeFileDate(const std::string & filename,  unsigned long dos_date, tm_unz tmu_date);
};

#endif