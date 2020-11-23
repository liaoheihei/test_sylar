#include "log.h"
#include <map>
#include <functional>
#include <iostream>
#include <time.h>
#include <string.h>

namespace sylar{
class MessageFormatItem : public LogFormatter::FormatItem {
   public:
    MessageFormatItem(const std::string& str = "") {}
    virtual void format(std::ostream& os, Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override {
        os << event->getContent();

    }
};

class LevelFormatItem : public LogFormatter::FormatItem {
   public:
    LevelFormatItem(const std::string& str = "") {}
    virtual void format(std::ostream& os, Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override {
        os << LogLevel::ToString(level);
    }
};

class ElapseFormatItem : public LogFormatter::FormatItem {
   public:
    ElapseFormatItem(const std::string str = "") {}
    virtual void format(std::ostream& os, Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override {
        os << event->getElapse();
    }
};

class NameFormatItem : public LogFormatter::FormatItem {
   public:
    NameFormatItem(const std::string str = "") {}
    virtual void format(std::ostream& os, Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override {
        os << logger->getName();
    }
};

class ThreadFormatItem : public LogFormatter::FormatItem {
   public:
    ThreadFormatItem(const std::string str = "") {}
    virtual void format(std::ostream& os, Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override {
        os << event->getThreadId();
    }
};

class FiberFormatItem : public LogFormatter::FormatItem {
   public:
    FiberFormatItem(const std::string str = "") {}
    virtual void format(std::ostream& os, Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override {
        os << event->getFiberId();
    }
};

class DateTimeFormatItem : public LogFormatter::FormatItem {
   public:
    DateTimeFormatItem(const std::string& format = "%Y-%m-%d %H：%M:%S")
        : m_format(format){
            if(m_format.empty()){
                m_format = "%Y-%m-%d %H:%M:%S";
            }
        }
    virtual void format(std::ostream& os, Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override {
        struct tm tm;
        time_t time = event->getTime();
        localtime_r(&time, &tm);
        char buf[64];
        strftime(buf, sizeof(buf), m_format.c_str(), &tm);

        os << buf;
    }

   private:
    // 时间格式
    std::string m_format;
};

class FilenameFormatItem : public LogFormatter::FormatItem {
   public:
    FilenameFormatItem(const std::string str = "") {}
    virtual void format(std::ostream& os, Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override {
        os << event->getFile();
    }
};

class LineFormatItem : public LogFormatter::FormatItem {
   public:
    LineFormatItem(const std::string str = "") {}
    virtual void format(std::ostream& os, Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override {
        os << event->getLine();
    }
};

class NewLineFormatItem : public LogFormatter::FormatItem {
   public:
    NewLineFormatItem(const std::string str = "") {}
    virtual void format(std::ostream& os, Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override {
        os << std::endl;
    }
};

class StringFormatItem : public LogFormatter::FormatItem {
   public:
    StringFormatItem(std::string str)
        : m_string(str){

        }
    virtual void format(std::ostream& os, Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override {
        os << m_string;
    }
    private:
        std::string m_string;
};

class TabFormatItem : public LogFormatter::FormatItem {
   public:
    TabFormatItem(const std::string str = "") {}
    virtual void format(std::ostream& os, Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override {
        os << "\t";
    }
    private:
        std::string m_string;
};

/////////////////////////////////////LogEvent/////////////////////////////////////
LogEvent::LogEvent(std::shared_ptr<Logger> logger, LogLevel::Level level, 
                    const char* file, int32_t line, uint32_t elapse, 
                    uint32_t threadID, uint32_t fiberID, uint64_t time)
            :m_file(file)
            ,m_line(line)
            ,m_elapse(elapse)
            ,m_threadID(threadID)
            ,m_fiberID(fiberID)
            ,m_time(time)
            ,m_logger(logger)
            ,m_level(level){

 }

 void LogEvent::format(const char* fmt, ...){
     va_list al;
     va_start(al, fmt);
     format(fmt, al);
     va_end(al);
 }

 void LogEvent::format(const char* fmt, va_list al){
     char* buf = nullptr;
     int len = vasprintf(&buf, fmt, al);
     if(len != -1) {
         m_ss << std::string(buf, len);
         free(buf);
     }
 }

