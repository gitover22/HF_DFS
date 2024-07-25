#ifndef  _MAKE_LOG_H_
#define  _MAKE_LOG_H
#include "pthread.h"

/**
 * @brief 将缓冲区 buf 中的内容写入到指定路径 path 的文件中。
 * @param path [in] 指向文件路径的字符数组，指定要写入的文件。
 * @param buf [in] 指向要写入文件的缓冲区的字符数组。
 * @return 返回0，表示执行成功。
 */
int out_put_file(char *path, char *buf);

/**
 * @brief 根据当前时间和服务名生成日志路径。
 * @param path [out] 用于存储最终生成的日志路径的字符数组。
 * @param module_name [in] 模块名，用于创建模块特定的日志目录。 cgi or database
 * @param proc_name [in] 进程名，用于创建进程特定的日志文件。
 * @return 函数执行成功返回0，失败返回非0值。
 */
int make_path(char *path, char *module_name, char *proc_name);

/**
 * @brief 用于格式化和记录日志消息到指定的文件中。
 * @param module_name [in] 模块名称，用于日志分类。
 * @param proc_name [in] 进程名称，用于标识消息来源的进程。
 * @param filename [in] 源文件名，用于日志定位。
 * @param line [in] 发生日志事件的源代码行号。
 * @param funcname [in] 发生日志事件的函数名。
 * @param fmt [in] 格式化字符串，类似于printf函数的格式化参数。
 * @param ... 可变参数，根据fmt格式化。
 * @return 函数始终返回0，表示日志记录成功或失败并不重要。
 */
int dumpmsg_to_file(char *module_name, char *proc_name, const char *filename,
                        int line, const char *funcname, char *fmt, ...);

                        
#ifndef _LOG
#define LOG(module_name, proc_name, x...) \
        do{ \
		dumpmsg_to_file(module_name, proc_name, __FILE__, __LINE__, __FUNCTION__, ##x);\
	}while(0)
#else
#define LOG(module_name, proc_name, x...)
#endif

extern pthread_mutex_t ca_log_lock;

#endif


