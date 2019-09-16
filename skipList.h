#pragma once

#include "Define.h"

class dataNode;

/************************************************************************
* 跳跃表节点
* data		: 数据节点,类型应该都为 VALUE_TYPE_DOUBLE
* score		: 值
* backward	: 后退指针
* level		: 层
*		forward	: 前进指针
*		span	: 跨度,到下一个节点的距离
************************************************************************/
struct skipNode
{
	dataNode *data;
	double score;
	skipNode *backward;
	struct skipNodeLevel 
	{
		skipNode* forward;
		unsigned int span;
	} level[];

	static skipNode* _create(int level, double score, dataNode* node)
	{
		skipNode* p = (skipNode*)__new((sizeof(skipNode)+level*sizeof(skipNode::skipNodeLevel)));
		p->score = score;
		p->data = node;
		return p;
	}

	 // 只负责释放skipNode自己的内存,但不释放dataNode
	static void _delete(skipNode* node)
	{
		__delete_obj(skipNode, node);
	}
};

/************************************************************************
* 跳跃表
* m_head	: 表头
* m_tail	: 表尾
* m_length	: 节点数
* m_level	: 当前使用的最大层数
************************************************************************/
class skipList
{
public:
	skipList();
	~skipList();

public:
	static skipList* _create();
	static void _delete(skipList* list);

public:
	unsigned long getLength();
	int getLevel();

public:
	void deleteScore(double score, dataNode* node); // 只是从跳跃表中删除,不负责从db中删除
	void insertScore(double score, dataNode* node); // 只是插入到跳跃表中,不负责插入到db中
	int randLevel();
	
	unsigned int getRankByNode(dataNode* node);				// 返回此节点的排名,0-不存在
	skipNode* findScoreRangeFirst(double min, double max);	// 查找第一个积分在范围里的节点
	skipNode* findScoreRangeLast(double min, double max);	// 查找最后一个积分在范围里的节点
	skipNode* findRankRangeFirst(int start, int end);		// 查找第一个排名在范围内的节点
	skipNode* findRankRangeLast(int start, int end);		// 查找最后一个排名在范围内的节点
	
public:
	void print();

private:
	skipNode* m_head;
	skipNode* m_tail;
	unsigned int m_length;
	int m_level;
};
