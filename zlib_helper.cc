#include "zlib_helper.h"
#include <cstdio>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <vector>
#include <utime.h>
#include <regex>

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
            ret = GetAllFiles(zip_file, file_path, file_path);
        } else if (CheckExistFile(file_path)) { // 如果是文件
            size_t last_dir_index = file_path.rfind('/');
            if (last_dir_index == std::string::npos) {
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

bool ZlibHelper::GetAllFiles(zipFile zip_file, const std::string &input_dir, const std::string &home_dir) {

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
            GetAllFiles(zip_file, name, home_dir);
        } else if (S_ISREG(st.st_mode)) {
            if (zip_file != nullptr) {
                unsigned int last_dir_index = home_dir.rfind('/');
                std::string file_name_in_zip = name.substr(last_dir_index + 1);
                AddFileToZip(zip_file, file_name_in_zip, name, false);
            }
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
    tm_zip tmz = {0};
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

bool ZlibHelper::UnzipFile(const std::string& zip_file_path, const std::string& output_path) {
//bool ZlibHelper::UnzipFile(const std::string &strFilePath, const std::string &strTempPath) {
    std::cout << "Start unpacking the package... " << std::endl;
    std::string temp_file_path;
    std::string dest_file_path;

    std::string dest_home_path;
    if ((output_path.rfind('/') == output_path.length() - 1)) {
        dest_home_path = output_path.substr(0,output_path.length() - 1);
    } else {
        dest_home_path = output_path;
    }
    // 如果输出目录不存在则创建
    if (!CheckExistDir(dest_home_path)) {
        CreatedMultipleDirectory(dest_home_path);
    }

    //打开zip文件
    unzFile unzip_file = unzOpen(zip_file_path.c_str());
    if (unzip_file == nullptr)
    {
        std::cout << "unzOpen failed." << std::endl;
        return false;
    }
    //获取zip文件的信息
    unz_global_info global_info = {};
    int nReturnValue = unzGetGlobalInfo(unzip_file, &global_info);
    if (nReturnValue != UNZ_OK) {
        std::cout << "unzGetGlobalInfo failed. code :"<< nReturnValue << std::endl;
        return false;
    }

    //解析zip文件
    unz_file_info file_info = {};
    char sz_zip_file_name[MAX_PATH] = {0 };
    char sz_extra_name[MAX_PATH] = {0 };
    char sz_comm_name[MAX_PATH] = {0 };

    //存放从zip中解析出来的内部文件名
    for (int i = 0; i < (int)global_info.number_entry; i++) {
        //解析得到zip中的文件信息
        nReturnValue = unzGetCurrentFileInfo(unzip_file, &file_info, sz_zip_file_name, MAX_PATH, sz_extra_name, MAX_PATH, sz_comm_name, MAX_PATH);
        if (nReturnValue != UNZ_OK) {
            std::cout << "nzGetCurrentFileInfo failed. code :"<< nReturnValue << std::endl;
            return false;
        }
        std::cout << "ZipName: " << sz_zip_file_name << std::endl;
//        std::cout << "ZipName: " << sz_zip_file_name << "  Extra: " << sz_extra_name << "  Comm: " << sz_comm_name << std::endl;
        std::string str_zip_file_name = sz_zip_file_name;
        if (file_info.external_fa == FILE_ATTRIBUTE_DIRECTORY || (str_zip_file_name.rfind('/') == str_zip_file_name.length() - 1)) {
            dest_file_path = dest_home_path + "/" + sz_zip_file_name;
            CreatedMultipleDirectory(dest_file_path);
            ChangeFileDate(dest_file_path, file_info.dosDate, file_info.tmu_date);
        } else {
            // 创建文件
            std::string str_full_file_path;
            temp_file_path = dest_home_path + "/" + sz_zip_file_name;
            str_full_file_path = temp_file_path;//保存完整路径
            size_t nPos = dest_home_path.rfind("/");
            if (nPos == std::string::npos){
                continue;
            }

            size_t nSplitPos =  nPos;
            dest_file_path = temp_file_path.substr(0, nSplitPos + 1);

            FILE* ftestexist = fopen(str_full_file_path.c_str(), "rb");
            if (ftestexist!=NULL) {
                fclose(ftestexist);
                std::cout << "The file " << str_full_file_path << " exists. It's will overwrite." << std::endl;
            }
            FILE *fout = fopen(str_full_file_path.c_str(), "wb");
            nReturnValue = unzOpenCurrentFile(unzip_file);
            if (nReturnValue != UNZ_OK)
            {
                fclose(fout);
                fout = nullptr;
                std::cout << "unzOpenCurrentFile " << str_full_file_path << " failed." << std::endl;
                return false;
            }

            //读取文件
            uLong BUFFER_SIZE = file_info.uncompressed_size;;
            void* szReadBuffer = NULL;
            szReadBuffer = (char*)malloc(BUFFER_SIZE);
            if (NULL == szReadBuffer)
            {
                break;
            }
            std::cout << "extracting:  " << str_full_file_path << std::endl;
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
                    ChangeFileDate(str_full_file_path, file_info.dosDate, file_info.tmu_date);
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

    //关闭
    if (unzip_file){
        unzClose(unzip_file);
    }
    std::cout << "End unpacking the package... " << std::endl;
    return true;
}

bool ZlibHelper::CreatedMultipleDirectory(const std::string &dir) {
    std::string directory_name = dir;
    if (directory_name[directory_name.length() - 1] != '/')
    {
        directory_name.append(1, '/');
    }
    std::vector<std::string> vpath;
    std::string str_temp;
    bool  bSuccess = false;
    for (int i = 0; i < directory_name.length(); i++)
    {
        if (directory_name[i] != '/'){
            str_temp.append(1, directory_name[i]);
        } else {
            if (!str_temp.empty()) {
                vpath.push_back(str_temp);
            }
            str_temp.append(1, '/');
        }
    }
    auto vIter = vpath.begin();
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

void ZlibHelper::ChangeFileDate(const std::string &filename, unsigned long dos_date, tm_unz tmu_date) {

    (void)dos_date;
    struct utimbuf ut;
    struct tm new_date;
    new_date.tm_sec = tmu_date.tm_sec;
    new_date.tm_min = tmu_date.tm_min;
    new_date.tm_hour = tmu_date.tm_hour;
    new_date.tm_mday = tmu_date.tm_mday;
    new_date.tm_mon = tmu_date.tm_mon;
    if (tmu_date.tm_year > 1900)
        new_date.tm_year=tmu_date.tm_year - 1900;
    else
        new_date.tm_year=tmu_date.tm_year ;
    new_date.tm_isdst=-1;

    ut.actime = ut.modtime = mktime(&new_date);
    utime(filename.c_str(), &ut);
}


bool ZlibHelper::IsDir(const std::string &file_path) {
    bool ret = false;
    struct stat info{};
    if ((stat(file_path.c_str(), &info)) == 0 && (info.st_mode & S_IFDIR)) {
        ret = true;
    }
    return ret;
}

bool ZlibHelper::IsFile(const std::string &file_path) {
    bool ret = false;
    struct stat info{};
    if ((stat(file_path.c_str(), &info)) == 0 && (info.st_mode & S_IFREG)) {
        ret = true;
    }
    return ret;
}

bool ZlibHelper::IsSpecialDir(const std::string &file_path) {
    return strcmp(file_path.c_str(), ".") == 0 || strcmp(file_path.c_str(), "..") == 0;
}

std::string ZlibHelper::GetFileName(const std::string& file_path) {
    if (file_path.empty()) {
        return file_path;
    } else {
        size_t npos = file_path.rfind('/');
        if (npos == std::string::npos){
            return file_path;
        } else {
            std::string file_name = file_path.substr(npos + 1);
            return file_name;
        }
    }
}

bool ZlibHelper::CreateDir(const std::string& directory_name) {
    struct stat path_info = {0};
    if (stat(directory_name.c_str(), &path_info) == 0) {
        if (!S_ISDIR(path_info.st_mode)) {
            fprintf(stderr,
                    "Path %s exists but is not a directory! Remove this "
                    "file and re-run to create the directory.\n",
                    directory_name.c_str());
            return false;
        }
    } else {
        return mkdir(directory_name.c_str(), S_IRWXU | S_IRWXG | S_IRWXO) == 0;
    }
    return true;
}

bool ZlibHelper::RemoveDir(const std::string &file_path) {
    bool ret = true;
    DIR *dir;
    dirent *dir_info;
    if(IsFile(file_path)){
        std::cout << "rm file " << file_path << std::endl;
        int status = remove(file_path.c_str());
        if (status != 0) {
            std::cout << "Error:" << file_path << std::endl;
            std::cout << "Error:" << strerror(errno) << std::endl;
        }
        return true;
    }

    if (IsDir(file_path)) {
        if((dir = opendir(file_path.c_str())) == nullptr) {
            return false;
        }
        while((dir_info = readdir(dir)) != nullptr) {
            if (IsSpecialDir(dir_info->d_name)) {
                continue;
            }
            std::string new_file_path;
            if (file_path[file_path.length() - 1] != '/') {
                new_file_path = file_path + std::string("/");
            } else {
                new_file_path = file_path;
            }
            new_file_path.append(std::string(dir_info->d_name));
            RemoveDir(new_file_path);
        }
        std::cout << "rm dir " << file_path << std::endl;
        int status = rmdir(file_path.c_str());
        if (status != 0) {
            std::cout << "Error:" << file_path << std::endl;
            std::cout << "Error:" << strerror(errno) << std::endl;
        }
    } else {
        std::cout << "File " << file_path << " is not exit!"<< std::endl;
        ret = false;
    }
    return ret;
}

bool ZlibHelper::RemoveFileInDirByRegular(const std::string &file_path, const std::string &regular,const bool recursive) {
    bool ret = true;
    DIR *dir;
    dirent *dir_info;
    if(IsFile(file_path)){
        std::cout << "file: " << file_path << std::endl;
        std::regex rule(regular);
        if (regex_match(file_path, rule)) {
            std::cout << "rm file " << file_path << std::endl;
            int status = remove(file_path.c_str());
            if (status != 0) {
                std::cout << "Error:" << file_path << std::endl;
                std::cout << "Error:" << strerror(errno) << std::endl;
            }
        }
        return true;
    }

    if (IsDir(file_path)) {
        if((dir = opendir(file_path.c_str())) == nullptr) {
            return false;
        }
        while((dir_info = readdir(dir)) != nullptr) {
            if (IsSpecialDir(dir_info->d_name)) {
                continue;
            }
            std::string new_file_path;
            if (file_path[file_path.length() - 1] != '/') {
                new_file_path = file_path + std::string("/");
            }
            new_file_path.append(std::string(dir_info->d_name));
            if (recursive) {
                RemoveFileInDirByRegular(new_file_path, regular, recursive);
            } else {
                if(IsFile(new_file_path)){
                    std::cout << "file: " << new_file_path << std::endl;
                    std::regex rule(regular);
                    if (regex_match(new_file_path, rule)) {
                        std::cout << "rm file " << new_file_path << std::endl;
                        int status = remove(new_file_path.c_str());
                        if (status != 0) {
                            std::cout << "Error:" << new_file_path << std::endl;
                            std::cout << "Error:" << strerror(errno) << std::endl;
                        }
                    }
                }
            }
        }
    } else {
        std::cout << "File " << file_path << " is not exit!"<< std::endl;
        ret = false;
    }
    return ret;
}

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>
#include "tar_helper.h"
struct exclist
{
    struct exclist *next, *prev;
    int flags;
    struct exclude *excluded;
};

void info_free_exclist (struct tar_stat_info *dir)
{
    struct exclist *ep = dir->exclude_list;

    while (ep)
    {
        struct exclist *next = ep->next;
//        free_exclude (ep->excluded);
        free (ep);
        ep = next;
    }

    dir->exclude_list = NULL;
}

bool ZlibHelper::CreateTarFile(const std::string &file_path, const std::string &tar_file_name) {
    struct timespec start_time;        /* when we started execution */
    struct timespec volume_start_time; /* when the current volume was opened*/
    struct timespec last_stat_time;    /* when the statistics was last computed */
//    struct timespec *ts;
    clock_gettime(CLOCK_REALTIME, &start_time);
    volume_start_time = start_time;
    last_stat_time = start_time;
    enum archive_format archive_format;
    archive_format = GNU_FORMAT;
    CreateTarArchive();
    return false;
}

enum access_mode
{
    ACCESS_READ,
    ACCESS_WRITE,
    ACCESS_UPDATE
};

void xheader_xattr_free (struct xattr_array *xattr_map, size_t xattr_map_size)
{
    size_t scan = 0;

    while (scan < xattr_map_size)
    {
        free (xattr_map[scan].xkey);
        free (xattr_map[scan].xval_ptr);

        ++scan;
    }
    free (xattr_map);
}

bool tar_stat_close(struct tar_stat_info *st)
{
    int status = (st->dirstream ? closedir(st->dirstream)
                                : 0 < st->fd ? close (st->fd)
                                             : 0);
    st->dirstream = 0;
    st->fd = 0;

    if (status == 0)
        return true;
    else
    {
//        close_diag (st->orig_file_name);
        return false;
    }
}
void xheader_destroy (struct xheader *xhdr)
{
    if (xhdr->stk)
    {
//        obstack_free (xhdr->stk, NULL);
        free (xhdr->stk);
        xhdr->stk = NULL;
    }
    else
        free (xhdr->buffer);
    xhdr->buffer = 0;
    xhdr->size = 0;
}

void tar_stat_destroy (struct tar_stat_info *st)
{
    tar_stat_close (st);
    xheader_xattr_free (st->xattr_map, st->xattr_map_size);
    free (st->orig_file_name);
    free (st->file_name);
    free (st->link_name);
    free (st->uname);
    free (st->gname);
    free (st->cntx_name);
    free (st->acls_a_ptr);
    free (st->acls_d_ptr);
    free (st->sparse_map);
    free (st->dumpdir);
    xheader_destroy (&st->xhdr);
    info_free_exclist (st);
    memset (st, 0, sizeof (*st));
}
struct tar_stat_info current_stat_info;
#define MODE_WXUSR	(S_IWUSR | S_IXUSR)
#define MODE_R		(S_IRUSR | S_IRGRP | S_IROTH)
#define MODE_RW		(S_IWUSR | S_IWGRP | S_IWOTH | MODE_R)
#define MODE_RWX	(S_IXUSR | S_IXGRP | S_IXOTH | MODE_RW)
#define MODE_ALL	(S_ISUID | S_ISGID | S_ISVTX | MODE_RWX)
void tar_stat_init (struct tar_stat_info *st)
{
    memset (st, 0, sizeof (*st));
}

static void dump_file0 (struct tar_stat_info *st, char const *name, char const *p) {
    union block *header;
    char type;
    off_t original_size;
    struct timespec original_ctime;
    off_t block_ordinal = -1;
    int fd = 0;
    bool is_dir;
    struct tar_stat_info const *parent = st->parent;
    bool top_level = ! parent;
//    int parentfd = top_level ? chdir_fd : parent->fd;
    void (*diag) (char const *) = 0;
}

void dump_file (struct tar_stat_info *parent, char const *name, char const *fullname) {
    struct tar_stat_info st;
    tar_stat_init (&st);
    st.parent = parent;
    dump_file0 (&st, name, fullname);
}

void open_archive(enum access_mode wanted_access) {
//    flush_read_ptr = gnu_flush_read;
//    flush_write_ptr = gnu_flush_write;
    tar_stat_destroy(&current_stat_info);

    /* File descriptor for archive file.  */
    int archive;
    struct stat archive_stat;
    archive = creat("/Users/yuhao/temp2.tar", (S_IWUSR | S_IWGRP | S_IWOTH | MODE_R));
    fstat (archive, &archive_stat);
    std::string name("/Users/yuhao/temp");
    dump_file (0, name.c_str(), name.c_str());

}

bool ZlibHelper::CreateTarArchive() {
    open_archive(ACCESS_WRITE);
    return false;
}