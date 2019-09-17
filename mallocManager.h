#pragma once

#include "base.h"
#include "mallocPool.h"

// 检测哪里申请的内存没有释放,可以由makefile定义
// 未做优化,严重影响性能,谨慎开启
//#define _TEST_MEM_

#if defined _TEST_MEM_
	#define __new(size) mallocManager::getInstance().malloc(size, __FILE__, __LINE__)
#else
	#define __new(size) mallocManager::getInstance().malloc(size)
#endif
#define __delete(p) mallocManager::getInstance().free((char*)(p))

#define __new_obj(classname, p)				classname* (p) = (classname*)__new(sizeof(classname));new((p)) classname()
#define __new_obj_1(classname, p, v1)		classname* (p) = (classname*)__new(sizeof(classname));new((p)) classname((v1))
#define __new_obj_2(classname, p, v1, v2)	classname* (p) = (classname*)__new(sizeof(classname));new((p)) classname((v1), (v2))
#define __delete_obj(classname, p)			{(p)->~classname();__delete((p));}

#define __new_copy_char(str, p)				\
	char* p = __new(str.length()+1);		\
	memcpy(p, str.c_str(), str.length());	\
	p[str.length()] = '\0'

#define __new_copy_int(num, p)		\
	char* p = __new(sizeof(int));	\
	*(int*)p = num

#define __new_copy_double(num, p)	\
	char* p = __new(sizeof(double));\
	*(double*)p = num

static const unsigned int mem_max_size = 1024 * 8;

struct recode
{
	string filename;
	int line;
};

typedef map<unsigned int, mallocPool*>				SIZE_POOL_MAP;
typedef unordered_map<unsigned long, mallocPool*>	PTR_POOL_UNMAP;
typedef map<unsigned long, unsigned int>			PTR_SIZE_MAP;
typedef map<unsigned long, recode>					PTR_RECODE_MAP;

class mallocManager
{
public:
	static mallocManager& getInstance()
	{
		static mallocManager _s;
		return _s;
	}

private:
	mallocManager(){};
	~mallocManager();

public:
	// 失败返回空
	char* malloc(unsigned int size, const char* filename, unsigned int line);
	char* malloc(unsigned int size);
	bool free(char* ptr);
	bool avail(char* ptr); // 判断内存是否有效(必须是我申请的内存)

private:
	unsigned int fixSize(unsigned int fsize);		// 调整内存大小为整数
	unsigned int getCount(unsigned int fixSize);	// 根据内存大小调整连续分配几块内存

private:
	SIZE_POOL_MAP	_small_list;	//小内存大小映射表 <size,pool>
	PTR_POOL_UNMAP _small_ptr_list;	//小内存指针映射表 <ptr,pool>
	PTR_SIZE_MAP _big_ptr_list;		//大内指针存映射表 <ptr,size>
	PTR_RECODE_MAP _ptr_recode;		// TEST 记录哪行申请的内存
};
