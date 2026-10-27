// Console.cpp —— 控制台辅助函数的实现

#include "Console.h"

#include <iostream>
#include <string>

#ifdef _WIN32
#include <windows.h>
#endif

namespace console
{
    void setupUtf8()
    {
#ifdef _WIN32
        ::SetConsoleOutputCP(CP_UTF8); // 源码与字符串都是 UTF-8，控制台也切到 UTF-8
#endif
    }

    void waitForEnterIfConsole()
    {
#ifdef _WIN32
        // 输入句柄不是控制台（例如被重定向）时 GetConsoleMode 会失败，此时直接返回
        DWORD mode = 0;
        if (!::GetConsoleMode(::GetStdHandle(STD_INPUT_HANDLE), &mode))
        {
            return;
        }
#endif
        std::cout << "\n按回车键退出程序..." << std::flush;
        std::string line;
        std::getline(std::cin, line);
    }
}
