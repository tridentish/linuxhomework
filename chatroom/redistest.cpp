#include <iostream>
#include "redis1.hpp"
using namespace std;

void testhgetall()
{
    redisContext *c = Redis::RedisConnect("127.0.0.1", 6379);
    redisReply *r = Redis::hgethashall(c, "account");
    if (r == NULL)
    {
        printf("Execut getValue failure\n");
        redisFree(c);
        return;
    }
    for (int i = 0; i < r->elements; ++i)
        cout << r->element[i]->str << endl;

    freeReplyObject(r);

    r = Redis::hgethash(c, "account", "123");
    if (r->type != REDIS_REPLY_STRING)
    {
        printf("Execut hgethash failure\n");
        cout << "2" << endl;
        freeReplyObject(r);
        redisFree(c);
        return;
    }
    cout << r->str << endl;
}

void lrangetest()
{
    string a = "123";
    redisContext *c = Redis::RedisConnect("127.0.0.1", 6379);
    redisReply *r = Redis::listlrange(c, a + "history" + "1", "0", "-1");
    cout << a + "history" + "1" << endl;
    cout << 1 << endl;
    if (r == NULL)
    {
        printf("Execut getValue failure\n");
        redisFree(c);
        return;
    }
    for (int i = 0; i < r->elements; ++i)
    {
        cout << r->element[i]->str << endl;
        cout << "####################################" << endl;
    }
}

int main(void)
{
    redisContext *c = Redis::RedisConnect("127.0.0.1", 6379);

    // redisReply *r = Redis::setValue(c, "aaa", "abc");
    // r = Redis::getValue(c, "aaa");
    // if (r == NULL)
    //     printf("GET ");
    // if (r->type != REDIS_REPLY_STRING)
    // {
    //     printf("Execut getValue failure\n");
    //     freeReplyObject(r);
    //     redisFree(c);
    // }
    // printf("%s\n", r->str);

    // r = Redis::existsValue(c, "aaa");
    // if (r == NULL)
    //     printf("GET ");
    // printf("%d", r->integer);

    redisReply *r = Redis::hsetValue(c, "info", "123", "abc");

    if (r == NULL)
        printf("hsetValue ");
    if (r->type != REDIS_REPLY_INTEGER) //返回值是否时一个整数
    {
        printf("Execut getValue failure\n");
        freeReplyObject(r);
        redisFree(c);
    }
    cout << r->integer << endl;

    freeReplyObject(r);

    r = Redis::hgethash(c, "info", "123");
    if (r == NULL)
    {
        cout << "111";
        redisFree(c);
    }
    if (r->type != REDIS_REPLY_STRING)
    {
        printf("Execut getValue failure\n");
        freeReplyObject(r);
        redisFree(c);
        return 0;
    }
    cout << r->str << endl;

    freeReplyObject(r);

    r = Redis::hsetexist(c, "info", "123");
    if (r->type != REDIS_REPLY_INTEGER) //返回值是否时一个整数
    {
        printf("Execut getValue failure\n");
        freeReplyObject(r);
        redisFree(c);
    }
    cout << r->integer << endl;
    freeReplyObject(r);

    redisFree(c);

    // testhgetall();
    lrangetest();

    return 0;
}