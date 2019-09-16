#include "localRedis.h"
#include "valueType.h"
#include "dataNode.h"
#include "zSet.h"

localRedis::localRedis()
{
	_db = dataBase::_create();
}

localRedis::~localRedis() 
{
	dataBase::_delete(_db);
	_db = nullptr;
}

void localRedis::printAll()
{
	_db->print("");
}

void localRedis::printAllCount()
{
	_db->printCount("");
}

unsigned int localRedis::getAllUsed()
{
	return _db->getUsed();
}

unsigned int localRedis::getAllSize()
{
	return _db->getSize();
}

unsigned int localRedis::set(const std::string& key, const std::string& value, bool force)
{
	valueType* v = valueType::createString(value);
	dataNode* node = dataNode::_create(key, v);

	return _db->set(node, force);
}

unsigned int localRedis::set(const std::string& key, const int value, bool force)
{
	valueType* v = valueType::createInt(value);
	dataNode* node = dataNode::_create(key, v);

	return _db->set(node, force);
}

unsigned int localRedis::get(const std::string& key, string& ret)
{
	dataNode* node = _db->find(key);
	if (node == nullptr)
		return retType::NO_KEY;
	
	if (node->getType() != VALUE_TYPE_STRING)
		return retType::TYPE_ERR;

	ret = (char*)node->getData();
	return retType::OK;
}

unsigned int localRedis::get(const std::string& key, int& ret)
{
	dataNode* node = _db->find(key);
	if (node == nullptr)
		return retType::NO_KEY;

	if (node->getType() != VALUE_TYPE_INT)
		return retType::TYPE_ERR;

	ret = (int)*(int*)node->getData();
	return retType::OK;
}

unsigned int localRedis::getClass(const std::string& key, BaseDataPtr** ret)
{
	dataNode* node = _db->find(key);
	if (node == nullptr)
		return retType::NO_KEY;

	if (node->getType() != VALUE_TYPE_CLASS)
		return retType::TYPE_ERR;

	*ret = (BaseDataPtr*)node->getData();
	return retType::OK;
}

unsigned int localRedis::del(const std::string& key)
{
	return _db->del(key);
}

unsigned int localRedis::hset(const string& key, const string& field, const string& value, bool force)
{
	valueType* v = valueType::createDB();
	dataNode* node = dataNode::_create(key, v);

	unsigned int ret = _db->set(node, force);
	if(ret != retType::OK)
		return ret;

	dataBase* hdb = (dataBase*)node->getData();

	valueType* hv = valueType::createString(value);
	dataNode* hnode = dataNode::_create(field,hv);

	return hdb->set(hnode, force);
}

unsigned int localRedis::hset(const std::string& key, const std::string& field, const int value, bool force)
{
	valueType* v = valueType::createDB();
	dataNode* node = dataNode::_create(key, v);

	unsigned int ret = _db->set(node, force);
	if(ret != retType::OK)
		return ret;

	dataBase* hdb = (dataBase*)node->getData();

	valueType* hv = valueType::createInt(value);
	dataNode* hnode = dataNode::_create(field,hv);

	return hdb->set(hnode, force);
}

unsigned int localRedis::hget(const string& key, const string& field, string& ret)
{
	dataNode* node = _db->find(key);
	if(node == nullptr)
		return retType::NO_KEY;

	if(node->getType() != VALUE_TYPE_HASHTABLE)
		return retType::TYPE_ERR;

	dataBase* fdb = (dataBase*)node->getData();
	dataNode* fnode = fdb->find(field);
	if(fnode == nullptr)
		return retType::NO_FIELD;

	if(node->getType() != VALUE_TYPE_STRING)
		return retType::TYPE_ERR;

	ret = (char*)fnode->getData();
	return retType::OK;
}

unsigned int localRedis::hget(const std::string& key, const std::string& field, int& ret)
{
	dataNode* node = _db->find(key);
	if (node == nullptr)
		return retType::NO_KEY;

	if (node->getType() != VALUE_TYPE_HASHTABLE)
		return retType::TYPE_ERR;

	dataBase* fdb = (dataBase*)node->getData();
	dataNode* fnode = fdb->find(field);
	if (fnode == nullptr)
		return retType::NO_FIELD;

	if (node->getType() != VALUE_TYPE_INT)
		return retType::TYPE_ERR;

	ret = (int)*(int*)fnode->getData();
	return retType::OK;
}

