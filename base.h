#pragma once

#define _CRT_SECURE_NO_DEPRECATE // 去掉4996警告

#include <stdio.h>
#include <iostream>
#include <string>
#include <time.h>
#include <new>
#include <malloc.h>
#include <time.h>
#include <string.h>

#if defined _WINDOWS || defined WIN32 || defined WIN64

#include <stdlib.h>
#include <crtdbg.h>
#include <Windows.h>
#include <Psapi.h>

#pragma comment(lib, "psapi.lib") 

#else

#include <sys/time.h>
#include <unistd.h>
        #if defined _SYSCALL
        #include <sys/syscall.h>
        #define gettid() syscall(__NR_gettid)
        #endif

#endif

#include <map>
#include <unordered_map>
#include <list>
#include <vector>

using namespace std;

inline std::string getTimeString()
{
        struct tm *t;
        time_t tt;
        time(&tt);
        t = localtime(&tt);
        char curtime[256] = {0};
        sprintf( curtime, "%02d:%02d:%02d", t->tm_hour, t->tm_min, t->tm_sec);
        return std::string( curtime );
}

#define LOG(os) std::cout<<"["<<getTimeString()<<"]["<<__FUNCTION__<<":"<<__LINE__<<"]"<<os<<std::endl

unsigned long long getTimeStampMsec();
int getRss();
void __sleep(float sec);
