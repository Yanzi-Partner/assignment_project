// main.cpp —— 课后项目任务演示：状态独立 / 静态计数 / 完整配置输出 / 自检
//
// 运行顺序就是任务的四条要求：
//   0. 还没有对象时，静态成员函数也能调用（类级信息不依赖实例）
//   1. 创建多个对象，验证状态独立
//   2. 拷贝构造与拷贝赋值，观察计数如何变化
//   3. 完整配置输出（友元 operator<< 与静态 printAll）
//   4. 作用域结束析构，核对计数平衡
//
// 末尾用 check() 做自检：全部通过时返回 0，否则返回 1，便于脚本化验证。

#include <iostream>
#include <string>

#include "Assignment.h"
#include "Console.h"

namespace
{
    int g_failures = 0; // 失败的检查项数量

    void section(const char* text)
    {
        std::cout << "\n=== " << text << " ===\n";
    }

    // 轻量自检：把预期写成可执行的断言，评审时一眼能看出哪条不成立
    void check(bool condition, const std::string& what)
    {
        std::cout << (condition ? "  [通过] " : "  [失败] ") << what << '\n';
        if (!condition)
        {
            ++g_failures;
        }
    }
}

int main()
{
    console::setupUtf8();
    std::cout << std::boolalpha;

    section("0. 尚未创建对象：静态成员函数照样可以调用");
    std::cout << "存活=" << Assignment::liveCount()
              << "，累计构造=" << Assignment::createdCount()
              << "，拷贝=" << Assignment::copiedCount()
              << "，已析构=" << Assignment::destroyedCount() << '\n';

    {
        section("1. 创建多个对象，验证状态独立");
        Assignment lab1("实验一", 1024, true); // 三参数构造
        Assignment lab2("实验二", 2048);       // 两参数构造
        Assignment lab3("", 0, true);          // 非法参数：占位值 + 未就绪

        Assignment::printAll(std::cout);

        // 只改 lab2 与 lab3：lab1 的数据成员不应该被带动
        const bool lab3ReadyBefore = lab3.isReady();
        const bool lab2Changed = lab2.setMaxBytes(4096);
        const bool lab3Renamed = lab3.setTitle("补交作业");
        const bool lab3Filled = lab3.setMaxBytes(512);

        std::cout << "\n修改 lab2、lab3 之后：\n";
        std::cout << lab1 << lab2 << lab3;

        check(lab2Changed && lab3Renamed && lab3Filled, "三处修改都被接受");
        check(lab1.getMaxBytes() == 1024ULL, "改 lab2 不影响 lab1 的上限");
        check(lab1.getSerial() != lab2.getSerial(), "不同对象的编号互不相同");
        check(!lab3ReadyBefore, "非法构造参数只得到占位值，创建后暂时不可用");
        check(lab3.isReady(), "补全名称和上限之后变为就绪");
        check(Assignment::liveCount() == 3, "当前存活对象数是 3");

        section("2. 拷贝构造：产生新对象，计数要增加");
        Assignment lab4 = lab1; // 拷贝构造
        check(Assignment::liveCount() == 4, "拷贝构造后存活数 +1");
        check(Assignment::copiedCount() == 1, "拷贝计数为 1");
        check(lab4.getSerial() != lab1.getSerial(), "拷贝对象拿到新编号");
        check(lab4.getMaxBytes() == lab1.getMaxBytes(), "拷贝对象内容与原对象一致");

        const bool lab4Changed = lab4.setMaxBytes(8192);
        check(lab4Changed, "拷贝对象可以独立修改");
        check(lab1.getMaxBytes() == 1024ULL, "改拷贝对象不影响原对象（各有一份数据）");

        section("3. 拷贝赋值：不产生新对象，计数不变");
        const int liveBeforeAssign = Assignment::liveCount();
        const int createdBeforeAssign = Assignment::createdCount();
        const int serialOfLab2 = lab2.getSerial();

        lab2 = lab1; // 拷贝赋值

        check(Assignment::liveCount() == liveBeforeAssign, "赋值后存活数不变");
        check(Assignment::createdCount() == createdBeforeAssign, "赋值后累计构造数不变");
        check(lab2.getSerial() == serialOfLab2, "赋值后编号不变（还是同一个对象）");
        check(lab2.getMaxBytes() == lab1.getMaxBytes(), "赋值后内容被覆盖为 lab1 的");

        section("4. 类级汇总与完整配置输出");
        std::cout << "存活=" << Assignment::liveCount()
                  << "，累计构造=" << Assignment::createdCount()
                  << "，其中拷贝=" << Assignment::copiedCount()
                  << "，已析构=" << Assignment::destroyedCount() << '\n';
        std::cout << "存活对象的上限之和=" << Assignment::totalMaxBytes() << " 字节\n";
        Assignment::printAll(std::cout);

        check(Assignment::createdCount() == Assignment::liveCount() + Assignment::destroyedCount(),
              "累计构造 = 存活 + 已析构（计数平衡）");
        check(Assignment::totalMaxBytes() == 1024ULL + 1024ULL + 512ULL + 8192ULL,
              "总上限等于各存活对象上限之和");

        section("5. 离开作用域：按创建顺序的逆序析构");
    } // lab4 → lab3 → lab2 → lab1 依次析构

    section("6. 全部析构后的最终统计");
    std::cout << "存活=" << Assignment::liveCount()
              << "，累计构造=" << Assignment::createdCount()
              << "，拷贝=" << Assignment::copiedCount()
              << "，已析构=" << Assignment::destroyedCount() << '\n';
    std::cout << "存活对象的上限之和=" << Assignment::totalMaxBytes() << " 字节\n";

    check(Assignment::liveCount() == 0, "存活数归零");
    check(Assignment::createdCount() == Assignment::destroyedCount(), "构造次数等于析构次数");
    check(Assignment::totalMaxBytes() == 0ULL, "没有存活对象时总上限为 0");

    std::cout << (g_failures == 0 ? "\n全部检查通过。\n"
                                  : "\n有检查未通过，请看上面的 [失败] 行。\n");

    console::waitForEnterIfConsole();
    return g_failures == 0 ? 0 : 1;
}
