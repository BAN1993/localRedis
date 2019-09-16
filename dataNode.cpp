#include "dataNode.h"
#include "valueType.h"

dataNode::dataNode()
{
	m_key = nullptr;
	m_value = nullptr;
	m_next = nullptr;
}

dataNode::dataNode(const string& key, valueType* value)
{
	__new_copy_char(key, s);
	m_key = s;
	m_value = value;
	m_next = nullptr;
}

dataNode::~dataNode()
{
	__delete(m_key);
	m_key = nullptr;
}

dataNode* dataNode::_create(const string& key, valueType* value)
{
	__new_obj_2(dataNode, p, key, value);
	return p;
}

void dataNode::_delete(dataNode* node)
{
	if(node->getValue())
		valueType::_delete(node->getValue());
	__delete_obj(dataNode, node);
}

char* dataNode::getKey()
{
	return m_key;
}

unsigned int dataNode::getType()
{
	if(m_value)
		return m_value->getType();
	return VALUE_TYPE_UNKNOWN;
}

valueType* dataNode::getValue()
{
	return m_value;
}

void* dataNode::getData()
{
	if(m_value)
		return m_value->getData();
	return nullptr;
}

dataNode* dataNode::getNext()
{
	return m_next;
}

void dataNode::setNext(dataNode* next)
{
	m_next = next;
}

int dataNode::compare(dataNode* other)
{
	if(!other) // 默认大于不存在的node
		return 1;
	if(this == other)
		return 0;
	unsigned int mlen = strlen(m_key);
	unsigned int olen = strlen(other->getKey());
	unsigned int minlen = (mlen < olen) ? mlen : olen;
	int cmp = memcmp(m_key, other->getKey(), minlen);
	if(cmp == 0) return mlen-olen;
	return cmp;
}
