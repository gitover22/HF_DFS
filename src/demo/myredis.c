/**
 * @brief redis客户端程序示例
 */
#include <stdio.h>
#include <hiredis.h> // 引入hiredis头文件
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main(int argc, const char* argv[])
{
    if(argc < 2)
    {
        printf("%s port\n", argv[1]);
        exit(1);
    }

    // 连接
    redisContext * res = redisConnect("127.0.0.1", atoi(argv[1]));
    if(res == NULL)
    {
        printf("redis server connect fail...\n");
        exit(1);
    }

    // 测试ping
    redisReply* ret = NULL;
    ret = redisCommand(res, "ping");
    freeReplyObject(ret);

    // 执行
    ret = redisCommand(res, "hset user name %s", "kevin");
    freeReplyObject(ret);
    ret = redisCommand(res, "hset user age %d", 34);
    freeReplyObject(ret);
    ret = redisCommand(res, "hset user passwd %s", "123abc");
    freeReplyObject(ret);
    // 释放
    redisFree(res);

    return 0;
}