 LogEvent::~LogEvent() {

 }

 /////////////////////////////////////LoggerManager/////////////////////////////////////
LoggerManager::LoggerManager() {
    m_root.reset(new Logger);
    // 添加默认的appender
    m_root->addAppender(LogAppender::ptr(new StdoutLogAppender));
    
}

void LoggerManager::init() {
    
}

Logger::ptr LoggerManager::getLogger(const std::string& name) {
    auto it = m_loggers.find(name);
    return it == m_loggers.end() ? m_root : it->second;
}


 /////////////////////////////////////LogEventWrap/////////////////////////////////////
LogEventWrap::LogEventWrap(LogEvent::ptr e)
    : m_event(e){

}

LogEventWrap::~LogEventWrap(){
    m_event->getLogger()->log(m_event->getLevel(), m_event);
}

std::stringstream& LogEventWrap::getSS(){
    return m_event->getSS();
}

/////////////////////////////////////LogLevel/////////////////////////////////////
const char* LogLevel::ToString(LogLevel::Level level){
    switch(level){
#define XX(name) \
        case LogLevel::name: \
            return #name; \
            break;
        
        XX(DEBUG);
        XX(INFO);
        XX(WARN);
        XX(ERROR);
        XX(FATAL);
#undef XX
        default:
            return "UNKNOW";
    }
    return "UNKONW";
}


/////////////////////////////////////Logger/////////////////////////////////////
    Logger::Logger(const std::string& name) 
        : m_name(name)
        ,m_level(LogLevel::DEBUG){
        // 设置默认的日志格式
        m_formatter.reset(new LogFormatter("%d%T%t%T%F%T[%p]%T[%c]%T%f:%l%T%m%n"));
    }

    void Logger::log(LogLevel::Level level, LogEvent::ptr event) {
        if (level >= m_level) {
            // 通过shared_from_this把自身的指针作为参数进行传递
            auto self = shared_from_this();
            for (auto& i : m_appenders) {
                i->log(self, level, event);
            }
        }
    }

    // 增加appender
    void Logger::addAppender(LogAppender::ptr appender){
        // 若记录日志时没有指定日志格式，使用默认的日志格式
        if(!appender->getFormmater()){
            appender->setFormatter(m_formatter);
        }
        m_appenders.push_back(appender);
    }
    // 删除appender
    void Logger::delAppender(LogAppender::ptr appender){
        for(auto it = m_appenders.begin(); it != m_appenders.end(); ++it){
            if(*it == appender){
                m_appenders.erase(it);
                break;
            }
        }
    }

    // 每一个级别对应单独的方法
    void Logger::debug(LogEvent::ptr event) {
        log(LogLevel::DEBUG, event);
    }

    void Logger::info(LogEvent::ptr event) {
        log(LogLevel::INFO, event);
    }

    void Logger::warn(LogEvent::ptr event){
        log(LogLevel::WARN, event);
    }

    void Logger::error(LogEvent::ptr event){
        log(LogLevel::ERROR, event);
    }

    void Logger::fatal(LogEvent::ptr event){
        log(LogLevel::FATAL, event);
    }

/////////////////////////////////////FileLogAppender/////////////////////////////////////
    FileLogAppender::FileLogAppender(const std::string& filename)
        : m_filename(filename) {
            reopen();
    }

    void FileLogAppender::log(std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) {
        if (level >= m_level) {
            m_filestream << m_formatter->format(logger, level, event);
        }
    }

    bool FileLogAppender::reopen() {
        if (m_filestream) {
            m_filestream.close();
        }
        m_filestream.open(m_filename);
        return !m_filestream;
    }

