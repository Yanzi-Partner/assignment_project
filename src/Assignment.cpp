// Assignment.cpp —— 作业类（迭代版）的实现

#include "Assignment.h"

#include <algorithm>
#include <iostream>
#include <ostream>
#include <string>
#include <vector>

// ==================== 静态数据成员的定义 ====================
// 静态成员在类内只是“声明”，必须在某个 .cpp 里给出唯一定义，否则链接报错。
int Assignment::liveCount_ = 0;
int Assignment::createdCount_ = 0;
int Assignment::copiedCount_ = 0;
int Assignment::destroyedCount_ = 0;
int Assignment::nextSerial_ = 1;

namespace
{
    // 存活对象清单：类级汇总（总上限、逐个输出）都靠它。
    // 这里用“函数内 static”而不是全局变量，既避免跨编译单元的初始化顺序问题，
    // 也保证第一次使用时才构造。
    std::vector<const Assignment*>& liveObjects()
    {
        static std::vector<const Assignment*> objects;
        return objects;
    }

    void registerObject(const Assignment* object)
    {
        liveObjects().push_back(object);
    }

    void unregisterObject(const Assignment* object)
    {
        std::vector<const Assignment*>& objects = liveObjects();
        const auto it = std::find(objects.begin(), objects.end(), object);
        if (it != objects.end())
        {
            objects.erase(it);
        }
    }
}

// ==================== 构造 ====================

Assignment::Assignment(const std::string& title, std::uint64_t maxBytes)
    : Assignment(title, maxBytes, false, "两参数")
{
}

Assignment::Assignment(const std::string& title, std::uint64_t maxBytes, bool requireReport)
    : Assignment(title, maxBytes, requireReport, "三参数")
{
}

// 私有构造函数：真正干活的版本，三个公开构造函数共用同一套初始化与计数
Assignment::Assignment(const std::string& title, std::uint64_t maxBytes,
                       bool requireReport, const char* tag)
    : title_(normalizeTitle(title)),
      maxBytes_(normalizeMaxBytes(maxBytes)),
      requireReport_(requireReport),
      titleConfigured_(hasValidTitle(title)),
      maxBytesConfigured_(hasValidMaxBytes(maxBytes)),
      serial_(nextSerial_++) // 编号由静态计数器统一分配，每个对象唯一
{
    ++createdCount_; // 累计构造（含拷贝）
    ++liveCount_;    // 存活数 +1
    registerObject(this);

    std::cout << "[构造·" << tag << "] #" << serial_ << " " << title_ << '\n';
}

// 拷贝构造：源对象的 5 个数据成员被复制，编号重新分配
Assignment::Assignment(const Assignment& other)
    : title_(other.title_),
      maxBytes_(other.maxBytes_),
      requireReport_(other.requireReport_),
      titleConfigured_(other.titleConfigured_),
      maxBytesConfigured_(other.maxBytesConfigured_),
      serial_(nextSerial_++)
{
    ++createdCount_;
    ++copiedCount_; // 这是“拷贝出来的对象”，单独计数
    ++liveCount_;
    registerObject(this);

    std::cout << "[拷贝构造] #" << serial_ << " ← #" << other.serial_ << " " << title_ << '\n';
}

// ==================== 拷贝赋值 ====================

Assignment& Assignment::operator=(const Assignment& other)
{
    if (this == &other)
    {
        return *this; // 自赋值：直接返回，避免自己覆盖自己
    }

    // 只复制配置内容，不动编号，也不动任何计数器：
    // 赋值不产生新对象，存活数、累计构造数都不应该变化。
    title_ = other.title_;
    maxBytes_ = other.maxBytes_;
    requireReport_ = other.requireReport_;
    titleConfigured_ = other.titleConfigured_;
    maxBytesConfigured_ = other.maxBytesConfigured_;

    std::cout << "[拷贝赋值] #" << serial_ << " ← #" << other.serial_ << " " << title_ << '\n';
    return *this;
}

// ==================== 析构 ====================

