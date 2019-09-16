#include "base.h"
#include "localRedis.h"

void simpleTest();
void testWithMap();
void runTestRss(unsigned int keynum, int perSec, int hour);
void testClass();
void testZSet();

int main()
{
	//srand((unsigned int)time(nullptr)); // 固定随机值,方便查问题

	//simpleTest();
	//testWithMap();
	runTestRss(1000,1000,60*60);
	//testClass();
	//testZSet();

	//getchar();
	return 0;
}

void simpleTest()
{
	if(true) // 简单数据结构
	{
		localRedis r;
		//int ret ;

		r.hset("A","B",1,true);
		LOG("---");
		r.hset("A","B","a",true);
		LOG("---");
		r.set("A",1,true);
		LOG("---");
		r.del("A");
		r.printAllCount();
	}

	if(true) // zset
	{
		localRedis r;
		r.zadd("A", "A", 1);
		r.zadd("A", "B", 2);
		r.zadd("A", "C", 3);
		r.zadd("A", "D", 3);
		r.zadd("A", "E", 4);
		r.zadd("A", "F", 5);
		r.zadd("A", "J", 6);
		{
			r.zadd("A", "A", 4);
			r.zadd("A", "A", -4);
			unsigned int c = 0;
			r.zcount("A", 2, 5, c);
			LOG(c);
		}
		{
			double d = 0;
			r.zscore("A","C",d);
			LOG(d);
		}
		{
			unsigned int rank = 0;
			r.zrank("A","C",rank);
			LOG(rank);
		}
		{
			unsigned int rank = 0;
			r.zadd("A","Ddd",0.1);
			r.zrank("A","Ddd",rank);
			LOG(rank);
			rank=0;
			r.zrem("A","Ddd");
			r.zrank("A","Ddd",rank);
			LOG(rank);
		}
		{
			vector<zsdata> ret;
			r.zrangebyscore("A", 1, 5, ret);
			for(unsigned int i=0;i<ret.size();i++)
			{
				LOG("zrangebyscore:rank="<<ret[i].rank<<",key="<<ret[i].key<<",score="<<ret[i].score);
			}
		}
		{
			vector<zsdata> ret;
			r.zrange("A", 4, 6, ret);
			for(unsigned int i=0;i<ret.size();i++)
			{
				LOG("zrange:rank="<<ret[i].rank<<",key="<<ret[i].key<<",score="<<ret[i].score);
			}
		}
		r.del("A");
	}
}

