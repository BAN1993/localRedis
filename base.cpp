#include "base.h"

unsigned long long getTimeStampMsec()
{
#if defined _WINDOWS || defined WIN32 || defined WIN64
        struct timeval tv;
        time_t clock;
        struct tm tm;
        SYSTEMTIME wtm;

        GetLocalTime(&wtm);
        tm.tm_year = wtm.wYear - 1900;
        tm.tm_mon = wtm.wMonth - 1;
        tm.tm_mday = wtm.wDay;
        tm.tm_hour = wtm.wHour;
        tm.tm_min = wtm.wMinute;
        tm.tm_sec = wtm.wSecond;
        tm.tm_isdst = -1;
        clock = mktime(&tm);
        tv.tv_sec = (long)clock;
        tv.tv_usec = wtm.wMilliseconds * 1000;
        return ((unsigned long long)tv.tv_sec * 1000 + (unsigned long long)tv.tv_usec / 1000);
#else
        struct timeval tv;
        gettimeofday(&tv, nullptr);
        return ((unsigned long long)tv.tv_sec * 1000 + (unsigned long long)tv.tv_usec / 1000);
#endif
}

#if defined _WINDOWS || defined WIN32 || defined WIN64

int getRss()
{
	HANDLE handle=GetCurrentProcess();
	PROCESS_MEMORY_COUNTERS_EX pmc = {0};
	int a = sizeof(pmc);
	if (!GetProcessMemoryInfo(handle,(PROCESS_MEMORY_COUNTERS*)&pmc,sizeof(pmc)))
	{
		return 0;
	}
	return pmc.WorkingSetSize/1024;
}

#else

#if defined _SYSCALL
bool doPopen(const char* cmdline, std::vector<string>& retVec)
{
        FILE *file = popen(cmdline, "r");
        char line[128]={0};
        string strline;
        if(nullptr==file)
        {
                LOG("file is nullptr");
                return false;
        }
        while(fgets(line, 128, file) != nullptr)
        {
                strline = line;
                retVec.push_back(line);
        }
        pclose(file);
        return true;
}

bool getCpuInfo(int& pid, float& cpu, int& rss)
{
        pid=getpid();
        char cmdline[100]={0};
        sprintf(cmdline,"ps -o %%cpu,rss -p %d 2>&1",pid);
        vector<string> vec;
        if(!doPopen(cmdline,vec))
        {
                LOG("can not get cpuinfo");
                return false;
        }
        sscanf(vec[1].c_str(), "%f %d", &cpu, &rss);
        return true;
}
#endif

int getRss()
{
#if defined _SYSCALL
        int pid=0;
        float cpu=0.0;
        int rss=0;
        if(!getCpuInfo(pid,cpu,rss))
        {
                return 0;
        }
        return rss;
#endif
        return 0;
}

#endif

void __sleep(float sec)
{
#if defined _WINDOWS || defined WIN32 || defined WIN64
	Sleep((DWORD)sec*1000);
#else
        #if defined _SYSCALL
	sleep(sec);
        #endif
#endif
}
