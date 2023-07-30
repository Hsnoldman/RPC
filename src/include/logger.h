#pragma once
#include "lockqueue.h"
#include <string>

// 定义宏 LOG_INFO("xxx %d %s", 20, "xxxx")
#define LOG_INFO(logmsgformat, ...)                     \
    do                                                  \
    {                                                   \
        Logger &logger = Logger::GetInstance();         \
        logger.SetLogLevel(INFO);                       \
        char c[1024] = {0};                             \
        snprintf(c, 1024, logmsgformat, ##__VA_ARGS__); \
        logger.Log(c,0);                                  \
    } while (0)

// 每一行结尾处的"\"为继续符，可用于一行的结尾，表示本行与下一行连接起来

#define LOG_ERR(logmsgformat, ...)                      \
    do                                                  \
    {                                                   \
        Logger &logger = Logger::GetInstance();         \
        logger.SetLogLevel(ERROR);                      \
        char c[1024] = {0};                             \
        snprintf(c, 1024, logmsgformat, ##__VA_ARGS__); \
        logger.Log(c,1);                                  \
    } while (0)

// 定义日志级别
enum LogLevel
{
    INFO,  // 普通信息
    ERROR, // 错误信息
};

// Mprpc框架提供的日志系统
class Logger
{
public:
    // 获取日志的单例
    static Logger &GetInstance();
    // 设置日志级别
    void SetLogLevel(LogLevel level);
    // 写日志
    void Log(std::string msg, int level);

private:
    int m_loglevel = 1;              // 记录日志级别
    LockQueue<std::string> m_lckQue; // 日志缓冲队列

    Logger();
    Logger(const Logger &) = delete;
    Logger(Logger &&) = delete;
};