// Console.h —— 控制台辅助：统一中文输出环境，并处理“窗口一闪而过”的问题。

#pragma once

namespace console
{
    // 让 Windows 控制台按 UTF-8 显示中文（非 Windows 平台为空操作）。
    void setupUtf8();

    // 若标准输入来自控制台（双击 exe 运行），则等待用户按回车再退出；
    // 若输入被重定向（脚本、管道），立即返回，不会卡住自动化运行。
    void waitForEnterIfConsole();
}