Assignment::~Assignment()
{
    --liveCount_;
    ++destroyedCount_;
    unregisterObject(this); // 必须先从清单里摘掉，否则类级汇总会访问已销毁的对象

    std::cout << "[析构] #" << serial_ << " " << title_
              << "（剩余存活 " << liveCount_ << "）\n";
}

// ==================== 修改 ====================

bool Assignment::setTitle(const std::string& title)
{
    if (!hasValidTitle(title))
    {
        return false; // 非法修改：原值与标志都不变
    }
    title_ = title;
    titleConfigured_ = true;
    return true;
}

bool Assignment::setMaxBytes(std::uint64_t bytes)
{
    if (!hasValidMaxBytes(bytes))
    {
        return false;
    }
    maxBytes_ = bytes;
    maxBytesConfigured_ = true;
    return true;
}

void Assignment::setRequireReport(bool value)
{
    requireReport_ = value;
}

// ==================== 查询 ====================

const std::string& Assignment::getTitle() const noexcept
{
    return title_;
}

std::uint64_t Assignment::getMaxBytes() const noexcept
{
    return maxBytes_;
}

bool Assignment::getRequireReport() const noexcept
{
    return requireReport_;
}

int Assignment::getSerial() const noexcept
{
    return serial_;
}

bool Assignment::isReady() const noexcept
{
    // 名称与上限都来自合法输入，才算“业务配置合格”
    return titleConfigured_ && maxBytesConfigured_;
}

// ==================== 静态成员函数 ====================

int Assignment::liveCount() noexcept
{
    return liveCount_;
}

int Assignment::createdCount() noexcept
{
    return createdCount_;
}

int Assignment::copiedCount() noexcept
{
    return copiedCount_;
}

int Assignment::destroyedCount() noexcept
{
    return destroyedCount_;
}

std::uint64_t Assignment::totalMaxBytes() noexcept
{
    std::uint64_t total = 0;
    for (const Assignment* object : liveObjects())
    {
        if (object != nullptr)
        {
            // 静态成员函数没有 this，但同样能访问同类对象的私有成员
            total += object->maxBytes_;
        }
    }
    return total;
}

void Assignment::printAll(std::ostream& os)
{
    os << "存活对象共 " << liveCount_ << " 个：\n";
    for (const Assignment* object : liveObjects())
    {
        if (object != nullptr)
        {
            os << *object; // 复用下面的友元输出，格式只有一处定义
        }
    }
}

// ==================== 友元输出 ====================

// 友元函数：能读私有成员（包括内部的“已配置”标志），一次输出完整配置。
// 如果不用 friend，就得为 titleConfigured_ / maxBytesConfigured_ 再加两个 public getter，
// 等于为了打印把内部实现细节暴露给所有调用方——取舍说明见 README。
std::ostream& operator<<(std::ostream& os, const Assignment& assignment)
{
    os << "  #" << assignment.serial_
       << "「" << assignment.title_ << "」"
       << " 上限=" << assignment.maxBytes_ << " 字节"
       << " 需报告=" << (assignment.requireReport_ ? "true" : "false")
       << " 名称已配置=" << (assignment.titleConfigured_ ? "true" : "false")
       << " 上限已配置=" << (assignment.maxBytesConfigured_ ? "true" : "false")
       << " 就绪=" << (assignment.isReady() ? "true" : "false")
       << '\n';
    return os;
}

// ==================== 校验与规范化 ====================

bool Assignment::hasValidTitle(const std::string& title) noexcept
{
    // 名称不能为空，也不能只由空白字符组成
    return title.find_first_not_of(" \t\r\n") != std::string::npos;
}

bool Assignment::hasValidMaxBytes(std::uint64_t bytes) noexcept
{
    // 0 字节没有意义，超过 1 GiB 也不符合课程要求
    return bytes > 0 && bytes <= kMaxAllowedBytes;
}

std::string Assignment::normalizeTitle(const std::string& title)
{
    return hasValidTitle(title) ? title : std::string(kPlaceholderTitle);
}

std::uint64_t Assignment::normalizeMaxBytes(std::uint64_t bytes) noexcept
{
    return hasValidMaxBytes(bytes) ? bytes : kPlaceholderMaxBytes;
}
