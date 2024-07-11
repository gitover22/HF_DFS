#ifndef _UTIL_CGI_H_
#define _UTIL_CGI_H_

#define FILE_NAME_LEN (256)    // 文件名字长度
#define TEMP_BUF_MAX_LEN (512) // 临时缓冲区大小
#define FILE_URL_LEN (512)     // 文件所存放storage的host_name长度
#define HOST_NAME_LEN (30)     // 主机ip地址长度
#define USER_NAME_LEN (128)    // 用户名字长度
#define TOKEN_LEN (128)        // 登陆token长度
#define MD5_LEN (256)          // 文件md5长度
#define PWD_LEN (256)          // 密码长度
#define TIME_STRING_LEN (25)   // 时间戳长度
#define SUFFIX_LEN (8)         // 后缀名长度
#define PIC_NAME_LEN (10)      // 图片资源名字长度
#define PIC_URL_LEN (256)      // 图片资源url名字长度

#define UTIL_LOG_MODULE "cgi"
#define UTIL_LOG_PROC "util"

/**
 * @brief  去掉一个字符串两边的空白字符
 * @param inbuf [out] 待处理字符串
 * @returns 成功:0
 */
int trim_space(char *inbuf);

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
char *memstr(char *full_data, int full_data_len, char *substr);

/**
 * @brief 解析查询字符串中的键值对,从给定的查询字符串中提取指定键对应的值
 * @param query 查询字符串，包含键值对
 * @param key 要查找的键
 * @param value [out] 用于存储找到的键对应的值的缓冲区
 * @param value_len_p [out] 指向一个整数的指针，用于存储找到的值的长度（不包括终止符'\0'）
 * @return 如果找到键并成功提取值，则返回0；如果未找到键，则返回-1。
 */
int query_parse_key_value(const char *query, const char *key, char *value, int *value_len_p);

/**
 * @brief 获取文件名的后缀
 * @param file_name [in] 文件名字符串
 * @param suffix [out] 用于存储文件名后缀的字符数组
 * @return 如果文件名存在后缀，则返回0；如果文件名不存在后缀或参数不合法，则返回-1。
 */
int get_file_suffix(const char *file_name, char *suffix);

/**
 * @brief 替换strSrc中的strFind为strReplace
 */
void str_replace(char *strSrc, char *strFind, char *strReplace);

// 返回前端情况，NULL代表失败, 返回的指针不为空，则需要free
char *return_status(char *status_num);

/**
 * @brief 验证用户令牌的有效性。
 * @param user [in] 用户名，用于在Redis中查找对应的令牌。
 * @param token [in] 用户提供的令牌，用于与Redis中存储的令牌进行比较。
 * @return 0表示令牌验证成功，-1表示令牌验证失败或Redis连接出错。
 */
int verify_token(char *user, char *token);

#endif
