#include "mallocManager.h"

mallocManager::~mallocManager()
{
#if defined _TEST_MEM_
	// 打印还未释放的内存
	PTR_RECODE_MAP::iterator itRecode = _ptr_recode.begin();
	for(;itRecode!=_ptr_recode.end();itRecode++)
	{
		LOG("UNDELETE:filename="<<itRecode->second.filename<<",line="<<itRecode->second.line);
	}
#endif

	for (auto it : _small_list)
	{
		if (it.second != nullptr)
		{
			delete it.second;
		}
		it.second = nullptr;
	}
	_small_list.clear();

	for (auto it : _big_ptr_list)
	{
		char* p = (char*)it.first;
		if (p != nullptr)
		{
			delete[] p;
		}
	}
	_big_ptr_list.clear();
}

char* mallocManager::malloc(unsigned int size, const char* filename, unsigned int line)
{
	char* retPtr = malloc(size);
#if defined _TEST_MEM_
	if(retPtr)
	{
		recode r;
		r.filename = filename;
		r.line = line;
		_ptr_recode.insert(make_pair((unsigned long)retPtr, r));
	}
#endif
	return retPtr;
}

char* mallocManager::malloc(unsigned int size)
{
	size = fixSize(size);
	unsigned int count = getCount(size);
	char* retPtr = nullptr;
	if (size < mem_max_size)
	{
		mallocPool* pool = nullptr;
		if (_small_list.find(size) == _small_list.end())
		{
			pool = new mallocPool(size, count);
			_small_list.insert(make_pair(size, pool));
		}

		pool = _small_list.find(size)->second;
		retPtr = pool->malloc();
		if (retPtr != nullptr)
			_small_ptr_list.insert(make_pair((unsigned long)retPtr,pool));
	}
	else
	{
		retPtr = new char[size];
		memset(retPtr, 0, size);
		_big_ptr_list.insert(make_pair((unsigned long)retPtr,size));
	}

	return retPtr;
}

bool mallocManager::free(char* ptr)
{
	if (!ptr)
		return false;
	unsigned long p = (unsigned long)ptr;

#if defined _TEST_MEM_
	PTR_RECODE_MAP::iterator itRecode = _ptr_recode.find(p);
	if(itRecode != _ptr_recode.end()) // 不应该找不到
	{
		_ptr_recode.erase(itRecode);
	}
#endif

	PTR_POOL_UNMAP::iterator itSmall = _small_ptr_list.find(p);
	if (itSmall != _small_ptr_list.end())
	{
		bool ret = itSmall->second->free((char*)p);
		_small_ptr_list.erase(itSmall);
		return ret;
	}

	PTR_SIZE_MAP::iterator itBig = _big_ptr_list.find(p);
	if (itBig != _big_ptr_list.end())
	{
		delete[] (char*)p;
		p = 0;
		_big_ptr_list.erase(itBig);
		return true;
	}

	return false;
}

bool mallocManager::avail(char* ptr)
{
	if(!ptr)
		return false;
	unsigned long p = (unsigned long)ptr;
	if(_small_ptr_list.find(p)!=_small_ptr_list.end())
		return true;
	if(_big_ptr_list.find(p)!=_big_ptr_list.end())
		return true;
	return false;
}

unsigned int mallocManager::fixSize(unsigned int fsize)
{
	unsigned int fact_size = fsize;
	if (fact_size <= 8)
		return 8;
	else if (fact_size <= 16)
		return 16;
	else if (fact_size <= 32)
		return 32;
	else if (fact_size <= 64)
		return 64;
	else if (fact_size <= 128)
		return 128;
	else if (fact_size <= 256)
		return 256;
	else if (fact_size <= 512)
		return 512;
	else if (fact_size <= 1024)
		return 1024;
	else if (fact_size <= 2048)
		return 2048;
	else if (fact_size <= 4096)
		return 4096;
	else if (fact_size <= mem_max_size)
		return mem_max_size;

	return fsize;
}

unsigned int mallocManager::getCount(unsigned int fixSize)
{
	if(fixSize>=mem_max_size)
		return 1;
	return mem_max_size/fixSize;
}
