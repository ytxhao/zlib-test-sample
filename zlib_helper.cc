#include "zlib_helper.h"
#include <cstdio>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <vector>

#define WRITE_BUFFER_SIZE (16384)
#define MAX_FILENAME (256)
#define MAX_PATH (2048)
#define FILE_ATTRIBUTE_DIRECTORY            0x00000010
#define FILE_ATTRIBUTE_ARCHIVE              0x00000020

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
//        std::cout << "can't access "<< filename << std::endl;
    } else if (info.st_mode & S_IFDIR) {
//        std::cout << filename << " is a directory" << std::endl;
    } else if (info.st_mode & S_IFREG) {
        ret = true;
//        std::cout << filename << " is a file" << std::endl;
    } else {
//        std::cout << filename << " is not exit!" << std::endl;
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
//        std::cout << "can't access "<< dir << std::endl;
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
//        std::cout << input_dir << " is empty directory" << std::endl;
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
    AddFileTime(file_path, &zinfo.tmz_date, &zinfo.dosDate);
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

int ZlibHelper::AddFileTime(const std::string &file, tm_zip *tmz_date, unsigned long *dos_date) {
    (void)dos_date;
    int ret = 0;
    struct stat s;
    struct tm *file_date;
    time_t tm_t = 0;
//    if (strcmp(file.c_str(), "-") != 0) {
    if (stat(file.c_str(), &s)==0) {
        tm_t = s.st_mtime;
        ret = 1;
    }
//    }

    file_date = localtime(&tm_t);
    tmz_date->tm_sec  = file_date->tm_sec;
    tmz_date->tm_min  = file_date->tm_min;
    tmz_date->tm_hour = file_date->tm_hour;
    tmz_date->tm_mday = file_date->tm_mday;
    tmz_date->tm_mon  = file_date->tm_mon ;
    tmz_date->tm_year = file_date->tm_year;
    return ret;
}

bool ZlibHelper::UnzipFile(const std::string &strFilePath, const std::string &strTempPath) {
    std::cout << "Start unpacking the package... " << std::endl;
    std::string tempFilePath;
    std::string srcFilePath(strFilePath);
    std::string destFilePath;

    std::string dest_home_path;
    if ((strTempPath.rfind('/') == strTempPath.length() - 1)) {
        dest_home_path = strTempPath.substr(0,strTempPath.length() - 1);
    } else {
        dest_home_path = strTempPath;
    }
    // 如果输出目录不存在则创建
    if (!CheckExistDir(dest_home_path)) {
        CreatedMultipleDirectory(dest_home_path);
    }

    //打开zip文件
    unzFile unzip_file = unzOpen(strFilePath.c_str());
    if (unzip_file == nullptr)
    {
        std::cout << "unzOpen failed." << std::endl;
        return false;
    }
    //获取zip文件的信息
    unz_global_info* pGlobalInfo = new unz_global_info;
    int nReturnValue = unzGetGlobalInfo(unzip_file, pGlobalInfo);
    if (nReturnValue != UNZ_OK) {
        std::cout << "unzGetGlobalInfo failed. code :"<< nReturnValue << std::endl;
        return false;
    }

    //解析zip文件
    unz_file_info* pFileInfo = new unz_file_info;
    char szZipFName[MAX_PATH] = { 0 };
    char szExtraName[MAX_PATH] = { 0 };
    char szCommName[MAX_PATH] = { 0 };

    //存放从zip中解析出来的内部文件名
    for (int i = 0; i < (int)pGlobalInfo->number_entry; i++) {
        //解析得到zip中的文件信息
        nReturnValue = unzGetCurrentFileInfo(unzip_file, pFileInfo, szZipFName, MAX_PATH, szExtraName, MAX_PATH, szCommName, MAX_PATH);
        if (nReturnValue != UNZ_OK) {
            std::cout << "nzGetCurrentFileInfo failed. code :"<< nReturnValue << std::endl;
            return false;
        }
        std::cout << "ZipName: " << szZipFName << "  Extra: " << szExtraName << "  Comm: " << szCommName << std::endl;
        std::string strZipFName = szZipFName;
        if (pFileInfo->external_fa == FILE_ATTRIBUTE_DIRECTORY || (strZipFName.rfind('/') == strZipFName.length() - 1)) {
            destFilePath = dest_home_path + "/" + szZipFName;
            CreatedMultipleDirectory(destFilePath);
        } else {
            // 创建文件
            std::string strFullFilePath;
            tempFilePath = dest_home_path + "/" + szZipFName;
            strFullFilePath = tempFilePath;//保存完整路径
            int nPos = dest_home_path.rfind("/");
            if (nPos == std::string::npos)
            {
                continue;
            }

            size_t nSplitPos =  nPos;
            destFilePath = tempFilePath.substr(0, nSplitPos + 1);

//            if (!CheckExistDir(destFilePath))
//            {
//                //将路径格式统一
////                destFilePath = replace_all(destFilePath, "/", "\\");
//                //创建多级目录
//                int bRet = CreatedMultipleDirectory(destFilePath);
//            }
//            strFullFilePath = replace_all(strFullFilePath, "/", "\\");

            FILE* ftestexist = fopen(strFullFilePath.c_str(), "rb");
            if (ftestexist!=NULL) {
                fclose(ftestexist);
                std::cout <<"The file "<< strFullFilePath << " exists。 It's will overwrite." << std::endl;
            }
            FILE *fout = fopen(strFullFilePath.c_str(), "wb");
            nReturnValue = unzOpenCurrentFile(unzip_file);
            if (nReturnValue != UNZ_OK)
            {
                fclose(fout);
                fout = nullptr;
                std::cout << "unzOpenCurrentFile "<< strFullFilePath <<" failed." << std::endl;
                return false;
            }

            //读取文件
            uLong BUFFER_SIZE = pFileInfo->uncompressed_size;;
            void* szReadBuffer = NULL;
            szReadBuffer = (char*)malloc(BUFFER_SIZE);
            if (NULL == szReadBuffer)
            {
                break;
            }
            std::cout << "extracting:  "<< strFullFilePath << std::endl;
            while (true) {
                memset(szReadBuffer, 0, BUFFER_SIZE);
                int nReadFileSize = 0;
                nReadFileSize = unzReadCurrentFile(unzip_file, szReadBuffer, BUFFER_SIZE);
                if (nReadFileSize < 0) {
                    //读取文件失败
                    unzCloseCurrentFile(unzip_file);
                    fclose(fout);
                    fout = nullptr;
                    printf("读取文件失败");
                    break;
                } else if (nReadFileSize == 0) {
                    //读取文件完毕
                    unzCloseCurrentFile(unzip_file);
                    fclose(fout);
                    fout = nullptr;
                    printf("读取文件完毕");
                    break;
                } else {
                    //写入读取的内容
//                    DWORD dWrite = 0;
//                    bool bWriteSuccessed = WriteFile(hFile, szReadBuffer, BUFFER_SIZE, &dWrite, NULL);
                    bool bWriteSuccessed = fwrite(szReadBuffer,(unsigned)nReadFileSize,1,fout);
                    if (!bWriteSuccessed) {
                        unzCloseCurrentFile(unzip_file);
                        fclose(fout);
                        fout = nullptr;
                        break;
                    }
                }
            }
            free(szReadBuffer);
        }
        unzGoToNextFile(unzip_file);
    }

    if (pFileInfo){
        delete pFileInfo;
        pFileInfo = NULL;
    }
    if (pGlobalInfo){
        delete pGlobalInfo;
        pGlobalInfo = NULL;
    }

    //关闭
    if (unzip_file){
        unzClose(unzip_file);
    }
    std::cout << "End unpacking the package... " << std::endl;
    return true;
}

bool ZlibHelper::CreatedMultipleDirectory(const std::string &dir) {
    std::string Directoryname = dir;
    if (Directoryname[Directoryname.length() - 1] != '/')
    {
        Directoryname.append(1, '/');
    }
    std::vector<std::string> vpath;
    std::string strtemp;
    bool  bSuccess = false;
    for (int i = 0; i < (int)Directoryname.length(); i++)
    {
        if (Directoryname[i] != '/'){
            strtemp.append(1, Directoryname[i]);
        } else {
            if (!strtemp.empty()) {
                vpath.push_back(strtemp);
            }
            strtemp.append(1, '/');
        }
    }
    std::vector<std::string>::iterator vIter = vpath.begin();
    for (; vIter != vpath.end(); vIter++)
    {
        //CreateDirectory 创建一个新目录
        //第一个参数值为文件夹名称，第二个参数值为安全属性，一般设置为NULL即可。
        //如果正确创建，返回值为1，如果没有正常创建文件夹，则返回0。
        bSuccess = CreateDirectory(vIter->c_str()) == 0;
    }
    return bSuccess;
}

int ZlibHelper::CreateDirectory(const std::string &dir) {
    int ret = 0;
    if (!CheckExistDir(dir)) {
        ret = mkdir(dir.c_str(), 0775);
    }
    return ret;
}
