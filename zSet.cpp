#include "zSet.h"
#include "valueType.h"
#include "dataNode.h"
#include "dataBase.h"
#include "skipList.h"

zSet::zSet()
{

}

zSet::~zSet()
{

}

zSet* zSet::_create()
{
	__new_obj(zSet, set);

	set->m_db = dataBase::_create();
	set->m_list = skipList::_create();
	return set;
}

void zSet::_delete(zSet* set)
{
	skipList::_delete(set->m_list);
	set->m_list = nullptr;
	dataBase::_delete(set->m_db);
	set->m_db = nullptr;

	__delete_obj(zSet, set);
}

unsigned long zSet::getLength()
{
	return m_list->getLength();
}

unsigned int zSet::addScore(const std::string& key, const double score)
{	
	// 查询zSet中是否已经有这个field
	dataNode* listnode = m_db->find(key);
	if(listnode)
	{
		// 存在
		double oldscore = *(double*)listnode->getData(); // TODO 不应该是其他类型的值
		double newscore = oldscore + score;
		if(oldscore!=newscore)
		{
			// 先从跳跃表中删除再插入
			m_list->deleteScore(oldscore, listnode);
			m_list->insertScore(newscore, listnode);
			*(double*)listnode->getData() = newscore;
		}
	}
	else
	{
		// 不存在,直接插入
		valueType* v = valueType::createDouble(score);
		listnode = dataNode::_create(key, v);
		m_db->add(listnode);
		m_list->insertScore(score,listnode);
	}

	return retType::OK;
}

unsigned int zSet::countRange(const double minScore, const double maxScore, unsigned int& ret)
{
	unsigned int begin = 0;
	unsigned int end = 0;
	
	skipNode* np = m_list->findScoreRangeFirst(minScore, maxScore);
	if(!np)
		return retType::NO_VALUE;

	begin = m_list->getRankByNode(np->data);
	
	np = m_list->findScoreRangeLast(minScore, maxScore);
	if(!np) // 这里应该不会失败的
		return retType::NO_VALUE;

	end = m_list->getRankByNode(np->data);
	ret = (end-begin)+1;
	return retType::OK;
}

unsigned int zSet::getScore(const std::string& key, double& score)
{
	dataNode* node = m_db->find(key);
	if(node == nullptr)
		return retType::NO_KEY;
	
	score = (double)*(double*)node->getData();
	return retType::OK;
}

unsigned int zSet::getRank(const std::string& key, unsigned int& rank)
{
	dataNode* node = m_db->find(key);
	if(node == nullptr)
		return retType::NO_FIELD;
	if(node->getType() != VALUE_TYPE_DOUBLE)
		return retType::TYPE_ERR;

	rank = m_list->getRankByNode(node);
	return retType::OK;
}

unsigned int zSet::remove(const std::string& key)
{
	dataNode* node = m_db->find(key);
	if(node == nullptr)
		return retType::NO_FIELD;
	if(node->getType() != VALUE_TYPE_DOUBLE)
		return retType::TYPE_ERR;

	m_list->deleteScore((double)*(double*)node->getData(), node);
	return m_db->del(key);
}

unsigned int zSet::getRangeByRank(const int start, const int end, vector<zsdata>& ret)
{
	if(start>end || start<=0) // TODO 先强制要求请求的范围准确
		return retType::RANGE_ERR;

	skipNode* s = m_list->findRankRangeFirst(start, end);
	if(!s)
		return retType::NO_VALUE;

	int rank = start;
	skipNode* e = m_list->findRankRangeLast(start, end);

	ret.clear();
	skipNode* np = s;
	while(np && np != e->level[0].forward)
	{
		zsdata d;
		d.key = np->data->getKey();
		d.rank = rank++;
		d.score = np->score;
		ret.push_back(d);
		np = np->level[0].forward;
	}
	return retType::OK;
}

unsigned int zSet::getRangeByScore(const double minScore, const double maxScore, vector<zsdata>& ret)
{
	skipNode* min = m_list->findScoreRangeFirst(minScore, maxScore);
	if(!min)
		return retType::NO_VALUE;

	int rank = m_list->getRankByNode(min->data);
	skipNode* max = m_list->findScoreRangeLast(minScore, maxScore);

	ret.clear();
	skipNode* np = min;
	while(np && np != max->level[0].forward)
	{
		zsdata d;
		d.key = np->data->getKey();
		d.rank = rank++;
		d.score = np->score;
		ret.push_back(d);
		np = np->level[0].forward;
	}
	return retType::OK;
}

void zSet::printList()
{
	m_list->print();
}

void zSet::printDB()
{
	m_db->print("");
}
