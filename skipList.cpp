#include "skipList.h"
#include "dataNode.h"

skipList::skipList()
{

}

skipList::~skipList()
{

}

skipList* skipList::_create()
{
	__new_obj(skipList, p);

	p->m_level = 1;
	p->m_length = 0;
	p->m_head = skipNode::_create(SKIPLIST_MAX_LEVEL, 0, nullptr);
	for(int i=0;i<SKIPLIST_MAX_LEVEL;i++)
	{
		p->m_head->level[i].forward = nullptr;
		p->m_head->level[i].span = 0;
	}
	p->m_head->backward = nullptr;
	p->m_tail = nullptr;
	return p;
}

void skipList::_delete(skipList* list)
{
	skipNode* np = list->m_head;
	while (np)
	{
		skipNode* tmp = np->level[0].forward;
		skipNode::_delete(np);
		np = tmp;
	}

	__delete_obj(skipList, list);
}

unsigned long skipList::getLength()
{
	return m_length;
}

int skipList::getLevel()
{
	return m_level;
}

void skipList::deleteScore(double score, dataNode* node)
{
	skipNode* update[SKIPLIST_MAX_LEVEL];
	skipNode* np = m_head;

	for(int i=m_level-1;i>=0;i--)
	{
		while(np->level[i].forward
			&& (np->level[i].forward->score < score
				|| (np->level[i].forward->score == score
					&& np->level[i].forward->data->compare(node)<0)))
		{
			np = np->level[i].forward;
		}
		update[i] = np;
	}

	// 存在 && 积分相等 && 节点相同
	np = np->level[0].forward;
	if(np && np->score == score && np->data->compare(node) == 0)
	{
		for(int i=0;i<m_level;i++)
		{
			if(update[i]->level[i].forward == np)
			{
				update[i]->level[i].forward = np->level[i].forward;
				update[i]->level[i].span += np->level[i].span;
			}
			update[i]->level[i].span--;
		}
	}
	skipNode::_delete(np);
	np = nullptr;
	m_length--;
}

void skipList::insertScore(double score, dataNode* node)
{
	skipNode* update[SKIPLIST_MAX_LEVEL]; // 每一层遍历到的节点
	unsigned int distance[SKIPLIST_MAX_LEVEL]; // 距离
	skipNode* np =m_head; // 当前遍历的节点

	for(int i=m_level-1;i>=0;i--)
	{
		distance[i] = (i==m_level-1) ? 0 : distance[i+1];
		while(np->level[i].forward
			&& (np->level[i].forward->score < score
				|| (np->level[i].forward->score == score
					&& np->level[i].forward->data->compare(node)<0)))
		{
			distance[i] += np->level[i].span;
			np = np->level[i].forward;
		}
		update[i] = np;
	}

	// 扩充
	int newLevel = randLevel();
	if(newLevel > m_level && newLevel<SKIPLIST_MAX_LEVEL)
	{
		for(int i=m_level; i<newLevel; i++)
		{
			distance[i] = 0;
			update[i] = m_head;
			update[i]->level[i].span = m_length;
		}
		m_level = newLevel;
	}

	// 插入到中间
	np = skipNode::_create(newLevel,score,node);
	for(int i=0; i<newLevel; i++)
	{
		np->level[i].forward = update[i]->level[i].forward;
		update[i]->level[i].forward = np;

		np->level[i].span = update[i]->level[i].span - (distance[0] - distance[i]);
        update[i]->level[i].span = (distance[0] - distance[i]) + 1;
	}

	for (int i = newLevel; i < m_level; i++)
	{
        update[i]->level[i].span++;
    }

	np->backward = (update[0] == m_head) ? NULL : update[0];
    if (np->level[0].forward)
        np->level[0].forward->backward = np;
    else
        m_tail = np;
	m_length++;
}

int skipList::randLevel()
{
	int level = 1;
    while ((rand()&0xFFFF) < (SKIPLIST_RAND * 0xFFFF))
        level += 1;
	return (level<SKIPLIST_MAX_LEVEL) ? level : SKIPLIST_MAX_LEVEL;
}

unsigned int skipList::getRankByNode(dataNode* node)
{
	if(!node || !node->getValue() || node->getType() != VALUE_TYPE_DOUBLE)
		return 0;

	double score = (double)*(double*)node->getData();
	unsigned int distance = 0;
	skipNode* np = m_head;
	for(int i=m_level-1;i>=0;i--)
	{
		while( np->level[i].forward
			&& (np->level[i].forward->score < score
				|| (np->level[i].forward->score == score
					&& np->level[i].forward->data->compare(node)<0)))
		{
			distance += np->level[i].span;
			np = np->level[i].forward;
		}
	}
	distance++;
	np = np->level[0].forward;
	if(np->data->compare(node)==0)
		return distance;
	return 0;
}

skipNode* skipList::findScoreRangeFirst(double min, double max)
{
	skipNode* np = m_head;
	for(int i=m_level-1;i>=0;i--)
	{
		while(np->level[i].forward && np->level[i].forward->score < min)
			np = np->level[i].forward;
	}
	if(!np
		|| !np->level[0].forward
		|| np->level[0].forward->score > max)
		return nullptr;
	np = np->level[0].forward;
	return np;
}

skipNode* skipList::findScoreRangeLast(double min, double max)
{
	skipNode* np = m_head;
	for(int i=m_level-1;i>=0;i--)
	{
		while(np->level[i].forward && np->level[i].forward->score <= max)
			np = np->level[i].forward;
	}
	if(!np
		|| np == m_head
		|| np->score < min)
		return nullptr;
	return np;
}

skipNode* skipList::findRankRangeFirst(int start, int end)
{
	if((int)m_length < start)
		return nullptr;

	skipNode* np = m_head;
	int rank = 0;
	for(int i=m_level-1;i>=0;i--)
	{
		while(np && (int)np->level[i].span+rank <= start)
		{
			rank += (int)np->level[i].span;
			np = np->level[i].forward;
		}
		if(rank == start)
			return np;
	}
	return nullptr;
}

skipNode* skipList::findRankRangeLast(int start, int end)
{
	if((int)m_length < start)
		return nullptr;
	if((int)m_length <= end)
		return m_tail;

	skipNode* np = m_head;
	int rank = 0;
	for(int i=m_level-1;i>=0;i--)
	{
		while(np && (int)np->level[i].span+rank <= end)
		{
			rank += (int)np->level[i].span;
			np = np->level[i].forward;
		}
		if(rank == end)
			return np;
	}
	return  nullptr;
}

void skipList::print()
{
	LOG("---------------------------------------------------------------");
	char line[1024]={0};
	skipNode* node = m_head;
	for(int i=m_level-1;i>=0;i--)
	{
		node = m_head;
		unsigned long tmplen = 0;
		int jump = node->level[i].span;
		sprintf(line,"[%d]{H,%d}", i, node->level[i].span);
		while(tmplen<m_length && node)
		{
			while(jump>0)
			{
				jump--;
				sprintf(line,"%s\t", line);
			}
			tmplen+=jump;
			node = node->level[i].forward;
			if(node)
			{
				sprintf(line,"%s{%d,%d}", line, (int)node->score, node->level[i].span);
				jump = node->level[i].span;
			}
		}
		LOG(line);
	}
	LOG("---------------------------------------------------------------");
}
