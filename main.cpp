#include <iostream>//
#include <sstream>
#include <vector>
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
//    ZlibHelper::CreateZipFromDir("/Users/yuhaoo/workspace/tmp","/Users/yuhaoo/workspace/tmp.zip");
//    ZlibHelper::CreateZipFromDir("/Users/yuhaoo/workspace/libzegoliveroom.so","/Users/yuhaoo/workspace/tmp2.zip");
    ZlibHelper::UnzipFile("/Users/yuhaoo/workspace/tmp.zip", "/Users/yuhaoo/workspace/tmp_unzip");
    return 1;
}
int main1() {
    std::cout << "qHello, World!" << std::endl;
    char now[64];
    struct std::tm *ttime;
    time_t tt = 1652691410;
    ttime = localtime(&tt);
    std::strftime(now, 64, "%Y-%m-%d %H:%M:%S", ttime);
    std::stringstream ss;
    ss << now;//<< std::endl;


    std::cout << ss.str() + "." +std::to_string(1652691410983%1000) << std::endl;
    return 0;
}
