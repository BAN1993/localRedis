#pragma once

#include "Define.h"

class dataNode;

/************************************************************************
* hash表
* size		: 总分配的节点数
* sizemask	: size-1
* used		: 已用节点数
* table	: 节点一维数组
************************************************************************/
struct hashTable
{
	unsigned long size;
	unsigned long sizemask;
	unsigned long used;
	dataNode **table;

	hashTable()
	{
		size = 0;
		sizemask = 0;
		used = 0;
		table = nullptr;
	}
};

/************************************************************************
* 数据库
* m_ht			: hash表,创建两个是为了doRehash
* m_rehashidx	: doRehash标识,-1表示未执行
* m_used		: 总数据量
************************************************************************/
class dataBase
{
public:
	dataBase();
	~dataBase();

public:
	static dataBase* _create();
	static void _delete(dataBase* db);

public:
	unsigned int set(dataNode* node, bool force);	// 尝试创建,存在则返回已存在数据指针(可覆盖),返回空表示未知错误
	dataNode* find(const std::string& key);			// 查找
	unsigned int del(const std::string& key);		// 删除
	unsigned int add(dataNode* node);				// 添加

public:
	void print(string fix);
	void printCount(string fix);
	unsigned int getUsed();
	unsigned int getSize();

private:
	unsigned int tryExpand();	// 尝试扩充
	unsigned int doExpand(unsigned long size);

private:
	void deleteHashTable();
	void deleteHashTable(hashTable* ht);
	bool isRehash();	// 是否正在执行rehash
	void tryRehash();	// 尝试转移table
	void doRehash();
	unsigned int hashKey(const void* key, int len);	// 根军字符串获取hashkey

private:
	hashTable m_ht[2];
	int m_rehashidx;
	unsigned int m_used;
};
