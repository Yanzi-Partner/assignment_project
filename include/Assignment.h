// Assignment.h —— 作业类（迭代版）：对象状态 + 类级静态统计 + 友元输出
//
// 本次迭代新增三件事（对应课后任务的三个技术点）：
//   1. 静态数据成员 + 静态成员函数：统计“存活 / 累计构造 / 拷贝 / 已析构”，
//      并在内部维护一份存活对象清单，供类级汇总使用；
//   2. 拷贝构造与拷贝赋值：明确“拷贝算新建对象，赋值不算”，保证计数不虚高；
//   3. 友元 operator<<：完整配置输出需要读内部的“已配置”标志，
//      用 friend 而不是为此再增加一批 public getter。

#pragma once

#include <cstdint>
#include <iosfwd>
#include <string>

class Assignment
{
public:
    // ---------------- 类内常量：占位值与业务边界 ----------------
    static constexpr const char* kPlaceholderTitle = "待设置名称";
    static constexpr std::uint64_t kPlaceholderMaxBytes = 1;
    static constexpr std::uint64_t kMaxAllowedBytes = 1024ULL * 1024 * 1024; // 1 GiB

    // ---------------- 构造与拷贝 ----------------
    Assignment(const std::string& title, std::uint64_t maxBytes);              // 报告要求缺省 false
    Assignment(const std::string& title, std::uint64_t maxBytes, bool requireReport);

    // 拷贝构造：产生一个新对象，因此要占一个新编号，并计入“累计构造 / 拷贝”
    Assignment(const Assignment& other);

    // 拷贝赋值：不产生新对象，存活数与编号都不变，只覆盖配置内容
    Assignment& operator=(const Assignment& other);

    // 析构：对象离开作用域时扣减存活数
    ~Assignment();

    // ---------------- 修改 ----------------
    [[nodiscard]] bool setTitle(const std::string& title);
    [[nodiscard]] bool setMaxBytes(std::uint64_t bytes);
    void setRequireReport(bool value);

    // ---------------- 查询（只暴露业务真正需要的部分）----------------
    [[nodiscard]] const std::string& getTitle() const noexcept;
    [[nodiscard]] std::uint64_t getMaxBytes() const noexcept;
    [[nodiscard]] bool getRequireReport() const noexcept;
    [[nodiscard]] int getSerial() const noexcept;
    [[nodiscard]] bool isReady() const noexcept;

    // ---------------- 静态成员函数：类级信息，不需要任何对象 ----------------
    [[nodiscard]] static int liveCount() noexcept;      // 当前存活对象数
    [[nodiscard]] static int createdCount() noexcept;   // 累计构造次数（含拷贝构造）
    [[nodiscard]] static int copiedCount() noexcept;    // 累计拷贝构造次数
    [[nodiscard]] static int destroyedCount() noexcept; // 累计析构次数
    [[nodiscard]] static std::uint64_t totalMaxBytes() noexcept; // 存活对象的上限之和
    static void printAll(std::ostream& os);             // 逐个输出存活对象的完整配置

    // ---------------- 友元：完整配置输出 ----------------
    friend std::ostream& operator<<(std::ostream& os, const Assignment& assignment);

private:
    // 私有构造函数：两个公开版本都委托给它，初始化与校验只写一份
    Assignment(const std::string& title, std::uint64_t maxBytes,
               bool requireReport, const char* tag);

    // 合法性判断与规范化（构造函数、setter、拷贝赋值共用）
    [[nodiscard]] static bool hasValidTitle(const std::string& title) noexcept;
    [[nodiscard]] static bool hasValidMaxBytes(std::uint64_t bytes) noexcept;
    [[nodiscard]] static std::string normalizeTitle(const std::string& title);
    [[nodiscard]] static std::uint64_t normalizeMaxBytes(std::uint64_t bytes) noexcept;

    // ---------------- 静态数据成员：整份程序只有一份 ----------------
    static int liveCount_;      // 当前存活对象数
    static int createdCount_;   // 累计构造次数（含拷贝构造）
    static int copiedCount_;    // 累计拷贝构造次数
    static int destroyedCount_; // 累计析构次数
    static int nextSerial_;     // 下一个可用编号

    // ---------------- 普通数据成员：每个对象各有一份 ----------------
    std::string title_;        // 作业名称
    std::uint64_t maxBytes_;   // 文件大小上限（字节）
    bool requireReport_;       // 是否需要实验报告
    bool titleConfigured_;     // 名称是否来自合法输入
    bool maxBytesConfigured_;  // 上限是否来自合法输入
    int serial_;               // 对象编号（由静态计数器分配）
};
