#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <hiredis.h>

int main(int argc, char **argv) {
    redisContext *c;
    redisReply *reply;
    const char *hostname = (argc > 1) ? argv[1] : "127.0.0.1";
    int port = (argc > 2) ? atoi(argv[2]) : 6379;

    struct timeval timeout = { 5, 500000 }; // 5.5 seconds
    c = redisConnectWithTimeout(hostname, port, timeout);
    if (c == NULL || c->err) {
        if (c) {
            printf("Connection error: %s\n", c->errstr);
            redisFree(c);
        } else {
            printf("Connection error: can't allocate redis context\n");
        }
        exit(1);
    }

    /* PING server */
    reply = (redisReply *)redisCommand(c,"HMSET hashTable1 userName zou passwd 123456 age 23");

    freeReplyObject(reply);

    /* Set a key */
    reply = (redisReply *)redisCommand(c,"HGETALL hashTable1");
    if(reply->type == REDIS_REPLY_ARRAY){
        for(int i =0 ;i<reply->elements;i += 2 ){
            printf("key: %s  value: %s \n",reply->element[i]->str,reply->element[i+1]->str);
        }
    }
    freeReplyObject(reply);

    /* Disconnects and frees the context */
    redisFree(c);

    return 0;
}
