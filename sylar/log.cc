#include "log.h"

namespace sylar{
/////////////////////////////////////Logger/////////////////////////////////////
    Logger::Logger(const std::string& name = "root") : m_name(name){

    }

    void Logger::log(LogLevel::Level level, LogEvent::ptr event){
        if(level >= m_level){
            for(auto& i : m_appenders){
                i->log(level, event);
            }
        }
    }

    // 增加appender
    void Logger::addAppender(LogAppender::ptr appender){
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
        : m_filename(filename){

    }

    void FileLogAppender::log(LogLevel::Level level, LogEvent::ptr event){
        if(level >= m_level){
            m_filestream << m_formatter->format(event);
        }
    }

    bool FileLogAppender::reopen(){
        if(m_filestream) {
            m_filestream.close();
        }
        m_filestream.open(m_filename);
        return !m_filestream;
    }

    void StdoutLogAppender::log(LogLevel::Level level, LogEvent::ptr event){
        if(level >= m_level){
            std::cout << m_formatter->format(event);
        }
    }

/////////////////////////////////////LogFormatter/////////////////////////////////////
LogFormatter::LogFormatter(const std::string& pattern) : m_pattern(pattern){

}

std::string LogFormatter::format(LogEvent::ptr event){
    std::stringstream ss;
    for(auto& i : m_items){
        i->format(ss, event);
    }
    return ss.str();
}

void LogFormatter::init(){
    // str, format, type
    std::vector<std::tuple<std::string, std::string, int> > vec;
    std::string str;
    for(size_t i = 0; i < m_pattern.size(); ++i){
        if(m_pattern[i] != '%'){
            str.append(1, m_pattern[i]);
            continue;
        }
        size_t n = i + 1;
        int fmt_status = 0;
        size_t fmt_begin = 0;

        std::string str;
        std::string fmt;
        while(n < m_pattern.size()){
            if(isspace(m_pattern[n])){
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
                    fmt_status = 2;
                    ++n;
                    continue;
                }
            }
        }

        if(fmt_status == 0){
            str = m_pattern.substr(i + 1, n - i - 1);
        }
    }
}

}