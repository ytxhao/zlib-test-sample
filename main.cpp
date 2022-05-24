#include <iostream>//
#include <sstream>
#include "zlib.h"
// 1652691410983

void CreateZipFromDir(std::string dirName, std::string zipFileName) {

}

int main() {

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
