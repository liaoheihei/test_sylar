#ifndef __SYLAR_CONFIG_H__
#define __SYLAR_CONFIG_H__

#include <memory>
#include <sstream>
#include <string>
#include <boost/lexical_cast.hpp>
#include <yaml-cpp/yaml.h>
#include <vector>
#include <list>
#include <map>
#include <set>
#include <unordered_map>
#include <unordered_set>
#include <functional>

#include "log.h"

namespace sylar{

/*
配置系统
*/

class ConfigVarBase{
public:
    typedef std::shared_ptr<ConfigVarBase> ptr;
    ConfigVarBase(const std::string& name, const std::string& description = "")
        : m_name(name),
        m_description(description) {
            std::transform(m_name.begin(), m_name.end(), m_name.begin(), ::tolower);
        }
    virtual ~ConfigVarBase() {}

    const std::string& getName() { return m_name; }
    const std::string& getDescription() { return m_description; }

    virtual std::string toString() = 0;
    virtual bool fromString(const std::string& val) = 0;
    virtual std::string getTypeName() const = 0;


protected:
    std::string m_name;
    std::string m_description;
};

// 基础类型转换仿函数, F from_type, T to_type
template<class F, class T>
class LexicalCast{
public:
    T operator() (const F& v) {
        return boost::lexical_cast<T>(v);
    }
};

// 常用容器转换仿函数,偏特化vector
template<class T>
class LexicalCast <std::string, std::vector<T> > {
public:
    std::vector<T> operator() (const std::string& v) {
        YAML::Node node = YAML::Load(v);
        std::stringstream ss;
        typename std::vector<T> m_set;
        for(size_t i = 0; i < node.size(); ++i) {
            ss.str("");
            ss << node[i];
            m_set.emplace_back(LexicalCast<std::string, T>() (ss.str()));
        }
        return m_set;
    }
};

template <class T>
class LexicalCast<std::vector<T>, std::string> {
   public:
    std::string operator()(const std::vector<T>& v) {
        YAML::Node node;
        for(auto& i : v){
            node.push_back(YAML::Load(LexicalCast<T, std::string>() (i)));
        }
        std::stringstream ss;
        ss << node;
        return ss.str();

    }
};

// 常用容器转换仿函数,偏特化list
template<class T>
class LexicalCast <std::string, std::list<T> > {
public:
    std::list<T> operator() (const std::string& v) {
        YAML::Node node = YAML::Load(v);
        std::stringstream ss;
        typename std::list<T> m_set;
        for(size_t i = 0; i < node.size(); ++i) {
            ss.str("");
            ss << node[i];
            m_set.emplace_back(LexicalCast<std::string, T>() (ss.str()));
        }
        return m_set;
    }
};

template <class T>
class LexicalCast<std::list<T>, std::string> {
   public:
    std::string operator()(const std::list<T>& v) {
        YAML::Node node;
        for(auto& i : v){
            node.push_back(YAML::Load(LexicalCast<T, std::string>() (i)));
        }
        std::stringstream ss;
        ss << node;
        return ss.str();

    }
};

// 常用容器转换仿函数,偏特化set
template<class T>
class LexicalCast <std::string, std::set<T> > {
public:
    std::set<T> operator() (const std::string& v) {
        YAML::Node node = YAML::Load(v);
        std::stringstream ss;
        typename std::set<T> m_set;
        for(size_t i = 0; i < node.size(); ++i) {
            ss.str("");
            ss << node[i];
            m_set.insert(LexicalCast<std::string, T>() (ss.str()));
        }
        return m_set;
    }
};

template <class T>
class LexicalCast<std::set<T>, std::string> {
   public:
    std::string operator()(const std::set<T>& v) {
        YAML::Node node;
        for(auto& i : v){
            node.push_back(YAML::Load(LexicalCast<T, std::string>() (i)));
        }
        std::stringstream ss;
        ss << node;
        return ss.str();

    }
};

// 常用容器转换仿函数,偏特化unordered_set
template<class T>
class LexicalCast <std::string, std::unordered_set<T> > {
public:
    std::unordered_set<T> operator() (const std::string& v) {
        YAML::Node node = YAML::Load(v);
        std::stringstream ss;
        typename std::unordered_set<T> m_set;
        for(size_t i = 0; i < node.size(); ++i) {
            ss.str("");
            ss << node[i];
            m_set.insert(LexicalCast<std::string, T>() (ss.str()));
        }
        return m_set;
    }
};

template <class T>
class LexicalCast<std::unordered_set<T>, std::string> {
   public:
    std::string operator()(const std::unordered_set<T>& v) {
        YAML::Node node;
        for(auto& i : v){
            node.push_back(YAML::Load(LexicalCast<T, std::string>() (i)));
        }
        std::stringstream ss;
        ss << node;
        return ss.str();

    }
};

// 常用容器转换仿函数,偏特化map
template<class T>
class LexicalCast <std::string, std::map<std::string, T> > {
public:
    std::map<std::string, T> operator() (const std::string& v) {
        YAML::Node node = YAML::Load(v);
        std::stringstream ss;
        typename std::map<std::string, T> m_map;
        for(auto it = node.begin(); it != node.end(); ++it) {
            ss.str("");
            ss << it->second;
            m_map.insert(std::make_pair(it->first.Scalar(), LexicalCast<std::string, T>() (ss.str())));
        }
        return m_map;
    }
};

template <class T>
class LexicalCast<std::map<std::string, T>, std::string> {
   public:
    std::string operator()(const std::map<std::string, T>& v) {
        YAML::Node node;
        for(auto& i : v){
            node[i.first] = YAML::Load(LexicalCast<T, std::string>() (i.second));
        }
        std::stringstream ss;
        ss << node;
        return ss.str();

    }
};

// 常用容器转换仿函数,偏特化unordered_map
template<class T>
class LexicalCast <std::string, std::unordered_map<std::string, T> > {
public:
    std::unordered_map<std::string, T> operator() (const std::string& v) {
        YAML::Node node = YAML::Load(v);
        std::stringstream ss;
        typename std::unordered_map<std::string, T> m_map;
        for(auto it = node.begin(); it != node.end(); ++it) {
            ss.str("");
            ss << it->second;
            m_map.insert(std::make_pair(it->first.Scalar(), LexicalCast<std::string, T>() (ss.str())));
        }
        return m_map;
    }
};

template <class T>
class LexicalCast<std::unordered_map<std::string, T>, std::string> {
   public:
    std::string operator()(const std::unordered_map<std::string, T>& v) {
        YAML::Node node;
        for(auto& i : v){
            node[i.first] = YAML::Load(LexicalCast<T, std::string>() (i.second));
        }
        std::stringstream ss;
        ss << node;
        return ss.str();

    }
};

/*
FromStr采用仿函数的方法，类内包括一个T operator() (const std::string&)的方法，
读取字符串并返回指定类型T。
ToStr采用仿函数的方法， 类内包括一个std::string ToStr() (const T&)
读取类型T并返回字符串
*/
template<class T, class FromStr = LexicalCast<std::string, T>, class ToStr = LexicalCast<T, std::string> >
class ConfigVar : public ConfigVarBase {
public:
    typedef std::shared_ptr<ConfigVar> ptr;

