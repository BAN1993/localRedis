#include "mallocPool.h"

mallocPool::mallocPool(unsigned int size, unsigned int count)
{
	_size = size;
	_count = count;
	_history_count = 0;
	_history_get = 0;
	_history_free = 0;
	doExpand();
}

mallocPool::~mallocPool()
{
	LOG("--size="<<_size<<",count="<<_history_count<<",free="<<_free_list.size()<<",historyget="<<_history_get<<",historyfree="<<_history_free);
	for (auto p : _all_mem_list)
	{
		delete[] p;
		p = nullptr;
	}
}

char* mallocPool::malloc()
{
	// 空闲内存不足,扩充
	if (_free_list.empty())
		doExpand();

	char* p = nullptr;
	if (!_free_list.empty())
	{
		p = _free_list.front();
		_free_list.pop_front();
		memset(p, 0, _size);
		_history_get++;
	}
	return p;
}

bool mallocPool::free(char* ptr)
{
	if (ptr == nullptr)
		return false;

	_free_list.push_back(ptr); // 插回空闲队列

	_history_free++;
	return true;
}

bool mallocPool::doExpand()
{
	// 申请整块内存
	char* p = new char[_size * _count];
	_all_mem_list.push_back(p);
	
	// 拆分并插入空闲队列
	char* np = p;
	for (unsigned int i = 0; i < _count; i++)
	{
		_free_list.push_back(np);
		np += _size;
	}
	
	_history_count += _count;

	return true;
}