    void StdoutLogAppender::log(std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) {
        if (level >= m_level) {
            std::cout << m_formatter->format(logger, level, event);
        }
    }

/////////////////////////////////////LogFormatter/////////////////////////////////////
LogFormatter::LogFormatter(const std::string& pattern) : m_pattern(pattern){
    init();
}

std::string LogFormatter::format(std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event){
    std::stringstream ss;
    for(auto& i : m_items){
        i->format(ss, logger, level, event);
    }
    return ss.str();
}

void LogFormatter::init(){
    // str, format, type
    std::vector<std::tuple<std::string, std::string, int> > vec;
    std::string nstr;
    for(size_t i = 0; i < m_pattern.size(); ++i){
        if(m_pattern[i] != '%'){
            nstr.append(1, m_pattern[i]);
            continue;
        }

        // 将%%解析为%
        if((i + 1) < m_pattern.size()){
            if(m_pattern[i + 1] == '%'){
                nstr.append(1, '%');
                continue;
            }
        }


        size_t n = i + 1;
        int fmt_status = 0;
        size_t fmt_begin = 0;

        std::string str;
        std::string fmt;
        while(n < m_pattern.size()){
            if(!isalpha(m_pattern[n]) && m_pattern[n] != '{'
            && m_pattern[n] != '}'){
                // 如果是空格，说明字符串不连续，中断
                break;
            }
            if (fmt_status == 0){
                if (m_pattern[n] == '{'){
                    str = m_pattern.substr(i + 1, n - i - 1);
                    fmt_status = 1;     // 解析格式
                    ++n;
                    fmt_begin = n;
                    continue;
                }
            }
            if(fmt_status == 1){
                if(m_pattern[n] == '}'){
                    fmt = m_pattern.substr(fmt_begin + 1, n - fmt_begin - 1);
                    fmt_status = 0;
                    ++n;
                    break;
                }
            }
            ++n;
        }

        if(fmt_status == 0){
            if(!nstr.empty()){
                vec.push_back(std::make_tuple(nstr, std::string(), 0));
                nstr.clear();
            }
            str = m_pattern.substr(i + 1, n - i - 1);
            vec.push_back(std::make_tuple(str, fmt, 1));
            i = n - 1;
        }
        else if(fmt_status == 1){
            //std::cout << "pattern parse error" << m_pattern.substr(i) << std::endl;
            vec.push_back(std::make_tuple("<<pattern_error>>", fmt, 0));
        }
        else if(fmt_status == 2){
            if(!nstr.empty()){
                vec.push_back(std::make_tuple(nstr, std::string(), 0));
                nstr.clear();
            }
            vec.push_back(std::make_tuple(str, fmt, 1));
            i = n - 1;
        }
    }

    if (!nstr.empty()){
        vec.push_back(std::make_tuple(nstr, "", 0));
    }

    static std::map<std::string, std::function<FormatItem::ptr(const std::string& str)>> s_format_item = {
#define XX(str, CLASS) \
        {#str, [](const std::string& fmt) { return FormatItem::ptr(new CLASS(fmt)); }}

        XX(m, MessageFormatItem),
        XX(p, LevelFormatItem),
        XX(r, ElapseFormatItem),
        XX(c, NameFormatItem),
        XX(t, ThreadFormatItem),
        XX(F, FiberFormatItem),
        XX(n, NewLineFormatItem),
        XX(d, DateTimeFormatItem),
        XX(f, FilenameFormatItem),
        XX(l, LineFormatItem),
        XX(T, TabFormatItem)
#undef XX
    };

    for(auto& i : vec){
        if(std::get<2>(i) == 0){
            m_items.push_back(FormatItem::ptr(new StringFormatItem(std::get<0>(i))));
        }
        else{
            // 在映射中查找对应的格式字符
            auto it = s_format_item.find(std::get<0>(i));
            if(it == s_format_item.end()){
                m_items.push_back(FormatItem::ptr(new StringFormatItem("<<error_format %" + std::get<0>(i) + ">>")));
            }
            else{
                m_items.push_back(it->second(std::get<1>(i)));
            }
        }

        //std::cout << "(" << std::get<0>(i) << ") - (" << std::get<1>(i) << ") - (" << std::get<2>(i) << ")" << std::endl;
    }

    // %m -- 消息体
    // %p -- level优先级
    // %r -- 启动后的时间
    // %c -- 日志名称
    // %t -- 线程ID
    // %n -- 回车
    // %d -- 时间
    // %f -- 文件名
    // %l -- 行号

}




}