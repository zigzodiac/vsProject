// GetVideoPreviewPic.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include <direct.h>
#include <io.h>
#include <string>
#include "ExtractVideo.h"


int main(int arg, char** argv)
{
    int ret;
    
    for (int i = 0; i < arg; i++) {
        av_log(NULL, AV_LOG_ERROR, "input param %d %s\n", i, argv[i]);
    }
    if (arg != 4) {
        av_log(NULL, AV_LOG_ERROR, "Incorrect number of parameters\n");
        std::cout << "example: GetVideoPreviewPic.exe D:\XunLeiDownload\fi9eP2FUaaYA.mp4  10 D:\XunLeiDownload\\" << std::endl;
        return -100;
    }

    if (ret = OpenVideo(argv[1]) < 0) {
        return ret;
    }

    std::string inFilename = std::string(argv[1]);
    int DiagonalBarPos = inFilename.find_last_of('/');
    int AntiSlantBarPos = inFilename.find_last_of('\\');
    int pos = DiagonalBarPos >= AntiSlantBarPos ? DiagonalBarPos : AntiSlantBarPos;
    std::string fn = inFilename.substr(pos + 1);

    pos = fn.find('.');
    std::string sDir = std::string(argv[3]) +fn.substr(0, pos);

    if (_access(sDir.c_str(), 0) == -1) // if the directory doesnot exist.
    {
        ret = _mkdir(sDir.c_str());    //create
    }
    if (ret < 0) {
        av_log(NULL, AV_LOG_ERROR, "<output dir> %s cerate failed\n", sDir.c_str());
    }
    char* dir = const_cast<char*>(sDir.c_str());
    if (ret = SetDir(&dir) < 0)
        return ret;
    if (ret = GetVideoPic() < 0)
        return ret;
    return 0;
}

// 运行程序: Ctrl + F5 或调试 >“开始执行(不调试)”菜单
// 调试程序: F5 或调试 >“开始调试”菜单

// 入门使用技巧: 
//   1. 使用解决方案资源管理器窗口添加/管理文件
//   2. 使用团队资源管理器窗口连接到源代码管理
//   3. 使用输出窗口查看生成输出和其他消息
//   4. 使用错误列表窗口查看错误
//   5. 转到“项目”>“添加新项”以创建新的代码文件，或转到“项目”>“添加现有项”以将现有代码文件添加到项目
//   6. 将来，若要再次打开此项目，请转到“文件”>“打开”>“项目”并选择 .sln 文件
