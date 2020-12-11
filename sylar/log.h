#ifndef __SYLAR_LOG_H_
#define __SYLAR_LOG_H_

#include <string>
#include <stdint.h>
#include <memory>
#include <list>
#include <sstream>
#include <fstream>
#include <iostream>
#include <vector>
#include <stdarg.h>
#include <map>

#include "singleton.h"
#include "util.h"

// 添加宏定义
#define SYLAR_LOG_LEVEL(logger, level) \
    if(logger->getLevel() <= level) \
        sylar::LogEventWrap(sylar::LogEvent::ptr(new sylar::LogEvent(logger, level, __FILE__, __LINE__, 0, sylar::GetThreadID(), \
         sylar::GetFiberID(), time(0)))).getSS()
        
#define SYLAR_LOG_DEBUG(logger) SYLAR_LOG_LEVEL(logger, sylar::LogLevel::DEBUG)
#define SYLAR_LOG_INFO(logger) SYLAR_LOG_LEVEL(logger, sylar::LogLevel::INFO)
#define SYLAR_LOG_WARN(logger) SYLAR_LOG_LEVEL(logger, sylar::LogLevel::WARN)
#define SYLAR_LOG_ERROR(logger) SYLAR_LOG_LEVEL(logger, sylar::LogLevel::ERROR)
#define SYLAR_LOG_FATAL(logger) SYLAR_LOG_LEVEL(logger, sylar::LogLevel::FATAL)

#define SYLAR_LOG_FMT_LEVEL(logger, level, fmt, ...) \
    if(logger->getLevel() <= level) \
        sylar::LogEventWrap(sylar::LogEvent::ptr(new sylar::LogEvent(logger, level, \
        __FILE__, __LINE__, 0, sylar::GetThreadID(), sylar::GetFiberID(), \
        time(0)))).getEvent()->format(fmt, __VA_ARGS__)

#define SYLAR_LOG_FMT_DEBUG(logger, fmt, ...) SYLAR_LOG_FMT_LEVEL(logger, sylar::LogLevel::DEBUG, fmt, __VA_ARGS__)
#define SYLAR_LOG_FMT_INFO(logger, fmt, ...) SYLAR_LOG_FMT_LEVEL(logger, sylar::LogLevel::INFO, fmt, __VA_ARGS__)
#define SYLAR_LOG_FMT_WARN(logger, fmt, ...) SYLAR_LOG_FMT_LEVEL(logger, sylar::LogLevel::WARN, fmt, __VA_ARGS__)
#define SYLAR_LOG_FMT_ERROR(logger, fmt, ...) SYLAR_LOG_FMT_LEVEL(logger, sylar::LogLevel::ERROR, fmt, __VA_ARGS__)
#define SYLAR_LOG_FMT_FATAL(logger, fmt, ...) SYLAR_LOG_FMT_LEVEL(logger, sylar::LogLevel::FATAL, fmt, __VA_ARGS__)

#define SYLAR_LOG_ROOT() sylar::LoggerMgr::GetInstance()->getRoot()

// 查找对应name的logger
#define SYLAR_LOG_NAME(name) sylar::LoggerMgr::GetInstance()->getLogger(name)

namespace sylar {
class Logger;
class LoggerManager;

class LogLevel{
public:
    // 日志级别
    enum Level{
        UNKNOW = 0,
        DEBUG = 1,
        INFO = 2,
        WARN = 3,
        ERROR = 4,
        FATAL = 5
    };

    static const char* ToString(LogLevel::Level level);
    static LogLevel::Level FromString(const std::string& str);
};

// 日志事件
/*
LogEvent记录当前日志事件，包括与日志相关的系统信息，日志等级。
并将当前日志事件与具体的logger对象关联起来。
*/
class LogEvent{
public:
    typedef std::shared_ptr<LogEvent> ptr;
    LogEvent();
    LogEvent(std::shared_ptr<Logger> logger, LogLevel::Level level, const char* file, int32_t line, uint32_t elapse, uint32_t threadID, 
            uint32_t fiberID, uint64_t time);
    ~LogEvent();

    // 获得属性方法
    const char* getFile() { return m_file; }
    int32_t getLine() { return m_line; }
    uint32_t getElapse() { return m_elapse; }
    uint32_t getThreadId() { return m_threadID; }
    uint32_t getFiberId() { return m_fiberID; }
    uint64_t getTime() { return m_time; }
    std::string getContent() const { return m_ss.str(); }
    std::shared_ptr<Logger> getLogger() const { return m_logger; }
    LogLevel::Level getLevel() const { return m_level; }
    std::stringstream& getSS()  { return m_ss; }

    // 提供formatter方法
    void format(const char* fmt, ...);
    void format(const char* fmt, va_list al);
private:
    const char* m_file = nullptr;       // 文件名
    int32_t m_line = 0;                 // 行号
    uint32_t m_elapse= 0;               // 程序启动开始到现在的毫秒数
    uint32_t m_threadID = 0;            // 线程ID
    uint32_t m_fiberID = 0;             // 协程ID
    uint64_t m_time;                    // 时间戳
    std::stringstream m_ss;             // 信号

