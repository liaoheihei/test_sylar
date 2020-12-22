#include "config.h"

namespace sylar {


/////////////////////////////////////Config/////////////////////////////////////
// 实例化m_datas静态变量
//Config::ConfigVarMap Config::m_datas;


// ConfigVarBase::ptr Config::LookupBase(const std::string& name) {
//     auto it = m_datas.find(name);
//     return it == m_datas.end() ? nullptr : it->second;
// }

static void ListAllMember(const std::string& prefix,
                          const YAML::Node& node,
                          std::list<std::pair<std::string, const YAML::Node> >& output){
    // 若prefix中含有非法字符，输出错误信息
    if (prefix.find_first_not_of("abcdefghijklmnopqrstuvwxyz._0123456789") != std::string::npos){
        SYLAR_LOG_ERROR(SYLAR_LOG_ROOT()) << "Config invalid name: " << prefix << " : " << node;
        return;
    }
    // 将prefix与对应的node放入输出集合中
    output.push_back(std::make_pair(prefix, node));
    if(node.IsMap()){
        for(auto it = node.begin(); it != node.end(); ++it){
            ListAllMember(prefix.empty() ? it->first.Scalar() : (prefix + "." + it->first.Scalar()), it->second, output);
        }
    }
}

void Config::LoadFromYaml(const YAML::Node& root) {
    std::list<std::pair<std::string, const YAML::Node> > all_nodes;
    ListAllMember("", root, all_nodes);

    for(auto& i : all_nodes){
        std::string key = i.first;
        if(key.empty())
            continue;
        
        std::transform(key.begin(), key.end(), key.begin(), ::tolower);
        ConfigVarBase::ptr var = LookupBase(key);

        if(var) {
            if(i.second.IsScalar()) {
                var->fromString(i.second.Scalar());
            } else {
                std::stringstream ss;
                ss << i.second;
                var->fromString(ss.str());
            }
        }
    }
}

void Config::Visit(std::function<void(ConfigVarBase::ptr)> cb) {
    RWMutextype::ReadLock lock(GetMutex());
    ConfigVarMap& m = GetDatas();
    for(auto it = m.begin(); it != m.end(); ++it) {
        cb(it->second);
    }
}

}