void testWithMap()
{
	int insertNum = 10000;
	localRedis r;
	map<string,map<string,string>> dic;
	unordered_map<string,unordered_map<string,string>> hashdic;

	// 初始化字典
	map<string,string> insertMap;
	{
		unsigned long long begin = getTimeStampMsec();

		char str[16]={0};
		string key,field;
		for(int i=0;i<insertNum;i++)
		{
			sprintf(str,"%u",i);
			key = str;
			sprintf(str,"%u",i*2);
			field = str;
			insertMap.insert(make_pair(key,field));
		}

		unsigned long long end = getTimeStampMsec();
		LOG("init:use time="<<(end-begin)<<",rss="<<getRss());
	}
	
	LOG("------------INSERT-----------");
	// 插入localRedis
	{
		map<string,string>::iterator it = insertMap.begin();
		unsigned long long begin = getTimeStampMsec();

		while(it!=insertMap.end())
		{
			r.hset(it->first,it->second,"v");
			it++;
		}

		unsigned long long end = getTimeStampMsec();
		LOG("localRedis:insert:use time="<<(end-begin)<<",rss="<<getRss());
	}
	getRss();

	// 插入到map
	{
		map<string,string>::iterator it = insertMap.begin();
		unsigned long long begin = getTimeStampMsec();

		while(it!=insertMap.end())
		{
			map<string,string> tmp;
			tmp.insert(make_pair(it->second,"v"));
			dic.insert(make_pair(it->first,tmp));
			it++;
		}

		unsigned long long end = getTimeStampMsec();
		LOG("map:insert:use time="<<(end-begin)<<",rss="<<getRss());
	}
	getRss();

	// 插入到hashmap
	{
		map<string,string>::iterator it = insertMap.begin();
		unsigned long long begin = getTimeStampMsec();

		while(it!=insertMap.end())
		{
			unordered_map<string,string> tmp;
			tmp.insert(make_pair(it->second,"v"));
			hashdic.insert(make_pair(it->first,tmp));
			it++;
		}

		unsigned long long end = getTimeStampMsec();
		LOG("hashmap:insert:use time="<<(end-begin)<<",rss="<<getRss());
	}
	getRss();

	LOG("------------FIND-----------");
	// 查找localRedis
	{
		map<string,string>::iterator it = insertMap.begin();
		string retstr;
		unsigned long long begin = getTimeStampMsec();

		while(it!=insertMap.end())
		{
			r.hget(it->first,it->second,retstr);
			it++;
		}

		unsigned long long end = getTimeStampMsec();
		LOG("localRedis:find:use time="<<(end-begin)<<",rss="<<getRss());
	}
	getRss();

	// 查找map
	{
		map<string,string>::iterator it = insertMap.begin();
		map<string,map<string,string>>::iterator ittmp;
		unsigned long long begin = getTimeStampMsec();

		while(it!=insertMap.end())
		{
			ittmp = dic.find(it->first);
			if(ittmp!=dic.end())
				ittmp->second.find(it->second);
			it++;
		}

		unsigned long long end = getTimeStampMsec();
		LOG("map:find:use time="<<(end-begin)<<",rss="<<getRss());
	}
	getRss();

	// 查找hashmap
	{
		map<string,string>::iterator it = insertMap.begin();
		unordered_map<string,unordered_map<string,string>>::iterator ittmp;
		unsigned long long begin = getTimeStampMsec();

		while(it!=insertMap.end())
		{
			ittmp = hashdic.find(it->first);
			if(ittmp!=hashdic.end())
				ittmp->second.find(it->second);
			it++;
		}

		unsigned long long end = getTimeStampMsec();
		LOG("hashmap:find:use time="<<(end-begin)<<",rss="<<getRss());
	}
	getRss();

	LOG("------------DEL------------");
	// 删除localRedis
	{
		map<string,string>::iterator it = insertMap.begin();
		unsigned long long begin = getTimeStampMsec();

		while(it!=insertMap.end())
		{
			r.hdel(it->first,it->second);
			it++;
		}

		unsigned long long end = getTimeStampMsec();
		LOG("localRedis:del:use time="<<(end-begin)<<",rss="<<getRss());
	}
	getRss();

	// 删除map
	{
		map<string,string>::iterator it = insertMap.begin();
		map<string,map<string,string>>::iterator ittmp;
		map<string,string>::iterator ittmp2;
		unsigned long long begin = getTimeStampMsec();

		while(it!=insertMap.end())
		{
			ittmp = dic.find(it->first);
			if(ittmp!=dic.end())
			{
				ittmp2 = ittmp->second.find(it->second);
				if(ittmp2!=ittmp->second.end())
				{
					ittmp->second.erase(ittmp2);
				}
				if(ittmp->second.size()<=0)
				{
					dic.erase(ittmp);
				}
			}
			it++;
		}

		unsigned long long end = getTimeStampMsec();
		LOG("map:del:use time="<<(end-begin)<<",rss="<<getRss());
	}
	getRss();

	// 删除hashmap
	{
		map<string,string>::iterator it = insertMap.begin();
		unordered_map<string,unordered_map<string,string>>::iterator ittmp;
		unordered_map<string,string>::iterator ittmp2;
		unsigned long long begin = getTimeStampMsec();

		while(it!=insertMap.end())
		{
			ittmp = hashdic.find(it->first);
			if(ittmp!=hashdic.end())
			{
				ittmp2 = ittmp->second.find(it->second);
				if(ittmp2!=ittmp->second.end())
				{
					ittmp->second.erase(ittmp2);
				}
				if(ittmp->second.size()<=0)
				{
					hashdic.erase(ittmp);
				}
			}
			it++;
		}

		unsigned long long end = getTimeStampMsec();
		LOG("hashmap:del:use time="<<(end-begin)<<",rss="<<getRss());
	}
	getRss();

}

