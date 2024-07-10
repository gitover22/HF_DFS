/**
 * @file util_cgi.c
 * @brief  cgi后台通用接口
 */

#include <ctype.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "make_log.h"
#include "cJSON.h"
#include "cfg.h"
#include "util_cgi.h"
#include "redis_op.h"

int trim_space(char *inbuf)
{
    int i = 0;
    int j = strlen(inbuf) - 1;

    char *str = inbuf;

    int count = 0;

    if (str == NULL)
    {
        // LOG(UTIL_LOG_MODULE, UTIL_LOG_PROC, "inbuf   == NULL\n");
        return -1;
    }

    while (isspace(str[i]) && str[i] != '\0')
    {
        i++;
    }

    while (isspace(str[j]) && j > i)
    {
        j--;
    }

    count = j - i + 1;

    strncpy(inbuf, str + i, count);

    inbuf[count] = '\0';

    return 0;
}

/**
 * @brief  在字符串full_data中查找字符串substr第一次出现的位置
 *
 * @param full_data 	源字符串首地址
 * @param full_data_len 源字符串长度
 * @param substr        匹配字符串首地址
 *
 * @returns
 *      成功: 匹配字符串首地址
 *      失败：NULL
 */
char *memstr(char *full_data, int full_data_len, char *substr)
{
    // 异常处理
    if (full_data == NULL || full_data_len <= 0 || substr == NULL)
    {
        return NULL;
    }

    if (*substr == '\0')
    {
        return NULL;
    }

    // 匹配子串的长度
    int sublen = strlen(substr);

    int i;
    char *cur = full_data;
    int last_possible = full_data_len - sublen + 1; // 减去匹配子串后的长度
    for (i = 0; i < last_possible; i++)
    {
        if (*cur == *substr)
        {
            if (memcmp(cur, substr, sublen) == 0)
            {
                // found
                return cur;
            }
        }

        cur++;
    }

    return NULL;
}

/**
 * @brief 解析查询字符串中的键值对,从给定的查询字符串中提取指定键对应的值
 * 
 * @param query 查询字符串，包含键值对
 * @param key 要查找的键
 * @param value [out] 用于存储找到的键对应的值的缓冲区
 * @param value_len_p [out] 指向一个整数的指针，用于存储找到的值的长度（不包括终止符'\0'）
 * @return 如果找到键并成功提取值，则返回0；如果未找到键，则返回-1。
 */
int query_parse_key_value(const char *query, const char *key, char *value, int *value_len_p)
{
    char *temp = NULL;
    char *end = NULL;
    int value_len = 0;

    // 找到是否有key
    temp = strstr(query, key);
    if (temp == NULL)
    {
        // LOG(UTIL_LOG_MODULE, UTIL_LOG_PROC, "Can not find key %s in query\n", key);

        return -1;
    }

    temp += strlen(key); //=
    temp++;              // value

    // get value
    end = temp;

    while ('\0' != *end && '#' != *end && '&' != *end)
    {
        end++;
    }

    value_len = end - temp;

    strncpy(value, temp, value_len);
    value[value_len] = '\0';

    if (value_len_p != NULL)
    {
        *value_len_p = value_len;
    }

    return 0;
}

/**
 * @brief 获取文件名的后缀
 * @param file_name [in] 文件名字符串
 * @param suffix [out] 用于存储文件名后缀的字符数组
 * @return 如果文件名存在后缀，则返回0；如果文件名不存在后缀或参数不合法，则返回-1。
 */
int get_file_suffix(const char *file_name, char *suffix)
{
    const char *p = file_name;
    int len = 0;
    const char *q = NULL;
    const char *k = NULL;

    if (p == NULL)
    {
        return -1;
    }

    q = p;

    // mike.doc.png
    //              ↑

    while (*q != '\0')
    {
        q++;
    }

    k = q;
    while (*k != '.' && k != p)
    {
        k--;
    }

    if (*k == '.')
    {
        k++;
        len = q - k;

        if (len != 0)
        {
            strncpy(suffix, k, len);
            suffix[len] = '\0';
        }
        else
        {
            strncpy(suffix, "null", 5);
        }
    }
    else
    {
        strncpy(suffix, "null", 5);
    }

    return 0;
}

/**
 * @brief 替换strSrc中的strFind为strReplace
 */
void str_replace(char *strSrc, char *strFind, char *strReplace)
{
    while (*strSrc != '\0')
    {
        if (*strSrc == *strFind)
        {
            if (strncmp(strSrc, strFind, strlen(strFind)) == 0)
            {
                int i = 0;
                char *q = NULL;
                char *p = NULL;
                char *repl = NULL;
                int lastLen = 0;

                i = strlen(strFind);
                q = strSrc + i;
                p = q; // p、q均指向剩余字符串的首地址
                repl = strReplace;

                while (*q++ != '\0')
                    lastLen++;
                char *temp = (char *)malloc(lastLen + 1); // 临时开辟一段内存保存剩下的字符串,防止内存覆盖
                int k = 0;
                for (k = 0; k < lastLen; k++)
                {
                    *(temp + k) = *(p + k);
                }
                *(temp + lastLen) = '\0';
                while (*repl != '\0')
                {
                    *strSrc++ = *repl++;
                }
                p = strSrc;
                char *pTemp = temp; // 回收动态开辟内存
                while (*pTemp != '\0')
                {
                    *p++ = *pTemp++;
                }
                free(temp);
                *p = '\0';
            }
            else
                strSrc++;
        }
        else
            strSrc++;
    }
}

// 返回前端情况，NULL代表失败, 返回的指针不为空，则需要free
char *return_status(char *status_num)
{
    char *out = NULL;
    cJSON *root = cJSON_CreateObject();                // 创建json项目
    cJSON_AddStringToObject(root, "code", status_num); // {"code":"000"}
    out = cJSON_Print(root);                           // cJSON to string(char *)

    cJSON_Delete(root);

    return out;
}

/**
 * @brief 验证用户令牌的有效性。
 * @param user [in] 用户名，用于在Redis中查找对应的令牌。
 * @param token [in] 用户提供的令牌，用于与Redis中存储的令牌进行比较。
 * @return 0表示令牌验证成功，-1表示令牌验证失败或Redis连接出错。
 */
int verify_token(char *user, char *token)
{
    int ret = 0;
    redisContext *redis_conn = NULL;
    char tmp_token[128] = {0};

    // redis 服务器ip、端口
    char redis_ip[30] = {0};
    char redis_port[10] = {0};

    // 读取redis配置信息
    get_cfg_value(CFG_PATH, "redis", "ip", redis_ip);
    get_cfg_value(CFG_PATH, "redis", "port", redis_port);

    // 连接redis数据库
    redis_conn = rop_connectdb_nopwd(redis_ip, redis_port);
    if (redis_conn == NULL)
    {
        LOG(UTIL_LOG_MODULE, UTIL_LOG_PROC, "redis connected error\n");
        ret = -1;
        goto END;
    }

    // 获取user对应的value
    ret = rop_get_string(redis_conn, user, tmp_token);
    if (ret == 0)
    {
        if (strcmp(token, tmp_token) != 0) // token不相等
        {
            ret = -1;
        }
    }

END:
    if (redis_conn != NULL)
    {
        rop_disconnect(redis_conn);
    }

    return ret;
}