    std::shared_ptr<Logger> m_logger;   // 当前logEvent对应的logger
    LogLevel::Level m_level;            // 当前logEvent对应的日志级别
};

// 用于封装LogEvent，在析构时把LogEvent写入logger里面
class LogEventWrap{
public:
    LogEventWrap(LogEvent::ptr e);
    ~LogEventWrap();
    std::stringstream& getSS();

    LogEvent::ptr getEvent() const { return m_event; }
private:
    LogEvent::ptr m_event;
};



// 日志格式器
/*
日志格式解析器，根据输入的pattern转换为要求的输出信息
*/
class LogFormatter{
public:
    typedef std::shared_ptr<LogFormatter> ptr;
    // 根据pattern的格式解析出Item内包含的信息
    LogFormatter(const std::string& pattern);

    std::string format(std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event);
public:
    class FormatItem{
    public:
        typedef std::shared_ptr<FormatItem> ptr;
        FormatItem(const std::string& fmt = "") {};
        virtual ~FormatItem() {}
        virtual void format(std::ostream& os, std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) = 0;
    };

    // init函数解析pattern
    void init();

    bool isError() const { return m_error; }
    const std::string getPattern() const { return m_pattern; }
private:
    std::string m_pattern;
    std::vector<FormatItem::ptr> m_items;
    bool m_error = false;   // 判断格式是否有错误
};

// 日志输出地
/*
虚基类，用于定义日志的输出目的地的类，并管理不同输出目的地的日志格式以及日志等级
*/
class LogAppender{
friend class Logger;
public:
    typedef std::shared_ptr<LogAppender> ptr;
    virtual ~LogAppender() {}

    virtual void log(std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) = 0;
    virtual std::string toYamlString() = 0;

    LogFormatter::ptr getFormmater() const {return m_formatter;}
    void setFormatter(LogFormatter::ptr val);
    LogLevel::Level getLevel() { return m_level; }
    void setLevel(LogLevel::Level level) { m_level = level; }
protected:
    LogLevel::Level m_level = LogLevel::DEBUG;

    // 不同的日志输出的格式可能会不同，需要定义各自的LogFormatter
    LogFormatter::ptr m_formatter;

    // 判断当前appender是否有属于自己的formatter
    bool m_hasFormatter = false;
};

// 日志器
/*
Logger类记录当前日志的等级level与日志事件event,并管理日志的输出器与日志格式
*/
class Logger : public std::enable_shared_from_this<Logger> {
friend class LoggerManager;
public:
    typedef std::shared_ptr<Logger> ptr;

    Logger(const std::string& name = "root");
    void log(LogLevel::Level level, LogEvent::ptr event);

    // 每一个级别对应单独的方法
    void debug(LogEvent::ptr event);
    void info(LogEvent::ptr event);
    void warn(LogEvent::ptr event);
    void error(LogEvent::ptr event); 
    void fatal(LogEvent::ptr event);

    // 增加appender
    void addAppender(LogAppender::ptr appender);
    // 删除appender
    void delAppender(LogAppender::ptr appender);
    // 清空appender
    void clearAppender();
    // 返回当前日志level
    LogLevel::Level getLevel() {return m_level;}
    // 设置当前日志level
    void setLevel(LogLevel::Level level) { m_level = level; }
    // 获得日志名称
    const std::string getName() const { return m_name; }
    // 设置当前格式
    void setFormatter(LogFormatter::ptr val);
    void setFormatter(const std::string& val);
    // 获得当前格式
    LogFormatter::ptr getFormatter();

    // 将logger内容转为Yaml字符串
    std::string toYamlString();
private:
    std::string m_name;                                //日志名称
    LogLevel::Level m_level;                           // 日志级别
    std::list<LogAppender::ptr> m_appenders;           // Appender集合
    LogFormatter::ptr m_formatter;
    Logger::ptr m_root;

};

// 输出到控制台的Appender
class StdoutLogAppender : public LogAppender{
public:
    typedef std::shared_ptr<StdoutLogAppender> ptr;
    void log(std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) override;
    std::string toYamlString() override;
};

// 输出到文件的Appender
class FileLogAppender : public LogAppender{
public:
    typedef std::shared_ptr<FileLogAppender> ptr;
    FileLogAppender(const std::string& filename);
    void log(std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) override;

    // 涉及到文件打开操作，需要定义文件重新打开的操作,文件打开成功返回true
    bool reopen();

    std::string toYamlString() override;
private:
    std::string m_filename;
    std::ofstream m_filestream;
};

// 日志管理器
class LoggerManager {
public:
    LoggerManager();
    Logger::ptr getLogger(const std::string& name);

    // 初始化Logger
    void init();

    // 获得缺省的logger
    Logger::ptr getRoot() const { return m_root; }

    // 统一将所有logger序列化为YAML
    std::string toYamlString();
private:
    std::map<std::string, Logger::ptr> m_loggers;

    // 缺省的logger
    Logger::ptr m_root;
};

// 使用单例保证全局只有一个logger管理器
typedef sylar::Singleton<LoggerManager> LoggerMgr;

};

#endif