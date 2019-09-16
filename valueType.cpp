#include "valueType.h"
#include "zSet.h"
#include "dataBase.h"

valueType::valueType()
{
	m_type = VALUE_TYPE_UNKNOWN;
	m_len = 0;
	m_data = nullptr;
}

valueType::~valueType()
{
}

valueType* valueType::createDB()
{
	__new_obj(valueType, p);

	p->m_type = VALUE_TYPE_HASHTABLE;
	p->m_data = dataBase::_create();
	p->m_len = -1;
	return p;
}

valueType* valueType::createZSet()
{
	__new_obj(valueType, p);

	p->m_type = VALUE_TYPE_ZSET;
	p->m_data = zSet::_create();
	p->m_len = -1;
	return p;
}

valueType* valueType::createString(const string& value)
{
	__new_obj(valueType, p);
	__new_copy_char(value, s);
	
	p->m_type = VALUE_TYPE_STRING;
	p->m_data = s;
	p->m_len = value.length()+1;
	return p;
}

valueType* valueType::createInt(const int value)
{
	__new_obj(valueType, p);
	__new_copy_int(value, i);
		
	p->m_type = VALUE_TYPE_INT;
	p->m_data = i;
	p->m_len = sizeof(int);
	return p;
}

valueType* valueType::createDouble(const double value)
{
	__new_obj(valueType, p);
	__new_copy_double(value, d);

	p->m_type = VALUE_TYPE_DOUBLE;
	p->m_data = d;
	p->m_len = sizeof(double);
	return p;
}

void valueType::_delete(valueType* vt)
{
	vt->resetData();
	__delete_obj(valueType, vt);
}

unsigned int valueType::getType()
{
	return m_type;
}

void* valueType::getData()
{
	return m_data;
}

void valueType::resetData()
{
	if(m_data)
	{
		switch (m_type)
		{
		case VALUE_TYPE_HASHTABLE:
			dataBase::_delete((dataBase*)m_data);
			break;

		case VALUE_TYPE_ZSET:
			zSet::_delete((zSet*)m_data);
			break;

		case VALUE_TYPE_CLASS:
			__delete_obj(BaseDataPtr, (BaseDataPtr*)m_data);
			break;

		default: // TODO 先默认当做是基础数据
			__delete(m_data);
			break;
		}
		m_type = VALUE_TYPE_UNKNOWN;
		m_data = nullptr;
	}
}
