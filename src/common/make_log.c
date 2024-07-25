#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>
#include <sys/stat.h>

#include "make_log.h"
#include <pthread.h>

#if 1

pthread_mutex_t ca_log_lock = PTHREAD_MUTEX_INITIALIZER;

int dumpmsg_to_file(char *module_name, char *proc_name, const char *filename,
					int line, const char *funcname, char *fmt, ...)
{
	char mesg[4096] = {0};
	char buf[4096] = {0};
	char filepath[1024] = {0};
	time_t t = 0;
	struct tm *now = NULL;
	va_list ap;
	time(&t);
	now = localtime(&t); // 转换为本地时间格式
	va_start(ap, fmt);
	vsprintf(mesg, fmt, ap);
	va_end(ap);
#if 1
	snprintf(buf, 4096, "[%04d-%02d-%02d %02d:%02d:%02d]--[%s:%d]--%s",
			 now->tm_year + 1900, now->tm_mon + 1,
			 now->tm_mday, now->tm_hour, now->tm_min, now->tm_sec,
			 filename, line, mesg);
#endif
	make_path(filepath, module_name, proc_name);
	pthread_mutex_lock(&ca_log_lock);
	out_put_file(filepath, buf);
	pthread_mutex_unlock(&ca_log_lock);
	return 0;
}
#endif

int out_put_file(char *path, char *buf)
{
	int fd;
	fd = open(path, O_CREAT | O_APPEND | O_RDWR, 0777);

	if (write(fd, buf, strlen(buf)) != (int)strlen(buf))
	{
		fprintf(stderr, "write error!\n");
		close(fd);
	}
	else
	{
		close(fd);
	}
	return 0;
}
int make_path(char *path, char *module_name, char *proc_name)
{
	time_t t;
	struct tm *now = NULL;
	// 顶级目录
	char top_dir[1024] = {".."};
	// 二级目录
	char second_dir[1024] = {"../logs"};
	char third_dir[1024] = {0};
	char y_dir[1024] = {0};
	char m_dir[1024] = {0};
	// char d_dir[1024] = {0};
	// 获取当前时间
	time(&t);
	now = localtime(&t);
	snprintf(path, 1024, "../logs/%s/%04d/%02d/%s-%02d.log", module_name, now->tm_year + 1900, now->tm_mon + 1, proc_name, now->tm_mday);

	sprintf(third_dir, "%s/%s", second_dir, module_name);		// ../logs/cgi
	sprintf(y_dir, "%s/%04d/", third_dir, now->tm_year + 1900); // ../logs/cgi/2024
	sprintf(m_dir, "%s/%02d/", y_dir, now->tm_mon + 1);			// ../logs/cgi/2024/08
	// sprintf(d_dir, "%s/%02d/", m_dir, now->tm_mday); // ../logs/cgi/2024/08/09
	// 检查顶级目录是否存在，若不存在则创建
	if (access(top_dir, 0) == -1)
	{
		if (mkdir(top_dir, 0777) == -1)
		{
			fprintf(stderr, "create %s failed!\n", top_dir);
		}
		else if (mkdir(second_dir, 0777) == -1)
		{
			fprintf(stderr, "%s:create %s failed!\n", top_dir, second_dir);
		}
		else if (mkdir(third_dir, 0777) == -1)
		{
			fprintf(stderr, "%s:create %s failed!\n", top_dir, third_dir);
		}
		else if (mkdir(y_dir, 0777) == -1)
		{
			fprintf(stderr, "%s:create %s failed!\n", top_dir, y_dir);
		}
		else if (mkdir(m_dir, 0777) == -1)
		{
			fprintf(stderr, "%s:create %s failed!\n", top_dir, m_dir);
		}
	}
	// 若顶级目录存在，检查二级目录是否存在，若不存在则创建
	else if (access(second_dir, 0) == -1)
	{
		if (mkdir(second_dir, 0777) == -1)
		{
			fprintf(stderr, "create %s failed!\n", second_dir);
		}
		else if (mkdir(third_dir, 0777) == -1)
		{
			fprintf(stderr, "%s:create %s failed!\n", second_dir, third_dir);
		}
		else if (mkdir(y_dir, 0777) == -1)
		{
			fprintf(stderr, "%s:create %s failed!\n", second_dir, y_dir);
		}
		else if (mkdir(m_dir, 0777) == -1)
		{
			fprintf(stderr, "%s:create %s failed!\n", second_dir, m_dir);
		}
	}
	else if (access(third_dir, 0) == -1)
	{
		if (mkdir(third_dir, 0777) == -1)
		{
			fprintf(stderr, "create %s failed!\n", third_dir);
		}
		else if (mkdir(y_dir, 0777) == -1)
		{
			fprintf(stderr, "%s:create %s failed!\n", third_dir, y_dir);
		}
		else if (mkdir(m_dir, 0777) == -1)
		{
			fprintf(stderr, "%s:create %s failed!\n", third_dir, m_dir);
		}
	}
	else if (access(y_dir, 0) == -1)
	{
		if (mkdir(y_dir, 0777) == -1)
		{
			fprintf(stderr, "create %s failed!\n", y_dir);
		}
		else if (mkdir(m_dir, 0777) == -1)
		{
			fprintf(stderr, "%s:create %s failed!\n", y_dir, m_dir);
		}
	}
	else if (access(m_dir, 0) == -1)
	{
		if (mkdir(m_dir, 0777) == -1)
		{
			fprintf(stderr, "create %s failed!\n", m_dir);
		}
	}
	// printf("path:%s\n", path);
	return 0;
}

#if 0
int main(void)
{
	char path[1024] = {0};
	char proc_name[] = {"sys_guard"};
	char buf[] = {"12345\n"};
	make_path(path, proc_name);
	out_put_file(path, buf);
	return 0;
}
#endif