unsigned int localRedis::hdel(const std::string& key, const std::string& field)
{
	dataNode* node = _db->find(key);
	if(node == nullptr)
		return retType::NO_KEY;

	if(node->getType() != VALUE_TYPE_HASHTABLE)
		return retType::TYPE_ERR;
	
	dataBase* fdb = (dataBase*)node->getData();
	unsigned int ret = fdb->del(field);
	if(ret != retType::OK)
	{
		if(ret == retType::NO_KEY)
			ret = retType::NO_FIELD;
		return ret;
	}

	// 如果hashtable空了,则删除整个table
	if(fdb->getUsed()==0)
	{
		return _db->del(key);
	}
	return retType::OK;
}

unsigned int localRedis::zadd(const std::string& key, const std::string& field, const double value)
{
	// 先查找或创建zset结构
	dataNode* node = _db->find(key);
	if(node == nullptr)
	{
		valueType* v = valueType::createZSet();
		node = dataNode::_create(key, v);
		_db->add(node);
	}
	else
	{
		if(node->getType() != VALUE_TYPE_ZSET)
			return retType::TYPE_ERR;
	}

	zSet* set = (zSet*)node->getData();
	return set->addScore(field, value);
}

unsigned int localRedis::zcard(const string& key, unsigned long& ret)
{
	dataNode* node = _db->find(key);
	if (node == nullptr)
		return retType::NO_KEY;
	if (node->getType() != VALUE_TYPE_ZSET)
		return retType::TYPE_ERR;

	zSet* set = (zSet*)node->getData();
	ret = set->getLength();
	return retType::OK;
}

unsigned int localRedis::zcount(const std::string& key, const double minScore, const double maxScore, unsigned int& ret)
{
	dataNode* node = _db->find(key);
	if (node == nullptr)
		return retType::NO_KEY;
	if (node->getType() != VALUE_TYPE_ZSET)
		return retType::TYPE_ERR;

	zSet* set = (zSet*)node->getData();
	return set->countRange(minScore, maxScore, ret);
}

unsigned int localRedis::zscore(const std::string& key, const std::string& field, double& score)
{
	dataNode* node = _db->find(key);
	if(node == nullptr)
		return retType::NO_KEY;
	if(node->getType() != VALUE_TYPE_ZSET)
		return retType::TYPE_ERR;

	zSet* set = (zSet*)node->getData();
	return set->getScore(field, score);
}

unsigned int localRedis::zrank(const std::string& key, const std::string& field, unsigned int& rank)
{
	dataNode* node = _db->find(key);
	if(node == nullptr)
		return retType::NO_KEY;
	if(node->getType() != VALUE_TYPE_ZSET)
		return retType::TYPE_ERR;

	zSet* set = (zSet*)node->getData();
	return set->getRank(field, rank);
}

unsigned int localRedis::zrem(const std::string& key, const std::string& field)
{
	dataNode* node = _db->find(key);
	if(node == nullptr)
		return retType::NO_KEY;
	if(node->getType() != VALUE_TYPE_ZSET)
		return retType::TYPE_ERR;

	zSet* set = (zSet*)node->getData();
	return set->remove(field);
}

unsigned int localRedis::zrange(const std::string& key, const int start, const int end, 
								vector<zsdata>& ret)
{
	dataNode* node = _db->find(key);
	if(node == nullptr)
		return retType::NO_KEY;
	if(node->getType() != VALUE_TYPE_ZSET)
		return retType::TYPE_ERR;

	zSet* set = (zSet*)node->getData();
	return set->getRangeByRank(start, end, ret);
}

unsigned int localRedis::zrangebyscore(const std::string& key, const double minScore, 
									   const double maxScore, vector<zsdata>& ret)
{
	dataNode* node = _db->find(key);
	if(node == nullptr)
		return retType::NO_KEY;
	if(node->getType() != VALUE_TYPE_ZSET)
		return retType::TYPE_ERR;

	zSet* set = (zSet*)node->getData();
	return set->getRangeByScore(minScore, maxScore, ret);
}
