#include "log.h"
#include "config.h"

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
        os << event->getLogger()->getName();
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

class ThreadNameFormatItem : public LogFormatter::FormatItem {
   public:
    ThreadNameFormatItem(const std::string str = "") {}
    virtual void format(std::ostream& os, Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override {
        os << event->getThreadName();
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
                    uint32_t threadID, uint32_t fiberID, uint64_t time
                    , const std::string& thread_name)
            :m_file(file)
            ,m_line(line)
            ,m_elapse(elapse)
            ,m_threadID(threadID)
            ,m_fiberID(fiberID)
            ,m_time(time)
            ,m_logger(logger)
            ,m_level(level)
            ,m_threadName(thread_name){

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

    // 将初始化好的m_root放入m_loggers中
    m_loggers[m_root->m_name] = m_root;
    
    init();
}

std::string LoggerManager::toYamlString() {
    Mutextype::Lock lock(m_mutex);
    YAML::Node node;
    for(auto& i : m_loggers) {
        node.push_back(YAML::Load(i.second->toYamlString()));
    }

    std::stringstream ss;
    ss << node;
    return ss.str();
}

Logger::ptr LoggerManager::getLogger(const std::string& name) {
    Mutextype::Lock lock(m_mutex);
    auto it = m_loggers.find(name);
    if(it != m_loggers.end()) {
        return it->second;
    }
    
    Logger::ptr logger(new Logger(name));
    logger->m_root = m_root;
    m_loggers[name] = logger;
    return logger;
}

/*
2020/12/04
定义logDefine LogAppenderDefine, 偏特化LexicalCast,
实现日志配置解析
*/

struct LogAppenderDefine {
    int type = 0;   // 1: File, 2:Stdout
    LogLevel::Level level = LogLevel::UNKNOW;
    std::string formatter;
    std::string file;

    bool operator==(const LogAppenderDefine& oth) const {
        return type == oth.type && level == oth.level && 
        formatter == oth.formatter && file == oth.file;
    }
};

struct LogDefine {
    std::string name;
    LogLevel::Level level = LogLevel::UNKNOW;
    std::string formatter;

    std::vector<LogAppenderDefine> appenders;

    bool operator==(const LogDefine& oth) const {
        return name == oth.name && level == oth.level && 
        formatter == oth.formatter && appenders == oth.appenders;
    }

    bool operator<(const LogDefine& oth) const {
        return name < oth.name;
    }
};

// 对LogDefine类型偏特化
template<>
class LexicalCast <std::string, std::set<LogDefine> > {
public:
    std::set<LogDefine> operator() (const std::string& v) {
        YAML::Node node = YAML::Load(v);
        std::set<LogDefine> m_set;
        for(size_t i = 0; i < node.size(); ++i) {
            auto n = node[i];
            if(!n["name"].IsDefined()) {
                std::cout << "log config error: name is null, " << n << std::endl;
                continue;
            }
            LogDefine ld;
            ld.name = n["name"].as<std::string>();
            ld.level = LogLevel::FromString(n["level"].IsDefined() ? n["level"].as<std::string>() : "");
            if(n["formatter"].IsDefined()) {
                ld.formatter = n["formatter"].as<std::string>();
            }
            if(n["appenders"].IsDefined()) {
                for(size_t x = 0; x < n["appenders"].size(); ++x) {
                    auto a = n["appenders"][x];
                    if(!a["type"].IsDefined()) {
                        std::cout << "log config error: appender type is null, " << a << std::endl;
                        continue;
                    }
                    std::string type = a["type"].as<std::string>();
                    LogAppenderDefine lad;
                    if(type == "FileLogAppender") {
                        lad.type = 1;

                        if(!a["file"].IsDefined()) {
                            std::cout << "log config error: fileappender file is null, " << a << std::endl;
                            continue;
                        }
                        lad.file = a["file"].as<std::string>();

                        if(a["formatter"].IsDefined()) {
                            lad.formatter = a["formatter"].as<std::string>();
                        }

                    } else if(type == "StdoutLogAppender"){
                        lad.type = 2;

                        if(a["formatter"].IsDefined()) {
                            lad.formatter = a["formatter"].as<std::string>();
                        }
                    } else {
                        std::cout << "log config error: appender type is invalid, " << a << std::endl;
                        continue;
                    }

                    ld.appenders.push_back(lad);
                }
            }
            m_set.insert(ld);
        }
        return m_set;
    }
};

template <>
class LexicalCast<std::set<LogDefine>, std::string> {
   public:
    std::string operator()(const std::set<LogDefine>& v) {
        YAML::Node node;
        for(auto& i : v) {
            YAML::Node n;
            n["name"] = i.name;
            if(i.level != LogLevel::UNKNOW) {
                n["level"] = LogLevel::ToString(i.level);
            }
            if(!i.formatter.empty()) {
                n["formatter"] = i.formatter;
            }

            for(auto& a : i.appenders) {
                YAML::Node na;
                if(a.type == 1) {
                    na["type"] = "FileLogAppender";
                    na["file"] = a.file;
                } else if(a.type == 2) {
                    na["type"] = "StdoutLogAppender";
                }
                if (a.level != LogLevel::UNKNOW) {
                    na["level"] = LogLevel::ToString(a.level);
                }

                if(!a.formatter.empty()) {
                    na["formatter"] = a.formatter;
                }

                n["appenders"].push_back(na);
            }
            node.push_back(n);
        }
        std::stringstream ss;
        ss << node;
        return ss.str();

    }
};

// 引入ConfigVar
sylar::ConfigVar<std::set<LogDefine> >::ptr g_log_defines = 
    sylar::Config::Lookup("logs", std::set<LogDefine>(), "logs config");

// 定义日志更改事件,
struct LogIniter {
    // 当事件进来时，触发该函数
    LogIniter() {
        g_log_defines->addListener([](const std::set<LogDefine>& old_value, 
            const std::set<LogDefine>& new_value) {
            SYLAR_LOG_INFO(SYLAR_LOG_ROOT()) << "on_logger_conf_changed";
            // 新增日志
            for(auto& i : new_value) {
                auto it = old_value.find(i);
                sylar::Logger::ptr logger;
                if(it == old_value.end()) {
                    // 新增logger
                    // logger.reset(new sylar::Logger(i.name));
                    logger = SYLAR_LOG_NAME(i.name); 
                } else {
                    if (!(i == *it)) {
                        // 修改的logger
                        logger = SYLAR_LOG_NAME(i.name);
                    }
                }
                logger->setLevel(i.level);
                if (!i.formatter.empty()) {
                    logger->setFormatter(i.formatter);
                }

                logger->clearAppender();
                for (auto& a : i.appenders) {
                    sylar::LogAppender::ptr ap;
                    if (a.type == 1) {
                        ap.reset(new FileLogAppender(a.file));
                    } else if (a.type == 2) {
                        ap.reset(new StdoutLogAppender);
                    }
                    ap->setLevel(a.level);
                    if(!a.formatter.empty()) {
                        LogFormatter::ptr fmt(new LogFormatter(a.formatter));
                        if(!fmt->isError()) {
                            ap->setFormatter(fmt);
                        } else {
                            std::cout << "log.name" <<i.name << "appender type=" << a.type 
                                      << " formatter=" << a.formatter << " is invalid" << std::endl;
                        }
                    }
                    logger->addAppender(ap);
                }
            }

            // 删除日志, 旧的有，新的没有，则需要删除
            for(auto& i : old_value) {
                auto it = new_value.find(i);
                if(it == new_value.end()) {
                    // 删除logger
                    auto logger = SYLAR_LOG_NAME(i.name);
                    logger->setLevel((LogLevel::Level)100);
                    logger->clearAppender();
                }
            }

        });
    }
};

static LogIniter __log_init;

void LoggerManager::init() {
    
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

LogLevel::Level LogLevel::FromString(const std::string& str) {
#define XX(level, v)               \
    if (str == #v) {        \
        return LogLevel::level; \
    }
    XX(DEBUG, debug);
    XX(INFO, info);
    XX(WARN, warn);
    XX(ERROR, error);
    XX(FATAL, fatal);

    XX(DEBUG, DEBUG);
    XX(INFO, INFO);
    XX(WARN, WARN);
    XX(ERROR, ERROR);
    XX(FATAL, FATAL);
    return LogLevel::UNKNOW;
#undef XX
}

/////////////////////////////////////Logger/////////////////////////////////////
Logger::Logger(const std::string& name)
    : m_name(name), m_level(LogLevel::DEBUG) {
    // 设置默认的日志格式
    m_formatter.reset(new LogFormatter("%d%T%t%T%N%T%F%T[%p]%T[%c]%T%f:%l%T%m%n"));
}

void Logger::log(LogLevel::Level level, LogEvent::ptr event) {
    if (level >= m_level) {
        // 通过shared_from_this把自身的指针作为参数进行传递
        auto self = shared_from_this();
        MutexType::Lock lock(m_mutex);
        if (!m_appenders.empty()) {
            // 遍历当前所拥有的输出器，逐个输出日志
            for (auto& i : m_appenders) {
                i->log(self, level, event);
            }
        } else if(m_root) {
            // 如果当前logger没有appender，就把日志交给m_root处理
            m_root->log(level, event);
        }
    }
}

// 增加appender
void Logger::addAppender(LogAppender::ptr appender) {
    MutexType::Lock lock(m_mutex);
    // 若记录日志时没有指定日志格式，使用默认的日志格式
    if (!appender->getFormmater()) {
        MutexType::Lock lock2(appender->m_mutex);
        appender->m_formatter = m_formatter;
    }
    // 将当前输出器加入到logger的输出器集合中
    m_appenders.push_back(appender);
}
// 删除指定appender
void Logger::delAppender(LogAppender::ptr appender) {
    MutexType::Lock lock(m_mutex);
    for (auto it = m_appenders.begin(); it != m_appenders.end(); ++it) {
        if (*it == appender) {
            m_appenders.erase(it);
            break;
        }
    }
}

// 清空appender
void Logger::clearAppender() {
    m_appenders.clear();
}

void Logger::setFormatter(LogFormatter::ptr val) {
    MutexType::Lock lock(m_mutex);
    m_formatter = val;

    // 将logger的appender内的formatter与logger的formatter保持一致
    for(auto& i : m_appenders) {
        MutexType::Lock lock2(i->m_mutex);
        if(!i->m_hasFormatter) {
            i->m_formatter = m_formatter;
        }
    }
}

void Logger::setFormatter(const std::string& val) {
    // 先将当前的Formatter解析出来，查看是否有错误
    sylar::LogFormatter::ptr new_val(new sylar::LogFormatter(val));
    if(new_val->isError()) {
        std::cout << "Logger setFormatter name=" << m_name
                  << "value=" << val << " invalid formatter"
                  << std::endl;
        return;
    }
    // m_formatter = new_val;
    setFormatter(new_val);
}

LogFormatter::ptr Logger::getFormatter() {
    MutexType::Lock lock(m_mutex);
    return m_formatter;
}

std::string Logger::toYamlString() {
    MutexType::Lock lock(m_mutex);
    YAML::Node node;
    node["name"] = m_name;
    if(m_level != LogLevel::UNKNOW) {
        node["level"] = LogLevel::ToString(m_level);
    }
    if(m_formatter) {
        node["formatter"] = m_formatter->getPattern();
    }

    for(auto& i : m_appenders) {
        node["appenders"].push_back(YAML::Load(i->toYamlString()));
    }
    std::stringstream ss;
    ss << node;
    return ss.str();
}

// 每一个级别对应单独的方法
void Logger::debug(LogEvent::ptr event) {
    log(LogLevel::DEBUG, event);
}

void Logger::info(LogEvent::ptr event) {
    log(LogLevel::INFO, event);
}

void Logger::warn(LogEvent::ptr event) {
    log(LogLevel::WARN, event);
}

void Logger::error(LogEvent::ptr event) {
    log(LogLevel::ERROR, event);
}

void Logger::fatal(LogEvent::ptr event) {
    log(LogLevel::FATAL, event);
}

/////////////////////////////////////FileLogAppender/////////////////////////////////////
FileLogAppender::FileLogAppender(const std::string& filename)
    : m_filename(filename) {
    reopen();
}

void FileLogAppender::log(std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) {
    if (level >= m_level) {
        uint64_t now = time(0);
        if(now != m_lastTime) {
            // 写文件时周期性reopen文件，防止文件被删除
            reopen();
            m_lastTime = now;
        }
        // 将格式化后的日志输出到文件中
        MutexType::Lock lock(m_mutex);
        m_filestream << m_formatter->format(logger, level, event);
    }
}

bool FileLogAppender::reopen() {
    MutexType::Lock lock(m_mutex);
    // 判断当前appender是否已打开了文件
    if (m_filestream) {
        m_filestream.close();
    }
    // 以追加写的模式打开文件
    m_filestream.open(m_filename, std::ios_base::app);
    return !m_filestream;
}

std::string FileLogAppender::toYamlString() {
    MutexType::Lock lock(m_mutex);
    YAML::Node node;
    node["type"] = "FileLogAppender";
    node["file"] = m_filename;
    if(m_level != LogLevel::UNKNOW) {
        node["level"] = LogLevel::ToString(m_level);
    }
    if(m_hasFormatter && m_formatter){
        node["formatter"] = m_formatter->getPattern();
    }
    std::stringstream ss;
    ss << node;
    return ss.str();
}

/////////////////////////////////////StdoutLogAppender/////////////////////////////////////

void StdoutLogAppender::log(std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) {
    if (level >= m_level) {
        MutexType::Lock lock(m_mutex);
        // StdoutLogAppender类直接在控制台进行输出
        std::cout << m_formatter->format(logger, level, event);
    }
}

std::string StdoutLogAppender::toYamlString() {
    MutexType::Lock lock(m_mutex);
    YAML::Node node;
    node["type"] = "StdoutLogAppender";
    if(m_level != LogLevel::UNKNOW) {
        node["level"] = LogLevel::ToString(m_level);
    }
    if (m_hasFormatter && m_formatter) {
        node["formatter"] = m_formatter->getPattern();
    }
    std::stringstream ss;
    ss << node;
    return ss.str();
}
/////////////////////////////////////LogAppender/////////////////////////////////////
void LogAppender::setFormatter(LogFormatter::ptr val) {
    // 多线程下设置与读取可能存在访问冲突，需要加锁
    MutexType::Lock lock(m_mutex);
    m_formatter = val;
    if(m_formatter) {
        m_hasFormatter = true;
    } else {
        m_hasFormatter = false;
    }
}

LogFormatter::ptr LogAppender::getFormmater() {
    // 多线程下设置与读取可能存在访问冲突，需要加锁
    MutexType::Lock lock(m_mutex);
    return m_formatter;
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
            m_error = true;
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

        XX(m, MessageFormatItem),           // m: 消息
        XX(p, LevelFormatItem),             // p: 日志级别
        XX(r, ElapseFormatItem),            // r: 累计毫秒数
        XX(c, NameFormatItem),              // c: 日志名称
        XX(t, ThreadFormatItem),            // t: 线程ID   
        XX(F, FiberFormatItem),             // F: 协程ID
        XX(n, NewLineFormatItem),           // n: 换行
        XX(d, DateTimeFormatItem),          // d: 时间
        XX(f, FilenameFormatItem),          // f: 文件名
        XX(l, LineFormatItem),              // l: 行号
        XX(T, TabFormatItem),               // T: Tab
        XX(N, ThreadNameFormatItem)         // N: 线程名称

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
                m_error = true;
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