#ifndef __SYLAR_FIBER_H__
#define __SYLAR_FIBER_H__

#include <ucontext.h>
#include <memory>
#include <functional>
#include "thread.h"

namespace sylar {

// 协程类，并允许类获得自身的智能指针
class Fiber : public std::enable_shared_from_this<Fiber> {
public:
    typedef std::shared_ptr<Fiber> ptr;

    enum State {
        INIT,
        HOLD,
        EXEC,
        TERM,
        READY,
        EXCEPT
    };
private:
    Fiber();

public:
    Fiber(std::function<void()> cb, size_t stacksize = 0);
    ~Fiber();

    // 重置协程函数，并重置状态(INIT, TERM)
    void reset(std::function<void()> cb);
    
    // 切换到当前协程执行
    void swapIn();

    // 将当前协程切换到后台
    void swapOut();

    // 获得协程ID
    uint64_t getID() const { return m_id; }

public:
    static void SetThis(Fiber* f);  //设置当前协程 
    static Fiber::ptr GetThis();    //获取自身当前的协程
    static void YieldToReady();     //协程切换到后台，改变自身执行状态为Ready
    static void YieldToHold();      //协程切换到后台，改变自身执行状态为Hold

    static uint64_t TotalFibers();  // 获得当前总协程数

    static void MainFunc();

    static uint64_t GetFiberID();

private:
    uint64_t m_id = 0;
    uint32_t m_stacksize = 0;
    State m_state = INIT;

    ucontext_t m_ctx;
    void* m_stack = nullptr;

    std::function<void()> m_cb;

};

}

#endif