    /*当配置发生更改时，通知旧值与新值的回调函数，使用function封装*/
    typedef std::function<void (const T& old_value, const T& new_value)> on_change_cb;

    ConfigVar(const std::string& name, 
                const T& default_value,
                const std::string& description) 
                : ConfigVarBase(name, description)
                , m_val(default_value) {

    }
    
    // 将基础类型T转换为string
    std::string toString() override {
        try {
            // return boost::lexical_cast<std::string>(m_val);
            return ToStr()(m_val);
        } catch(std::exception& e){
            SYLAR_LOG_ERROR(SYLAR_LOG_ROOT()) << "ConfigVar::toString exception" 
                << e.what() << " convert: " << typeid(m_val).name() << " to string";

        }
        return "";
    }

    // 将string转换为基础类型T
    bool fromString(const std::string& val) override {
        try {
            // m_val = boost::lexical_cast<T>(val);
            setValue(FromStr()(val));
        } catch(std::exception& e) {
            SYLAR_LOG_ERROR(SYLAR_LOG_ROOT()) << "ConfigVar::toString exception"
                << e.what() << " convert string to " << typeid(m_val).name();
        }
        return false;
    }

    const T getValue() const { return m_val; }
    void setValue(const T& v) { 
        if(v == m_val)
            return;
        
        for(auto& i : m_cbs) {
            // i为on_change_cb回调函数
            i.second(m_val, v);
        }
        m_val = v;
    }
    std::string getTypeName() const override { return typeid(T).name(); }

