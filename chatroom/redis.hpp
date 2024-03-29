/*
typedef struct redisReply
{
    //命令执行结果的返回类型
    int type; //REDIS_REPLY
    //存储执行结果返回为整数
    long long integer; // The integer when type is REDIS_REPLY_INTEGER
    //字符串值的长度
    size_t len; //Length of string
    //存储命令执行结果返回是字符串
    char *str; // Used for both REDIS_REPLY_ERROR and REDIS_REPLY_STRING
    //返回结果是数组的大小
    size_t elements; // number of elements, for REDIS_REPLY_ARRAY
    //存储执行结果返回是数组
    struct redisReply **element; // elements vector for REDIS_REPLY_ARRAY
} redisReply;

REDIS_REPLY_STRING == 1:返回值是字符串,字符串储存在redis->str当中,字符串长度为redis->len。
REDIS_REPLY_ARRAY == 2：返回值是数组，数组大小存在redis->elements里面，数组值存储在redis->element[i]里面。
数组里面存储的是指向redisReply的指针，数组里面的返回值可以通过redis->element[i]->str来访问，
数组的结果里全是type==REDIS_REPLY_STRING的redisReply对象指针。
REDIS_REPLY_INTEGER == 3：返回值为整数 long long。
REDIS_REPLY_NIL==4：返回值为空表示执行结果为空。
REDIS_REPLY_STATUS ==5：返回命令执行的状态，比如set foo bar 返回的状态为OK，
存储在str当中 reply->str == "OK"。
REDIS_REPLY_ERROR ==6 ：命令执行错误,错误信息存放在 reply->str当中。

set key value : 返回 string reply->str == "OK"。
hset key hkey value:返回 integer reply->integer == 1。
hdel key hkey:返回 integer reply->integer ==1。
sadd set member:返回 integer reply->integer ==1。
sismember setkey member:返回 integer reply->integer ==1。
smembers setkey:返回 array reply->element[i]->str为返回结果值。

*/

#ifndef __REDIS2_H
#define __REDIS2_H
#include <hiredis/hiredis.h>
#include <string>
#include <cstring>
#include <iostream>
using namespace std;

enum
{
    M_REDIS_OK = 0,          //执行成功
    M_CONNECT_FAIL = -1,     //连接redis失败
    M_CONTEXT_ERROR = -2,    // RedisContext返回错误
    M_REPLY_ERROR = -3,      // redisReply错误
    M_EXE_COMMAND_ERROR = -4 // redis命令执行错误
};

class Redis
{
public:
    Redis();
    ~Redis();
    int RedisConnect(const string &addr, int port, const string &pwd); //连接redis数据库：addr：IP地址，port：端口号，pwd：密码(默认为空)
    int disConnect();                                                  //断开连接
    redisReply *setValue(const string &key, const string &value);      //添加或修改键值对，成功返回0，失败<0
    redisReply *getValue(const string &key);                           //获取键对应的值，成功返回0，失败<0
    redisReply *exists(const string &key);                             //判断一个Key是否存在
    redisReply *delKey(const string &key);                             //删除键，成功返回影响的行数，失败<0

    redisReply *hsetValue(const string &key, const string &field, const string &value); //插入哈希表
    redisReply *hsetexist(const string &key, const string &field);                     //查看是否存在，存在返回1，不存在返回0
    redisReply *hgethash(const string &key, const string &field);                       //获取对应的hash_value
    redisReply *hashdel(const string &key, const string &field);                        //从哈希表删除指定的元素
    redisReply *hgethashall(const string &key);                                         //获取哈希中所有元素

    //对list进行操作
    redisReply *listlpush(const string &key, const string &value);
    redisReply *listrpush(const string &key, const string &value);
    redisReply *listlpop(const string &key);
    redisReply *listrpop(const string &key);
    redisReply *listlrem(const string &key, const string &len, const string &value);
    redisReply *listlen(const string &key);
    redisReply *listlrange(const string &key);                     //返回所有消息
    redisReply *listlrange(const string &key, string a, string b); //返回指定的消息记录
    redisReply *listltrim(const string &key);                      //删除链表中的所有元素
    redisReply *getredisReply();

private:
    string m_addr;        // IP地址
    int m_port;           //端口号
    string m_pwd;         //密码
    redisContext *pm_rct; // redis结构体
    redisReply *pm_rr;    //返回结构体
    string error_msg;     //错误信息

    // int connectAuth(const string &pwd); //使用密码登录
    int handleReply(void *value = NULL, redisReply ***array = NULL); //处理返回的结果
};

Redis::Redis()
{
    m_addr = "";
    m_port = 0;
    m_pwd = "";
    pm_rct = NULL;
    pm_rr = NULL;
    error_msg = "";
}
Redis::~Redis()
{
    disConnect();
    pm_rct = NULL;
    pm_rr = NULL;
}
redisReply *Redis::getredisReply()
{
    return pm_rr;
}

