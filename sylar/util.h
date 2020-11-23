#ifndef __SYLAR_UTIL_H__
#define __SYLAR_UTIL_H__

#include <sys/syscall.h>
#include <unistd.h>
#include <stdint.h>

namespace sylar{

pid_t GetThreadID();
uint32_t GetFiberID();

}

#endif