void runTestRss(unsigned int keynum, int perSec, int sec)
{
	#define RAND_FORCE	rand()%2 == 0? true:false
	#define RAND_STR	dic[rand()%keynum]
	#define RAND_CLASS	dicClass[rand()%3]
	#define RAND_NUM	rand()%keynum

	struct A : public BaseDataPtr
	{
		int _a;
		string _b;
		A()
		{
			_a = 10; 
			_b = "123";
		}
		A(const A &a)
		{
			_a = a._a;
			_b = a._b;
		}
		~A()
		{
		}
	};

	struct B : public BaseDataPtr
	{
		vector<int> _v;
		B()
		{ 
			_v.push_back(rand());
		}
		B(const B &b)
		{
			_v = b._v;
		}
		~B()
		{
		}
	};
	
	struct C : public BaseDataPtr
	{
		char *_p;
		C()
		{
			_p = new char[10];
			memset(_p,0,10);
		}
		C(const C &c)
		{
			_p = new char[10];
			memcpy(_p,c._p,sizeof(10));
		}
		~C()
		{
			delete[] _p;
		}
	};

	localRedis r;
	int all = sec;

	// 初始化字典
	vector<string> dic;
	char k[16] = {0};
	for(unsigned int i=0;i<keynum;i++)
	{
		sprintf(k,"%u",i);
		dic.push_back(k);
	}
	
	vector<BaseDataPtr> dicClass;
	A a;
	B b;
	C c;
	dicClass.push_back(a);
	dicClass.push_back(b);
	dicClass.push_back(c);

	int setC=0,getC=0,delC=0,hsetC=0,hgetC=0,hdelC=0,setClass=0,getClass=0;
	int zadd=0,zcard=0,zcount=0,zscore=0,zrank=0,zrem=0,zrange=0,zrangebyscore=0;
	int rCmd=0;
	unsigned int ret;
	map<unsigned int,int> retMap;
	string retS;
	unsigned int retI;
	unsigned long retL;
	double retD;
	BaseDataPtr* retObj;
	vector<zsdata> retZSDL;
	for(unsigned int i=0;i<(unsigned int)all;i++)
	{
		retMap.clear();
		setC=0,getC=0,delC=0,hsetC=0,hgetC=0,hdelC=0;setClass=0;getClass=0;
		zadd=0,zcard=0,zcount=0,zscore=0,zrank=0,zrem=0,zrange=0,zrangebyscore=0;
		unsigned long long begin = getTimeStampMsec();
		for(unsigned int j=0;j<(unsigned int)perSec;j++)
		{
			rCmd = rand()%16;

			switch (rCmd)
			{
			case 0: //set
				ret = r.set(RAND_STR, RAND_STR, RAND_FORCE);
				setC++;
				break;
			case 1: // get
				ret = r.get(RAND_STR, retS);
				getC++;
				break;
			case 2: // del
				ret = r.del(RAND_STR);
				delC++;
				break;
			case 3: // hset
				ret = r.hset(RAND_STR, RAND_STR, RAND_STR,RAND_FORCE);
				hsetC++;
				break;
			case 4: // hget
				ret = r.hget(RAND_STR, RAND_STR, retS);
				hgetC++;
				break;
			case 5: //hdel
				ret = r.hdel(RAND_STR, RAND_STR);
				hdelC++;
				break;
			case 6: // setClass
				ret = r.setClass(RAND_STR, RAND_CLASS, RAND_FORCE);
				setClass++;
				break;
			case 7: // getClass
				ret = r.getClass(RAND_STR, &retObj);
				getClass++;
				break;
			case 8: // zadd
				ret = r.zadd(RAND_STR, RAND_STR, RAND_NUM);
				zadd++;
				break;
			case 9: // zcard
				ret = r.zcard(RAND_STR, retL);
				zcard++;
				break;
			case 10: // zcount
				ret = r.zcount(RAND_STR, RAND_NUM, RAND_NUM, retI);
				zcount++;
				break;
			case 11: // zscore
				ret = r.zscore(RAND_STR, RAND_STR, retD);
				zscore++;
				break;
			case 12: // zrank
				ret = r.zrank(RAND_STR, RAND_STR, retI);
				zrank++;
				break;
			case 13: // zrem
				ret = r.zrem(RAND_STR, RAND_STR);
				zrem++;
				break;
			case 14: // zrange
				ret = r.zrange(RAND_STR, RAND_NUM, RAND_NUM, retZSDL);
				zrange++;
				break;
			case 15: // zrangebyscore
				ret = r.zrangebyscore(RAND_STR, RAND_NUM, RAND_NUM, retZSDL);
				zrangebyscore++;
				break;
			default:
				break;
			}
			retMap[ret]++;
		}
		unsigned long long end = getTimeStampMsec();
		LOG("set=" << setC << ",get=" << getC << ",del=" << delC << ",hset=" << hsetC << ",hget=" << hgetC << ",hdel=" << hdelC <<",setClass="<<setClass<<",getClass="<<getClass);
		LOG("	--zadd="<<zadd<<",zcard="<<zcard<<",zcount="<<zcount<<",zscore="<<zscore<<",zrank="<<zrank<<",zrem="<<zrem<<",zrange="<<zrange<<",zrangebyscore="<<zrangebyscore);
		LOG("	--rss=" << getRss() << ",usetime=" << (end - begin) << ",used=" << r.getAllUsed()<<",size="<<r.getAllSize());
		__sleep(1.0);
	}
	r.printAllCount();
}

