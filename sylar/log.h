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

namespace sylar {

// 日志事件
class LogEvent{
public:
    typedef std::shared_ptr<LogEvent> ptr;
    LogEvent();
private:
    const char* m_file = nullptr;       // 文件名
    int32_t m_line = 0;                 // 行号
    uint32_t m_elapse= 0;               // 程序启动开始到现在的毫秒数
    uint32_t m_threadID = 0;            // 线程ID
    uint32_t m_fiberID = 0;             // 协程ID
    uint64_t m_time;                    // 时间戳
    std::string m_content;              // 信号

};

class LogLevel{
public:
    // 日志级别
    enum Level{
        DEBUG = 1,
        INFO = 2,
        WARN = 3,
        ERROR = 4,
        FATAL = 5
    };
};

// 日志格式器
class LogFormatter{
public:
    typedef std::shared_ptr<LogFormatter> ptr;
    // 根据pattern的格式解析出Item内包含的信息
    LogFormatter(const std::string& pattern);

    std::string format(LogEvent::ptr event);
private:
    class FormatItem{
    public:
        typedef std::shared_ptr<FormatItem> ptr;
        virtual ~FormatItem() {}
        virtual void format(std::ostream& os, LogEvent::ptr event) = 0;
    };

    // init函数解析pattern
    void init();
private:
    std::string m_pattern;
    std::vector<FormatItem::ptr> m_items;
};

// 日志输出地
class LogAppender{
public:
    typedef std::shared_ptr<LogAppender> ptr;
    virtual ~LogAppender() {}

    virtual void log(LogLevel::Level level, LogEvent::ptr event) = 0;

    LogFormatter::ptr getFormmater() const {return m_formatter;}
    void setFormatter(LogFormatter::ptr val) {m_formatter = val;}
protected:
    LogLevel::Level m_level;

    // 不同的日志输出的格式可能会不同，需要定义各自的LogFormatter
    LogFormatter::ptr m_formatter;
};

// 日志器
class Logger{
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
    // 返回当前日志level
    LogLevel::Level getLevel() {return m_level;}
    // 设置当前日志level
    void setLevel(LogLevel::Level level) {m_level = level;}


private:
    std::string m_name;                                //日志名称
    LogLevel::Level m_level;                           // 日志级别
    std::list<LogAppender::ptr> m_appenders;           // Appender集合

};

// 输出到控制台的Appender
class StdoutLogAppender : public LogAppender{
public:
    typedef std::shared_ptr<StdoutLogAppender> ptr;
    void log(LogLevel::Level level, LogEvent::ptr event) override;
};

// 输出到文件的Appender
class FileLogAppender : public LogAppender{
public:
    typedef std::shared_ptr<FileLogAppender> ptr;
    FileLogAppender(const std::string& filename);
    void log(LogLevel::Level level, LogEvent::ptr event) override;

    // 涉及到文件打开操作，需要定义文件重新打开的操作,文件打开成功返回true
    bool reopen();
private:
    std::string m_filename;
    std::ofstream m_filestream;
};

}

#endif