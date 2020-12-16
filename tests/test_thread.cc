#include "../sylar/sylar.h"

sylar::Logger::ptr g_logger = SYLAR_LOG_ROOT();

int count = 0;
sylar::RWMutex s_mutex;

void func1() {
    SYLAR_LOG_INFO(g_logger) << "name: " << sylar::Thread::GetName()
                             << " this.name: " << sylar::Thread::GetThis()->getName()
                             << " id: " << sylar::GetThreadID()
                             << " this.id: " << sylar::Thread::GetThis()->getID();
    for(int i = 0; i < 100000; ++i) {
        sylar::RWMutex::WriteLock lock(s_mutex);
        ++count;
    }
}

void func2() {
}

int main(int argc, char** argv) {
    SYLAR_LOG_INFO(g_logger) << "thread test begin";
    std::vector<sylar::Thread::ptr> thrs;
    for(int i = 0; i < 5; ++i) {
        // std::cout << "name_" + std::to_string(i) << std::endl;
        sylar::Thread::ptr thr(new sylar::Thread(&func1, "name_" + std::to_string(i)));
        thrs.push_back(thr);
        // std::cout << "current ID: " << i << std::endl;
    }

    for(int i = 0; i < 5; ++i) {
        thrs[i]->join();
    }
    SYLAR_LOG_INFO(g_logger) << "thread test end";
    SYLAR_LOG_INFO(g_logger) << "count = " << count;
    return 0;
}