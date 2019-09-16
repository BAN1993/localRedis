#pragma once

#include "base.h"

using namespace std;

#define FLAG false
#define REC_MEM(os) if(FLAG){LOG(os);}

class mallocPool
{
public:
	mallocPool(unsigned int size, unsigned int count);
	~mallocPool();

public:
	// 失败返回空
	char* malloc();
	bool free(char* ptr);

private:
	bool doExpand();

private:
	unsigned int _size;						// 每块内存大小
	unsigned int _count;					// 每次扩展几块
	list<char*> _all_mem_list;				// 整块内存列表
	list<char*> _free_list;					// 空闲内存列表

private:
	unsigned long long _history_count;		// 一共分配了多少块内存
	unsigned long long _history_get;		// 一共获取了几次
	unsigned long long _history_free;		// 一共释放了几次
};
