#pragma once

/************************************************************************
 * 注意项:
 *		-> 非线程安全
 *		-> 没有超时机制
 *		-> 若要储存结构体请继承自BaseDataPtr,注意浅拷贝问题
 *
 *		-> TODO 
 *		->		很多地方没有考虑失败或做安全检查,方便前期定位问题
 *		->		支持的数据类型很少
 *
 *
 *													add by jxc 2019.9.6
************************************************************************/

#include "base.h"
#include "mallocManager.h"

#define HT_INIT_SIZE		4 // hashTable初始化时分配的数量

// 数据类型
#define VALUE_TYPE_UNKNOWN		0 // 未定义
#define VALUE_TYPE_HASHTABLE	1 // hashtable
#define VALUE_TYPE_ZSET			2 // zset
#define VALUE_TYPE_STRING		3 // 字符串
#define VALUE_TYPE_CLASS		4 // 类
#define VALUE_TYPE_INT			5 // int
#define VALUE_TYPE_DOUBLE		6 // double

// skiplist相关定义
#define SKIPLIST_MAX_LEVEL	32		// 最大层数
#define SKIPLIST_RAND		0.25	// 扩展层时,每次往上的概率

// 返回枚举
enum retType
{
	OK=0,			// 成功
	ERR_UNKNOWN,	// 未知错误
	TYPE_ERR,		// 类型错误
	SET_ERR,		// set失败
	DB_NULL,		// 数据库为空
	RANGE_ERR,		// 范围错误

	NO_KEY,			// 不存在key
	NO_FIELD,		// 不存在field
	NO_VALUE,		// 没有数据

	IS_EXIST_OTHER,	// 已存在,且类型不一致
	IS_REHASHING,	// 正在执行doRehash
	IS_EXIST,		// 已存在

	MAX_RET_CNT		// end
};

/************************************************************************
 * 基础数据结构体
 * 若要储存结构体,需要继承自此
************************************************************************/
struct BaseDataPtr
{
	virtual ~BaseDataPtr(){}
};

/************************************************************************
 * zset返回的顺序数据
 * 
************************************************************************/
struct zsdata
{
	unsigned int rank;
	std::string key;
	double score;
};
