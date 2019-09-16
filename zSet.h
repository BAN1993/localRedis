#pragma once

#include "Define.h"

class dataBase;
class skipList;

/************************************************************************
* 有序集合
* m_db		: 所有的节点
* m_list	: 跳跃表
************************************************************************/
class zSet
{
public:
	zSet();
	~zSet();

public:
	static zSet* _create();
	static void _delete(zSet* set);

public:
	unsigned long getLength();

public:
	unsigned int addScore(const std::string& key, const double score);
	unsigned int countRange(const double minScore, const double maxScore, unsigned int& ret);
	unsigned int getScore(const std::string& key, double& score);
	unsigned int getRank(const std::string& key, unsigned int& rank);
	unsigned int remove(const std::string& key);
	unsigned int getRangeByRank(const int start, const int end, vector<zsdata>& ret);
	unsigned int getRangeByScore(const double minScore, const double maxScore, vector<zsdata>& ret);

public:
	void printList();
	void printDB();

private:
	dataBase* m_db;
	skipList* m_list;
};
