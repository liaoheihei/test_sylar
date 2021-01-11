#ifndef __SYLAR_UTIL_H__
#define __SYLAR_UTIL_H__

#include <sys/syscall.h>
#include <unistd.h>
#include <stdint.h>
#include <vector>
#include <string>
#include <stdlib.h>

namespace sylar{

pid_t GetThreadID();
uint32_t GetFiberID();

void Backtrace(std::vector<std::string>&,  int size, int skip = 1);
std::string BacktraceToString(int size, int skip = 2, const std::string& prefix = "");

}

#endif