void testClass()
{
	struct A : public BaseDataPtr
	{
		int _a;
		string _b;
		A()
		{
			_a = 10; 
			_b = "123";
			LOG("new a:"<<this);
		}
		A(const A &a)
		{
			_a = a._a;
			_b = a._b;
			LOG("copy a:"<<this<<",other="<<(int*)&a);
		}
		~A()
		{
			LOG("delete a:"<<this);
		}
	};

	struct B : public BaseDataPtr
	{
		vector<int> _v;
		B()
		{ 
			_v.push_back(rand());
			LOG("new b:"<<this);
		}
		B(const B &b)
		{
			_v = b._v;
			LOG("copy b:"<<this<<",other="<<(int*)&b);
		}
		~B()
		{
			LOG("delete b:"<<this);
		}
	};
	
	struct C : public BaseDataPtr
	{
		char *_p;
		C()
		{
			_p = new char[10];
			memset(_p,0,10);
			LOG("new c:"<<this<<",p:"<<(int*)_p);
		}
		C(const C &c)
		{
			_p = new char[10];
			memcpy(_p,c._p,sizeof(10));
			LOG("copy c:"<<this<<",other="<<(int*)&c);
		}
		~C()
		{
			LOG("delete c:"<<this<<",p:"<<(int*)_p);
			delete[] _p;
		}
	};

	{
		localRedis r;

		{
			// __new_obj_2(A,a,10,"abc");
			// __new_obj(B,b);
			// __new_obj(B,c);
			// r.set("A", (BaseDataPtr*)a);
			// r.set("A", (BaseDataPtr*)b);
			// r.set("B", (BaseDataPtr*)c);

			A a;
			B b,b2;
			C c;
			LOG("-------------");
			r.setClass("A",a);
			r.del("A");
			r.setClass("A",b);
			r.setClass("A",c);
			r.setClass("A",b);
			r.setClass("A",b2);
			r.setClass("A",c);
			r.setClass("A",b);
			r.setClass("A",c);
		}

		{
			BaseDataPtr* get = nullptr;
			r.getClass("A", &get);
			A* reta = dynamic_cast<A*>(get);
			if(reta) {LOG("get A,"<<reta<<","<<reta->_a<<","<<reta->_b);}

			B* retb = dynamic_cast<B*>(get);
			if(retb) {retb->_v.push_back(3);LOG("get B,"<<retb<<",size="<<retb->_v.size());}

			r.getClass("A", &get);
			retb = dynamic_cast<B*>(get);
			if(retb) {retb->_v.push_back(3);LOG("get B,"<<retb<<",size="<<retb->_v.size());}
		}
		r.printAllCount();
	}
}

void testZSet()
{
	localRedis r;
	unsigned long begin = 0;
	unsigned long end = 0;
	char key[1024] = {0};
	vector<zsdata> ret;

	{ // 初始化
		begin = getTimeStampMsec();
		for(int i=0;i<10000;i++)
		{
			sprintf(key,"%d", i);
			r.zadd("test", key, (double)i/100);
		}
		end = getTimeStampMsec();
		unsigned int count=0;
		r.zcount("test", 0, 10000, count);
		LOG("zadd:first:usetime="<<end-begin<<",count="<<count);
	}

	{ // 排名范围
		int teststart = 2;
		int testend = 2650;

		begin = getTimeStampMsec();
		r.zrange("test", teststart, testend, ret);
		end = getTimeStampMsec();

		// 检查结果对错
		bool flag = false;
		if(ret.size() == (unsigned int)(testend-teststart+1))
		{
			unsigned int i=0;
			for(;i<ret.size();i++)
			{
				if(ret[i].rank != (teststart+i)
					|| ret[i].score != (double)(teststart+i-1)/100)
				{
					flag = false;
					break;
				}
			}
			if(i==ret.size())
				flag = true;
		}
		LOG("zrange:start="<<teststart<<",end="<<testend<<",flag="<<(flag?"true":"flase")<<",usetime="<<end-begin);
	}
}

