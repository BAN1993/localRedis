#pragma once

#include "base.h"
#include "Define.h"

#include "valueType.h"
#include "dataNode.h"
#include "dataBase.h"

class localRedis
{
public:
	localRedis();
	~localRedis();

//-------------------
// 测试接口
//-------------------
public:
	void printAll();			// 打印整个db
	void printAllCount();		// 只打印数量
	unsigned int getAllUsed();	// 获取使用量
	unsigned int getAllSize();	// 大小

//-------------------
// 简单数据接口
//-------------------
public:
	unsigned int set(const std::string& key, const std::string& value, bool force=false);
	unsigned int set(const std::string& key, const int value, bool force=false);

	unsigned int get(const std::string& key, string& ret);
	unsigned int get(const std::string& key, int& ret);

	/*
	* 储存结构体接口,每次set都会拷贝并覆盖
	* 若要修改已有的内容,请直接修改get后的指针
	*/
	template <typename T>
	unsigned int setClass(const std::string& key, const T& value, bool force=false);
	unsigned int getClass(const std::string& key, BaseDataPtr** ret);

	unsigned int del(const std::string& key);

//-------------------
// hash表接口
//-------------------
public:
	unsigned int hset(const std::string& key, const std::string& field, const std::string& value, bool force=false);
	unsigned int hset(const std::string& key, const std::string& field, const int value, bool force=false);

	unsigned int hget(const std::string& key, const std::string& field, string& ret);
	unsigned int hget(const std::string& key, const std::string& field, int& ret);

	unsigned int hdel(const std::string& key, const std::string& field);

//-------------------
// zSet表接口
//-------------------
public:
	unsigned int zadd(const std::string& key, const std::string& field, const double value);	// 添加,不存在创建,存在叠加
	unsigned int zcard(const std::string& key, unsigned long& ret);								// 获取成员个数
	unsigned int zcount(const std::string& key, const double minScore,							// 获取区间内个数
						const double maxScore, unsigned int& ret);
	unsigned int zscore(const std::string& key, const std::string& field, double& score);		// 获取成员分值
	unsigned int zrank(const std::string& key, const std::string& field, unsigned int& rank);	// 获取成员排名
	unsigned int zrem(const std::string& key, const std::string& field);						// 删除成员
	unsigned int zrange(const std::string& key, const int start, const int end,					// 获取排名范围数据
						 vector<zsdata>& ret);
	unsigned int zrangebyscore(const std::string& key, const double minScore,					// 获取积分范围数据
								const double maxScore, vector<zsdata>& ret);
	
private:
	dataBase* _db;

};

template <typename T>
unsigned int localRedis::setClass(const std::string& key, const T& value, bool force)
{
	static_assert(std::is_base_of<BaseDataPtr,T>::value, "Class needs to be DataBasePtr based.");

	valueType* v = valueType::createClass(value);
	dataNode* node = dataNode::_create(key, v);

	return _db->set(node, force);
}

