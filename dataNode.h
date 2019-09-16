#pragma once

#include "Define.h"

class valueType;

/************************************************************************
 * 数据节点
 * m_key	: 键值
 * m_data	: 储存的数据
 * m_next	: 下一个节点,解决hash冲突
************************************************************************/
class dataNode
{
public:
	dataNode();
	dataNode(const string& key, valueType* value);
	~dataNode();

public:
	static dataNode* _create(const string& key, valueType* value);
	static void _delete(dataNode* node);

public:
	char* getKey();				// 获取键值
	unsigned int getType();		// 获取类型
	valueType* getValue();		// 获取数据结构体
	void* getData();			// 获取数据
	dataNode* getNext();
	void setNext(dataNode* next);

public:
	int compare(dataNode* other); // 比较node大小, =0:相等, >0:我大, <0:我小
	
private:
    char* m_key;
	valueType* m_value;
    dataNode* m_next;
};
