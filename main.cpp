#include <iostream>//
#include <sstream>
#include <vector>
#include <thread>
#include <regex>
#include "zlib_helper.h"
// 1652691410983

void CreateZipFromDir(std::string dirName, std::string zipFileName) {

}

int main() {

    ZlibHelper helper;
    std::vector<std::string> output_files;
    std::vector<std::string> output_dies;
//    ZlibHelper::GetAllFiles(nullptr, ".",output_files,output_dies);
//    for(auto & file : output_files)
//    {
//        std::cout << file << std::endl;
//    }
//    std::cout << "===========================" << std::endl;
//    for(auto & dir : output_dies)
//    {
//        std::cout << dir << std::endl;
//    }
    std::unique_ptr<FILE, std::function<void(FILE *)>> pPushDataFile;
    std::unique_ptr<FILE, std::function<void(FILE *)>> pFile(fopen("/Users/yuhao/workspace/tmp/zorro_push_data.txt", "wb"),
                                                             [](FILE *file) {
                                                                 std::cout << "file zorro_push_data.pcm close" << std::endl;
                                                                 fclose(file);
                                                             });
    pPushDataFile = std::move(pFile);
//    FILE *pFile = fopen("zorro_push_data.txt", "wb");
    bool t_stop = false;
    std::thread thread([&](){
        int i = 0;
        std::stringstream sstream;

        while (!t_stop) {
            std::this_thread::sleep_for (std::chrono::milliseconds(50));
            auto now = std::chrono::system_clock::now();
            auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch());
            char now_str[64];
            struct std::tm *ttime;
            time_t current_seconds = ms.count() / 1000;
            ttime = localtime(&current_seconds);
            std::strftime(now_str, 64, "%Y-%m-%d %H:%M:%S", ttime);
//            std::cout << ms.count() << std::endl;
            sstream << now_str << "." << std::to_string(ms.count() % 1000000) << std::endl;
//            char buffer[1024] = {0};
//            // ?????????????????????
//            buffer[0] = 'H';
//            buffer[1] = 'e';
//            buffer[2] = 'l';
//            buffer[3] = 'l';
//            buffer[4] = 'o';
//            size_t write_size = fwrite(buffer, sizeof(char), 5, pFile);
            std::string str(sstream.str());
            fwrite(str.c_str(), str.length(), 1, pPushDataFile.get());
//            std::cout << sstream.str() << "write_size:"<< write_size <<std::endl;
//            stringstream?????????????????????????????????????????????????????????clear?????????
//            ??????stringstream?????????????????????????????????stringstream?????????????????????????????????
//            ??????????????????????????????.str("")?????????
            sstream.str("");
            sstream.clear();
//            fclose(pFile);
            fflush(pPushDataFile.get());

        }
    });

//    ZlibHelper::CreateZipFromDir("/Users/yuhao/workspace/libzegoliveroom.so","/Users/yuhaoo/workspace/tmp2.zip");
//    ZlibHelper::UnzipFile("/Users/yuhaoo/workspace/tmp.zip", "/Users/yuhaoo/workspace/tmp_unzip/");
//    getchar();
    t_stop = true;
    thread.join();
//    bool ret = ZlibHelper::CreateDir("/Users/yuhao/workspace/tmp/di/dia");
//    ZlibHelper::RemoveFileInDirByRegular("/Users/yuhao/workspace/tmp2",".*\\.txt");
//    std::string file_path("/Users/yuhao/workspace/tmp2/ags.txt");
//    std::cout << "file: " << file_path << std::endl;
//    std::regex rule(".*\\.txt");
//    if (regex_match(file_path, rule))
//    std::cout <<"regex_match "<< regex_match(file_path, rule)<< std::endl;
//    ZlibHelper::CreateZipFromDir("/Users/yuhao/workspace/tmp/","/Users/yuhao/workspace/tmp.zip");
    std::string file_name = ZlibHelper::GetFileName("/fa/tmp.txt");
    std::cout << "file_name: " << file_name << std::endl;
    ZlibHelper::RemoveFileInDirByRegular("/Users/yuhao/temp", ".*\\.txt", true);
    std::this_thread::sleep_for (std::chrono::milliseconds(1000));
    fclose(pPushDataFile.get());
//    t_stop = true;
//    thread.join();
    return 1;
}
int main1() {
    std::cout << "Hello, World!" << std::endl;
    char now[64];
    struct std::tm *ttime;
    time_t tt = 1652691410;
    ttime = localtime(&tt);
    std::strftime(now, 64, "%Y-%m-%d %H:%M:%S", ttime);
    std::stringstream ss;
    ss << now;//<< std::endl;
    std::cout << ss.str() + "." +std::to_string(1652691410983%1000) << std::endl;

//    ZlibHelper::RemoveDir("/Users/yuhao/temp2");
    ZlibHelper::CreateTarFile("/Users/yuhao/temp","/Users/yuhao/temp3.tar");
    return 0;
}
