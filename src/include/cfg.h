#ifndef _CFG_H_
#define _CFG_H_

#define CFG_PATH    "/home/huafeng/HF_DFS/src/conf/cfg.json" //配置文件路径

#define CFG_LOG_MODULE "cgi"
#define CFG_LOG_PROC   "cfg"

/**
 * @brief  从配置文件中得到相对应的参数
 *
 * @param profile   [in]         配置文件路径
 * @param title     [in]         配置文件title名称
 * @param key       [in]         key
 * @param value     [out]        得到的value
 *
 * @return
 *      0 succ, -1 fail
 */
extern int get_cfg_value(const char *profile, char *tile, char *key, char *value);

/**
 * @brief  从配置文件cfg.json中读取mysql的配置信息
 * @param mysql_user [out]  mysql用户名 
 * @param mysql_pwd  [out]  mysql密码
 * @param mysql_db   [out]  mysql数据库名
 */
extern int get_mysql_info(char *mysql_user, char *mysql_pwd, char *mysql_db);


#endif
