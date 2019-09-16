#include "dataBase.h"
#include "dataNode.h"

dataBase::dataBase()
{
	m_rehashidx = -1;
	m_used = 0;
}

dataBase::~dataBase()
{
}

dataBase* dataBase::_create()
{
	__new_obj(dataBase, db);
	return db;
}

void dataBase::_delete(dataBase* db)
{
	db->deleteHashTable();

	if(db)
		__delete_obj(dataBase, db);
}

unsigned int dataBase::set(dataNode* node, bool force)
{
	dataNode* exist = find(node->getKey());
	if(!exist)
		return add(node);

	// 已存在
	if(exist->getType() != node->getType() && !force) // 这里不判断value是否为空,理论上不应该发生,遇到直接crash吧
	{
		dataNode::_delete(node);
		node = nullptr;
		return retType::IS_EXIST_OTHER;
	}

	// TODO 感觉这样设计不好,效率不高
	del(exist->getKey());
	add(node);
	return retType::OK;
}

dataNode* dataBase::find(const std::string& key)
{
	if(m_ht[0].size==0)
		return nullptr;

	tryRehash();

	unsigned int hindex = hashKey(key.c_str(),key.length());
	unsigned int fixIndex = 0;
	for(unsigned int i=0;i<2;i++)
	{
		fixIndex = hindex & m_ht[i].sizemask;
		dataNode* node = m_ht[i].table[fixIndex];
		while(node)
		{
			if(node == nullptr)
				break;
			if(node->getKey() == key) // TODO 这里判断会有问题,记得改
				return node;
			node = node->getNext();
		}
		if(!isRehash())
			break;
	}
	return nullptr;
}

unsigned int dataBase::del(const std::string& key)
{
	if(m_used == 0)
		return retType::DB_NULL;

	tryRehash();

	unsigned int hindex = hashKey(key.c_str(), key.length());
	unsigned int fixIndex = 0;

	for(unsigned int i=0;i<2;i++)
	{
		fixIndex = hindex & m_ht[i].sizemask;
		dataNode* node = m_ht[i].table[fixIndex];
		dataNode* last = nullptr;
		while(node)
		{
			if(node==nullptr)
				break;
			if(node->getKey() == key)
			{
				if(last!=nullptr)
					last->setNext(node->getNext());
				else
					m_ht[i].table[fixIndex] = node->getNext();
				dataNode::_delete(node);
				node = nullptr;
				m_ht[i].used--;
				m_used--;
				return retType::OK;
			}
			last = node;
			node = node->getNext();

		}
		if(!isRehash())
			break;
	}
	return retType::NO_KEY;
}

unsigned int dataBase::add(dataNode* node)
{
	tryExpand();

	hashTable* h = nullptr;
	if(isRehash())
		h = &m_ht[1];
	else
		h = &m_ht[0];

	unsigned int hindex = hashKey(node->getKey(),strlen(node->getKey()));
	hindex = hindex & h->sizemask;

	node->setNext(h->table[hindex]);
	h->table[hindex] = node;
	h->used++;
	m_used++;
	return retType::OK;
}

void dataBase::print(string fix)
{
	LOG(fix<<"--------------------------------------------------------");
	LOG(fix<<"begin print db="<<this);
	LOG(fix<<"--------------------------------------------------------");
	for(unsigned int i=0;i<2;i++)
	{
		LOG(fix<<"ht="<<i<<",size="<<m_ht[i].size<<",used="<<m_ht[i].used);
		for(unsigned int j=0;j<m_ht[i].size;j++)
		{
			dataNode* node = m_ht[i].table[j];
			if(node==nullptr)
			{
				LOG(fix<<"[[table="<<j<<"]] is null");
			}
			else
			{
				while(node)
				{
					if(node->getType()==VALUE_TYPE_HASHTABLE)
					{
						LOG(fix<<"[[table="<<j<<"]] is hashtable,key="<<node->getKey()<<",value="<<node->getData()<<",next="<<node->getNext());
						((dataBase*)node->getData())->print(fix+"\t\t");
					}
					else
					{
						string value = (char*)node->getData();
						LOG(fix<<"**[[table="<<j<<"]]is string,key="<<node->getKey()<<",value="<<value<<",next="<<node->getNext());
					}
					node = node->getNext();
				}
			}
		}
	}
	LOG(fix<<"--------------------------------------------------------");
	LOG(fix<<"end print db="<<this);
	LOG(fix<<"--------------------------------------------------------");
}

