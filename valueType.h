#pragma once

#include "Define.h"

/************************************************************************
 * 储存的数据
 * m_type	: 节点类型,对应OBJ_TYPE_*
 * m_len	: 数据长度
 * m_data	: 储存的数据,可以是任意类型,由type确定
************************************************************************/
class valueType
{
public:
	valueType();
	~valueType();

public:
	static valueType* createDB();							// 创建database
	static valueType* createZSet();							// 创建有序集合
	static valueType* createString(const string& value);
	static valueType* createInt(const int value);
	static valueType* createDouble(const double value);
	template<typename T>
	static valueType* createClass(const T& value);			// 创建结构体类型

	static void _delete(valueType* vt);

public:
	unsigned int getType();
	void* getData();
	void resetData();		// 重置数据,会调用delete
	
private:
	unsigned int m_type;
	int m_len;
	void* m_data;
};

template <class T>
valueType* valueType::createClass(const T& value)
{	
	__new_obj(valueType, p);
	__new_obj(T, a);

	p->m_type = VALUE_TYPE_CLASS;
	p->m_data = a;
	p->m_len = sizeof(a);
	return p;
}
