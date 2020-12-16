#include "thread.h"
#include "log.h"
#include "util.h"

namespace sylar {

///////////////////////////////thread///////////////////////////////

static sylar::Logger::ptr g_logger = SYLAR_LOG_NAME("system");

static thread_local Thread* t_thread = nullptr;     // 定义线程局部变量来获取当前线程
static thread_local std::string t_thread_name = "UNKNOW";
Thread* Thread::GetThis() {
 return t_thread;
}

const std::string& Thread::GetName() {
    return t_thread_name;
}

void Thread::SetName(const std::string& name) {
    if(t_thread) {
        t_thread->m_name = name;
    }
    t_thread_name = name;
}

Thread::Thread(std::function<void()> cb, const std::string& name) 
    :m_cb(cb)
    ,m_name(name) {
    if(name.empty()) {
        m_name = "UNKNOW";
    }
    int rt = pthread_create(&m_thread, nullptr, &Thread::run, this);
    if(rt) {
        SYLAR_LOG_ERROR(g_logger) << "pthread_create thread fail, rt=" << rt
            << " name=" << name;
        throw std::logic_error("pthread_create error");
    }

    // 等待创建的线程运行起来后才离开构造函数
    m_semaphore.wait();
}

Thread::~Thread() {
    if(m_thread) {
        pthread_detach(m_thread);
    }
}

void Thread::join() {
    if(m_thread) {
        int rt = pthread_join(m_thread, nullptr);
        if(rt) {
            SYLAR_LOG_ERROR(g_logger) << "pthread_join thread fail, rt=" << rt
            << " name=" << m_name;
            throw std::logic_error("pthread_join error");
        }
        // join完成后清空线程
        m_thread = 0;
    }
}

void* Thread::run(void* arg) {
    // 线程启动函数
    Thread* thread = (Thread*)arg;
    t_thread = thread;
    t_thread_name = thread->m_name;
    // 设置线程ID
    thread->m_id = sylar::GetThreadID();
    // 设置线程名称
    pthread_setname_np(pthread_self(), thread->m_name.substr(0, 15).c_str());

    // 设置线程处理函数
    std::function<void()> cb;
    cb.swap(thread->m_cb);

    thread->m_semaphore.notify();

    cb();
    return 0;
}

///////////////////////////////Semaphore///////////////////////////////

Semaphore::Semaphore(uint32_t count) {
    if(sem_init(&m_semaphore, 0, count)) {
        throw std::logic_error("sem_init error");
    }
}

Semaphore::~Semaphore() {
    sem_destroy(&m_semaphore);
}

void Semaphore::wait() {
    if(sem_wait(&m_semaphore)) {
        throw std::logic_error("sem_wait error");
    }
}

void Semaphore::notify() {
    if(sem_post(&m_semaphore)) {
        throw std::logic_error("sem_post error");
    }
}


}