void dataBase::printCount(string fix)
{
	for(unsigned int i=0;i<2;i++)
	{
		LOG(fix<<"[-]ht="<<i<<",size="<<m_ht[i].size<<",used="<<m_ht[i].used);
		for(unsigned int j=0;j<m_ht[i].size;j++)
		{
			int count=0;
			dataNode* node = m_ht[i].table[j];
			if(node!=nullptr)
			{
				while(node)
				{
					count++;
					if(node->getType() == VALUE_TYPE_HASHTABLE)
					{
						((dataBase*)node->getData())->printCount(fix+"\t\t");
					}
					node = node->getNext();
				}
			}
			LOG(fix<<"table["<<j<<"],count="<<count);
		}
	}
}

unsigned int dataBase::getUsed()
{
	return m_used;
}

unsigned int dataBase::getSize()
{
	return m_ht[0].size + m_ht[1].size;
}

unsigned int dataBase::tryExpand()
{
	if(isRehash())
		return retType::IS_REHASHING;

	if(m_ht[0].size == 0)
		return doExpand(HT_INIT_SIZE);

	if(m_ht[0].used>=m_ht[0].size) // TODO 先简单处理.一旦used大于size就扩展
		return doExpand(m_ht[0].used * 2);

	return retType::ERR_UNKNOWN;
}

unsigned int dataBase::doExpand(unsigned long size)
{
	if(isRehash())
		return retType::IS_REHASHING;

	hashTable h;
	h.size = size;
	h.sizemask = size-1;
	h.used = 0;
	h.table = (dataNode**)__new(size*sizeof(int*));

	if(m_ht[0].table == nullptr)
	{
		m_ht[0] = h;
		return retType::OK;
	}

	m_ht[1] = h;
	m_rehashidx = 0;
	return retType::OK;
}

void dataBase::deleteHashTable()
{
	for(int i=0;i<2;i++)
	{
		deleteHashTable(&m_ht[i]);
	}
}

void dataBase::deleteHashTable(hashTable* ht)
{
	if(ht->size>0)
	{
		for(unsigned int i=0;i<ht->size;i++)
		{
			dataNode* node = ht->table[i];
			while(node)
			{
				dataNode* tmp = node->getNext();
				dataNode::_delete(node);
				node = tmp;
			}
		}
		__delete(ht->table);
	}
	ht->table = nullptr;
}

bool dataBase::isRehash()
{
	return m_rehashidx == -1 ? false:true;
}

void dataBase::tryRehash()
{
	if(!isRehash())
		return;
	if(m_ht[1].size<=0)
		return;
	doRehash();
}

void dataBase::doRehash()
{
	unsigned int index = 0;
	
	// 如果doRehash完成,则将h1置为h
	if(m_ht[0].used<=0)
	{
		deleteHashTable(&m_ht[0]);

		m_ht[0] = m_ht[1];

		m_ht[1].size=0;
		m_ht[1].used=0;
		m_ht[1].sizemask=0;
		m_ht[1].table=nullptr;

		m_rehashidx = -1;
		return;
	}

	dataNode* node = nullptr;
	dataNode* next = nullptr;

	while(m_ht[0].table[m_rehashidx] == nullptr) m_rehashidx++;

	node = m_ht[0].table[m_rehashidx];
	while(node)
	{
		next = node->getNext();
		index = hashKey(node->getKey(),strlen(node->getKey())) & m_ht[1].sizemask;
		node->setNext(m_ht[1].table[index]);
		m_ht[1].table[index] = node;
		m_ht[0].used--;
		m_ht[1].used++;

		node = next;
	}

	m_ht[0].table[m_rehashidx] = nullptr;
	m_rehashidx++;
}

unsigned int dataBase::hashKey(const void* key, int len)
{
	// 完全拷贝的,使用 MurmurHash2 算法
	unsigned int seed = 5381;
    const unsigned int m = 0x5bd1e995;
    const int r = 24;

    /* Initialize the hash to a 'random' value */
    unsigned int h = seed ^ len;

    /* Mix 4 bytes at a time into the hash */
    const unsigned char *data = (const unsigned char *)key;

    while(len >= 4) {
        unsigned int k = *(unsigned int*)data;

        k *= m;
        k ^= k >> r;
        k *= m;

        h *= m;
        h ^= k;

        data += 4;
        len -= 4;
    }

    /* Handle the last few bytes of the input array  */
    switch(len) {
    case 3: h ^= data[2] << 16;
    case 2: h ^= data[1] << 8;
    case 1: h ^= data[0]; h *= m;
    };

    /* Do a few final mixes of the hash to ensure the last few
     * bytes are well-incorporated. */
    h ^= h >> 13;
    h *= m;
    h ^= h >> 15;

    return (unsigned int)h;
}