//连接数据库
//失败返回M_CONNECT_FAIL
int Redis::RedisConnect(const string &addr = "127.0.0.1", int port = 6379, const string &pwd = "")
{
    m_addr = addr;
    m_port = port;
    m_pwd = pwd;
    pm_rct = redisConnect(m_addr.c_str(), m_port);
    if (pm_rct->err)
    {
        error_msg = pm_rct->errstr;
        return M_CONNECT_FAIL;
    }
    return M_REDIS_OK;
}

//断开链接
int Redis::disConnect()
{
    redisFree(pm_rct);
    return 1;
}

redisReply *Redis::setValue(const string &key, const string &value)
{
    string cmd = "set  " + key + "  " + value;
    pm_rr = (redisReply *)redisCommand(pm_rct, cmd.c_str());
    return pm_rr;
}

redisReply *Redis::getValue(const string &key) //获取键对应的值，成功返回0，失败<0
{
    string cmd = "get  " + key;
    pm_rr = (redisReply *)redisCommand(pm_rct, cmd.c_str());
    return pm_rr;
}

redisReply *Redis::exists(const string &key)
{
    string cmd = "exists " + key;
    pm_rr = (redisReply *)redisCommand(pm_rct, cmd.c_str());
    return pm_rr;
}

redisReply *Redis::delKey(const string &key)
{
    string cmd = "del  " + key;
    pm_rr = (redisReply *)redisCommand(pm_rct, cmd.c_str());
    return pm_rr;
}

redisReply *Redis::hsetValue(const string &key, const string &field, const string &value) //插入哈希表
{
    string cmd = "hset  " + key + " " + field + " " + value;
    pm_rr = (redisReply *)redisCommand(pm_rct, cmd.c_str());
    return pm_rr;
}

redisReply *Redis::hsetexist(const string &key, const string &field) //查看是否存在，存在返回1，不存在返回0
{
    string cmd = "hexists  " + key + "  " + field;
    pm_rr = (redisReply *)redisCommand(pm_rct, cmd.c_str());
    return pm_rr;
}

redisReply *Redis::hgethash(const string &key, const string &field) //获取对应的hash_value
{
    string cmd = "hget  " + key + "  " + field;
    pm_rr = (redisReply *)redisCommand(pm_rct, cmd.c_str());
    return pm_rr;
}

redisReply *Redis::hashdel(const string &key, const string &field) //从哈希表删除指定的元素
{
    string cmd = "hdel  " + key + "  " + field;
    pm_rr = (redisReply *)redisCommand(pm_rct, cmd.c_str());
    return pm_rr;
}

redisReply *Redis::hgethashall(const string &key) //遍历哈希中的所有元素
{
    string cmd = "hgetall " + key;
    pm_rr = (redisReply *)redisCommand(pm_rct, cmd.c_str());
    return pm_rr;
}

redisReply *Redis::listlpush(const string &key, const string &value)
{
    string cmd = "lpush  " + key + " " + value;
    pm_rr = (redisReply *)redisCommand(pm_rct, cmd.c_str());
    return pm_rr;
}

redisReply *Redis::listrpush(const string &key, const string &value)
{
    string cmd = "rpush  " + key + " " + value;
    pm_rr = (redisReply *)redisCommand(pm_rct, cmd.c_str());
    return pm_rr;
}

redisReply *Redis::listlpop(const string &key)
{
    string cmd = "lpop  " + key;
    pm_rr = (redisReply *)redisCommand(pm_rct, cmd.c_str());
    return pm_rr;
}
redisReply *Redis::listrpop(const string &key)
{
    string cmd = "rpop  " + key;
    pm_rr = (redisReply *)redisCommand(pm_rct, cmd.c_str());
    return pm_rr;
}

redisReply *Redis::listlrem(const string &key, const string &len, const string &value)
{
    string cmd = "lrem " + key + " " + len + " " + value;
    pm_rr = (redisReply *)redisCommand(pm_rct, cmd.c_str());
    return pm_rr;
}

redisReply *Redis::listlen(const string &key)
{
    string cmd = "llen  " + key;
    pm_rr = (redisReply *)redisCommand(pm_rct, cmd.c_str());
    return pm_rr;
}

redisReply *Redis::listlrange(const string &key) //返回所有消息
{
    string cmd = "lrange  " + key + "  0" + "  -1";
    pm_rr = (redisReply *)redisCommand(pm_rct, cmd.c_str());
    return pm_rr;
}

redisReply *Redis::listlrange(const string &key, string a, string b) //返回指定的消息记录
{
    string cmd = "lrange  " + key + "  " + a + "  " + b;
    pm_rr = (redisReply *)redisCommand(pm_rct, cmd.c_str());
    return pm_rr;
}

redisReply *Redis::listltrim(const string &key) //删除链表中的所有元素
{
    string cmd = "ltrim  " + key + " 1 " + " 0 ";
    pm_rr = (redisReply *)redisCommand(pm_rct, cmd.c_str());
    return pm_rr;
}

#endif