    void addListener(uint64_t key, on_change_cb cb) {
        m_cbs[key] = cb;
    }

    void delListener(uint64_t key) {
        m_cbs.erase(key);
    }

    on_change_cb getListener(uint64_t key) {
        auto it = m_cbs.find(key);
        return it == m_cbs.end() ? nullptr : it->second;
    }

    void clearListener() {
        m_cbs.clear();
    }
private:
    T m_val;

    // 变更回调函数组，uint64_t key,要求唯一， 一般可以用hash
    std::map<uint64_t, on_change_cb> m_cbs;
};

class Config {
public:
    typedef std::unordered_map<std::string, ConfigVarBase::ptr> ConfigVarMap;

    template <class T>
    static typename ConfigVar<T>::ptr Lookup(const std::string& name,
                                             const T& default_value, const std::string& description = "") {
        auto it = m_datas.find(name);
        if(it != m_datas.end()) {
            // 将基类ConfigVarBase指针向下转换为ConfigVar<T>指针
            auto tmp  = std::dynamic_pointer_cast<ConfigVar<T> >(it->second);
            if(tmp) {
                // 存在，返回找到的正确类型的value
                SYLAR_LOG_INFO(SYLAR_LOG_ROOT()) << "Lookup name=" << name << " exists";
                return tmp;
            } else {
                // 存在，但类型不正确，返回空指针
                SYLAR_LOG_ERROR(SYLAR_LOG_ROOT()) << "Lookup name=" << name << " exists but type not "
                        << typeid(T).name() << " real_type=" << it->second->getTypeName() << 
                        " " << it->second->toString();
            }
        }

        // auto tmp = Lookup<T>(name);
        // if (tmp) {
        //     SYLAR_LOG_INFO(SYLAR_LOG_ROOT()) << "Lookup name=" << name << " exists";
        //     return tmp;
        // }
        if (name.find_first_not_of("abcdefghijklmnopqrstuvwxyz._0123456789") != std::string::npos) {
            SYLAR_LOG_ERROR(SYLAR_LOG_ROOT()) << "Lookup name invalid " << name;
            throw std::invalid_argument(name);
        }
        typename ConfigVar<T>::ptr v(new ConfigVar<T>(name, default_value, description));
        m_datas[name] = v;
        return v;
    }

    template<class T>
    static typename ConfigVar<T>::ptr Lookup(const std::string& name) {
        auto it = m_datas.find(name);
        if(it == m_datas.end()) {
            return nullptr;
        }
        return std::dynamic_pointer_cast<ConfigVar<T> >(it->second);
    }

    static void LoadFromYaml(const YAML::Node& root);

    // 查找是否存在名称为name的node
    static ConfigVarBase::ptr LookupBase(const std::string& name) {
        auto it = m_datas.find(name);
        return it == m_datas.end() ? nullptr : it->second;
    }

private:
    static ConfigVarMap m_datas;
};

}

#endif