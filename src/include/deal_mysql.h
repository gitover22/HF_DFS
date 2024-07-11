#ifndef _DEAL_MYSQL_H_
#define _DEAL_MYSQL_H_

#include <mysql/mysql.h> //数据库

#define SQL_MAX_LEN         (512)   //sql语句长度

/**
 * @brief  打印操作数据库出错时的错误信息
 * @param conn [in] 连接数据库的句柄
 * @param title [in] 用户错误提示信息
 */
void print_error(MYSQL *conn, const char *title);

/**
 * @brief  连接数据库
 * @param user_name	 (in)   数据库用户
 * @param passwd     (in)   数据库密码
 * @param db_name    (in)   数据库名称
 *
 * @returns
 *          成功：连接数据库的局部
 *			失败：NULL
 */
MYSQL* msql_conn(char *user_name, char* passwd, char *db_name);



/**
 * @brief  处理数据库查询结果
 * @param conn	   [in]   连接数据库的句柄
 * @param res_set  [in]   数据库查询后的结果集
 */
void process_result_test(MYSQL *conn, MYSQL_RES *res_set);


/**
 * @brief  处理数据库查询结果并可选择性地将第一条记录存储至缓冲区
 * @param conn       [in]   连接数据库的句柄
 * @param sql_cmd    [in]   要执行的SQL命令
 * @param buf        [out]  存储查询结果的第一条记录的缓冲区，如果不需要存储结果可以设为NULL
 * @return           函数执行状态和结果集的存在状态:
 *                   -1: SQL执行或结果集生成失败
 *                    0: SQL执行成功但未生成结果集
 *                    1: 结果集中无记录
 *                    2: 结果集中有记录但未保存
 *                    other: 成功保存结果集中的第一条记录至buf
 */
int process_result_one(MYSQL *conn, char *sql_cmd, char *buf);

#endif
