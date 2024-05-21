/**
* Copyright (C) 2008 Happy Fish / YuQing
*
* FastDFS may be copied only under the terms of the GNU General
* Public License V3, which may be found in the FastDFS source kit.
* Please visit the FastDFS Home Page http://www.csource.org/ for more detail.
**/

#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <signal.h>
#include <inttypes.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/file.h>
#include <dirent.h>
#include <grp.h>
#include <pwd.h>
#include <math.h>

#include "shared_func.h"
#include "logger.h"
#include "sockopt.h"

#ifdef OS_LINUX
#include <sys/sysinfo.h>
#else
#ifdef OS_FREEBSD
#include <sys/sysctl.h>
#endif
#endif

char *formatDatetime(const time_t nTime, \
	const char *szDateFormat, \
	char *buff, const int buff_size)
{
	static char szDateBuff[128];
	struct tm tmTime;
	int size;

	localtime_r(&nTime, &tmTime);
	if (buff == NULL)
	{
		buff = szDateBuff;
		size = sizeof(szDateBuff);
	}
	else
	{
		size = buff_size;
	}

	*buff = '\0';
	strftime(buff, size, szDateFormat, &tmTime);
	
	return buff;
}

int getCharLen(const char *s)
{
	unsigned char *p;
	int count = 0;
	
	p = (unsigned char *)s;
	while (*p != '\0')
	{
		if (*p > 127)
		{
			if (*(++p) != '\0')
			{
				p++;
			}
		}
		else
		{
			p++;
		}
		
		count++;
	}
	
	return count;
}

char *replaceCRLF2Space(char *s)
{
	char *p = s;
	
	while (*p != '\0')
	{
		if (*p == '\r' || *p == '\n')
		{
			*p = ' ';
		}
		
		p++;
	}
	
	return s;
}

char *getAbsolutePath(const char *filename, char *szAbsPath, \
		const int pathSize)
{
	char *p;
	int nPathLen;
	char szPath[1024];
	char cwd[256];
	
	p = strrchr(filename, '/');
	if (p == NULL)
	{
		szPath[0] = '\0';
	}
	else
	{
		nPathLen = p - filename;
		if (nPathLen >= sizeof(szPath))
		{
			logError("file: "__FILE__", line: %d, " \
				"filename length: %d is too long, exceeds %d",\
				__LINE__, nPathLen, (int)sizeof(szPath));
			return NULL;
		}
	
		memcpy(szPath, filename, nPathLen);
		szPath[nPathLen] = '\0';
	}
	
	if (szPath[0] == '/')
	{
		snprintf(szAbsPath, pathSize, "%s", szPath);
	}
	else
	{
		if (getcwd(cwd, sizeof(cwd)) == NULL)
		{
			logError("file: "__FILE__", line: %d, " \
				"call getcwd fail, errno: %d, error info: %s", \
				__LINE__, errno, STRERROR(errno));
			return NULL;
		}
		
		nPathLen = strlen(cwd);
		if (cwd[nPathLen - 1] == '/')
		{
			cwd[nPathLen - 1] = '\0';
		}
		
		if (szPath[0] != '\0')
		{
			snprintf(szAbsPath, pathSize, "%s/%s", cwd, szPath);
		}
		else
		{
			snprintf(szAbsPath, pathSize, "%s", cwd);
		}
	}
	
	return szAbsPath;
}

char *getExeAbsoluteFilename(const char *exeFilename, char *szAbsFilename, \
		const int maxSize)
{
	const char *filename;
	const char *p;
	int nFileLen;
	int nPathLen;
	char cwd[256];
	char szPath[1024];
	
	nFileLen = strlen(exeFilename);
	if (nFileLen >= sizeof(szPath))
	{
		logError("file: "__FILE__", line: %d, " \
			"filename length: %d is too long, exceeds %d!", \
			__LINE__, nFileLen, (int)sizeof(szPath));
		return NULL;
	}
	
	p = strrchr(exeFilename, '/');
	if (p == NULL)
	{
		int i;
		char *search_paths[] = {"/bin", "/usr/bin", "/usr/local/bin"};

		*szPath = '\0';
		filename = exeFilename;
		for (i=0; i<3; i++)
		{
			snprintf(cwd, sizeof(cwd), "%s/%s", \
				search_paths[i], filename);
			if (fileExists(cwd))
			{
				strcpy(szPath, search_paths[i]);
				break;
			}
		}

		if (*szPath == '\0')
		{
			if (!fileExists(filename))
			{
				logError("file: "__FILE__", line: %d, " \
					"can't find exe file %s!", __LINE__, \
					filename);
				return NULL;
			}
		}
		else
		{
			snprintf(szAbsFilename, maxSize, "%s/%s", \
				szPath, filename);
			return szAbsFilename;
		}
	}
	else
	{
		filename = p + 1;
		nPathLen = p - exeFilename;
		memcpy(szPath, exeFilename, nPathLen);
		szPath[nPathLen] = '\0';
	}
	
	if (*szPath == '/')
	{
		snprintf(szAbsFilename, maxSize, "%s/%s", szPath, filename);
	}
	else
	{
		if (getcwd(cwd, sizeof(cwd)) == NULL)
		{
			logError("file: "__FILE__", line: %d, " \
				"call getcwd fail, errno: %d, error info: %s", \
				__LINE__, errno, STRERROR(errno));
			return NULL;
		}
		
		nPathLen = strlen(cwd);
		if (cwd[nPathLen - 1] == '/')
		{
			cwd[nPathLen - 1] = '\0';
		}
		
		if (*szPath != '\0')
		{
			snprintf(szAbsFilename, maxSize, "%s/%s/%s", \
				cwd, szPath, filename);
		}
		else
		{
			snprintf(szAbsFilename, maxSize, "%s/%s", \
				cwd, filename);
		}
	}
	
	return szAbsFilename;
}

#ifndef WIN32
int getProccessCount(const char *progName, const bool bAllOwners)
{
	int *pids = NULL;
	return getUserProcIds(progName, bAllOwners, pids, 0);
}

int getUserProcIds(const char *progName, const bool bAllOwners, \
		int pids[], const int arrSize)
{
	char path[128]="/proc";
	char fullpath[128];
	struct stat statbuf;
	struct dirent *dirp;
	DIR  *dp;
	int  myuid=getuid();
	int  fd;
	char filepath[128];
	char buf[256];
	char *ptr;
	int  nbytes;
	char procname[64];
	int  cnt=0;
	char *pTargetProg;
	
	if ((dp = opendir(path)) == NULL)
	{
		return -1;
	}
	
	pTargetProg = (char *)malloc(strlen(progName) + 1);
	if (pTargetProg == NULL)
	{
		logError("file: "__FILE__", line: %d, " \
			"malloc %d bytes fail", __LINE__, \
			(int)strlen(progName) + 1);
		return -1;
	}

	ptr = strrchr(progName, '/');
	if (ptr == NULL)
	{
		strcpy(pTargetProg, progName);
	}
	else
	{
		strcpy(pTargetProg, ptr + 1);
	}
	
	while( (dirp = readdir(dp)) != NULL )
	{
		if (strcmp(dirp->d_name, ".")==0 || strcmp(dirp->d_name, "..")==0 )
		{
			continue;
		}
		
		sprintf(fullpath, "%s/%s", path, dirp->d_name);
		memset(&statbuf, 0, sizeof(statbuf));
		if (lstat(fullpath, &statbuf)<0)
		{
			continue;
		}
		
		if ((bAllOwners || (statbuf.st_uid == myuid)) && S_ISDIR(statbuf.st_mode))
		{
			sprintf(filepath, "%s/cmdline", fullpath);
			if ((fd = open(filepath, O_RDONLY))<0)
			{
				continue;
			}
			
			memset(buf, 0, 256);
			if ((nbytes = read(fd, buf, 255)) < 0){
				close(fd);
				continue;
			}
			close(fd);
			
			if (*buf == '\0')
			{
				continue;
			}
			
			ptr = strrchr(buf, '/');
			if (ptr == NULL)
			{
				snprintf(procname, 64, "%s", buf);
			}
			else
			{
				snprintf(procname, 64, "%s", ptr + 1);
			}
			
			if (strcmp(procname, pTargetProg) == 0)
			{				
				if (pids != NULL)
				{
					if (cnt >= arrSize)
					{
						break;
					}
					pids[cnt] = atoi(dirp->d_name);
				}
				
				cnt++;
			}
		}
	}
	free(pTargetProg);
	
	closedir(dp);
	return cnt;
}

int getExecResult(const char *command, char *output, const int buff_size)
{
	FILE *fp;
	char *pCurrent;
	int bytes_read;
	int remain_bytes;

	if((fp=popen(command, "r")) == NULL)
	{
        *output = '\0';
		return errno != 0 ? errno : EMFILE;
	}

	pCurrent = output;
	remain_bytes = buff_size - 1;
	while (remain_bytes > 0 && \
		(bytes_read=fread(pCurrent, 1, remain_bytes, fp)) > 0)
	{
		pCurrent += bytes_read;
		remain_bytes -= bytes_read;
	}
	pclose(fp);

	*pCurrent = '\0';
	return remain_bytes > 0 ? 0 : ENOSPC;
}

#endif

char *toLowercase(char *src)
{
	char *p;
	
	p = src;
	while (*p != '\0')
	{
		if (*p >= 'A' && *p <= 'Z')
		{
			*p += 32;
		}
		p++;
	}
	
	return src;
}

char *toUppercase(char *src)
{
	char *p;
	
	p = src;
	while (*p != '\0')
	{
		if (*p >= 'a' && *p <= 'z')
		{
			*p -= 32;
		}
		p++;
	}
	
	return src;	
}

void daemon_init(bool bCloseFiles)
{
#ifndef WIN32
	pid_t pid;
	int i;
	
	if((pid=fork()) != 0)
	{
		exit(0);
	}
	
	setsid();
	
	if((pid=fork()) != 0)
	{
		exit(0);
	}

#ifdef DEBUG_FLAG
	#define MAX_CORE_FILE_SIZE  (256 * 1024 * 1024)
	if (set_rlimit(RLIMIT_CORE, MAX_CORE_FILE_SIZE) != 0)
	{
		logWarning("file: "__FILE__", line: %d, " \
			"set max core dump file size to %d MB fail, " \
			"errno: %d, error info: %s", \
			__LINE__, MAX_CORE_FILE_SIZE / (1024 * 1024), \
			errno, STRERROR(errno));
	}
#else
	if (chdir("/") != 0)
	{
		logWarning("file: "__FILE__", line: %d, " \
			"change directory to / fail, " \
			"errno: %d, error info: %s", \
			__LINE__, errno, STRERROR(errno));
	}
#endif

	if (bCloseFiles)
	{
		for(i=0; i<=2; i++)
		{
			close(i);
		}
	}
#endif

	return;
}

char *bin2hex(const char *s, const int len, char *szHexBuff)
{
	unsigned char *p;
	unsigned char *pEnd;
	int nLen;
	
	nLen = 0;
	pEnd = (unsigned char *)s + len;
	for (p=(unsigned char *)s; p<pEnd; p++)
	{
		nLen += sprintf(szHexBuff + nLen, "%02x", *p);
	}
	
	szHexBuff[nLen] = '\0';
	return szHexBuff;
}

char *hex2bin(const char *s, char *szBinBuff, int *nDestLen)
{
        char buff[3];
	char *pSrc;
	int nSrcLen;
	char *pDest;
	char *pDestEnd;
	
	nSrcLen = strlen(s);
        if (nSrcLen == 0)
        {
          *nDestLen = 0;
          szBinBuff[0] = '\0';
          return szBinBuff;
        }

	*nDestLen = nSrcLen / 2;
	pSrc = (char *)s;
        buff[2] = '\0';

	pDestEnd = szBinBuff + (*nDestLen);
	for (pDest=szBinBuff; pDest<pDestEnd; pDest++)
	{
		buff[0] = *pSrc++;
		buff[1] = *pSrc++;
		*pDest = (char)strtol(buff, NULL, 16);
	}
	
	*pDest = '\0';
	return szBinBuff;
}

void printBuffHex(const char *s, const int len)
{
	unsigned char *p;
	int i;
	
	p = (unsigned char *)s;
	for (i=0; i<len; i++)
	{
		printf("%02X", *p);
		p++;
	}
	printf("\n");
}

char *trim_left(char *pStr)
{
	char *p;
	char *pEnd;
	int nDestLen;

	pEnd = pStr + strlen(pStr);
	for (p=pStr; p<pEnd; p++)
	{
		if (!(' ' == *p|| '\n' == *p || '\r' == *p || '\t' == *p))
		{
			break;
		}
	}
	
	if ( p == pStr)
	{
		return pStr;
	}
	
	nDestLen = (pEnd - p) + 1; //including \0
	memmove(pStr, p, nDestLen);

	return pStr;
}

char *trim_right(char *pStr)
{
	int len;
	char *p;
	char *pEnd;

	len = strlen(pStr);
	if (len == 0)
	{
		return pStr;
	}

	pEnd = pStr + len - 1;
	for (p = pEnd;  p>=pStr; p--)
	{
		if (!(' ' == *p || '\n' == *p || '\r' == *p || '\t' == *p))
		{
			break;
		}
	}

	if (p != pEnd)
	{
		*(p+1) = '\0';
	}

	return pStr;
}

char *trim(char *pStr)
{
	trim_right(pStr);
	trim_left(pStr);
	return pStr;
}

char *formatDateYYYYMMDDHHMISS(const time_t t, char *szDateBuff, const int nSize)
{
	time_t timer = t;
	struct tm tm;

	localtime_r(&timer, &tm);
	
	snprintf(szDateBuff, nSize, "%04d%02d%02d%02d%02d%02d", \
		tm.tm_year+1900, tm.tm_mon+1, tm.tm_mday, \
		tm.tm_hour, tm.tm_min, tm.tm_sec);

	return szDateBuff;
}

int getOccurCount(const char *src, const char seperator)
{
	int count;
	char *p;

	count = 0;
	p = strchr(src, seperator);
	while (p != NULL)
	{
		count++;
		p = strchr(p + 1, seperator);
	}

	return count;
}

char **split(char *src, const char seperator, const int nMaxCols, int *nColCount)
{
	char **pCols;
	char **pCurrent;
	char *p;
	int i;
	int nLastIndex;

	if (src == NULL)
	{
		*nColCount = 0;
		return NULL;
	}

	*nColCount = 1;
	p = strchr(src, seperator);
	
	while (p != NULL)
	{
		(*nColCount)++;
		p = strchr(p + 1, seperator);
	}

	if (nMaxCols > 0 && (*nColCount) > nMaxCols)
	{
		*nColCount = nMaxCols;
	}
	
	pCurrent = pCols = (char **)malloc(sizeof(char *) * (*nColCount));
	if (pCols == NULL)
	{
		logError("file: "__FILE__", line: %d, " \
			"malloc %d bytes fail", __LINE__, \
			(int)sizeof(char *) * (*nColCount));
		return NULL;
	}

	p = src;
	nLastIndex = *nColCount - 1;
	for (i=0; i<*nColCount; i++)
	{
		*pCurrent = p;
		pCurrent++;

		p = strchr(p, seperator);
		if (i != nLastIndex)
		{
			*p = '\0';
			p++;
		}
	}

	return pCols;
}

void freeSplit(char **p)
{
	if (p != NULL)
	{
		free(p);
	}
}

int splitEx(char *src, const char seperator, char **pCols, const int nMaxCols)
{
	char *p;
	char **pCurrent;
	int count = 0;

	if (nMaxCols <= 0)
	{
		return 0;
	}

	p = src;
	pCurrent = pCols;

	while (true)
	{
		*pCurrent = p;
		pCurrent++;

		count++;
		if (count >= nMaxCols)
		{
			break;
		}

		p = strchr(p, seperator);
		if (p == NULL)
		{
			break;
		}

		*p = '\0';
		p++;
	}

	return count;
}

int my_strtok(char *src, const char *delim, char **pCols, const int nMaxCols)
{
    char *p;
    char **pCurrent;
    int count;
    bool bWordEnd;
    
    if (src == NULL || pCols == NULL)
    {
        return -1;
    }

    if (nMaxCols <= 0)
    {
        return 0;
    }
    
    p = src;
    pCurrent = pCols;
    
    while (*p != '\0')
    {
        if (strchr(delim, *p) == NULL)
        {
            break;
        }
        p++;
    }
    
    if (*p == '\0')
    {
        return 0;
    }
    
    *pCurrent = p;
    bWordEnd = false;
    count = 1;
    if (count >= nMaxCols)
    {
        return count;
    }
    
    while (*p != '\0')
    {
        if (strchr(delim, *p) != NULL)
        {
            *p = '\0';
            bWordEnd = true;
        }
        else
        {
            if (bWordEnd)
            {
                pCurrent++;
                *pCurrent = p;
                
                count++;
                if (count >= nMaxCols)
                {
                    break;
                }

                bWordEnd = false;
            }
        }
        
        p++;
    }

    return count;
}

int str_replace(const char *s, const int src_len, const char *replaced, 
		        const char *new_str, char *dest, const int dest_size)
{
	const char *pStart;
	const char *pEnd;
	char *pDest;
	const char *p;
	int old_len;
	int new_len;
	int len;
	int max_dest_len;
	int remain_len;

	if (dest_size <= 0)
	{
		return 0;
	}

	max_dest_len = dest_size - 1;
	old_len = strlen(replaced);
	new_len = strlen(new_str);
	if (old_len == 0)
	{
		len = src_len < max_dest_len ? src_len : max_dest_len;
		memcpy(dest, s, len);
		dest[len] = '\0';
		return len;
	}

	remain_len = max_dest_len;
	pDest = dest;
	pStart = s;
	pEnd = s + src_len;
	while (1)
	{
		p = strstr(pStart, replaced);
		if (p == NULL)
		{
			break;
		}

		len = p - pStart;
		if (len > 0)
		{
			if (len < remain_len)
			{
				memcpy(pDest, pStart, len);
				pDest += len;
				remain_len -= len;
			}
			else
			{
				memcpy(pDest, pStart, remain_len);
				pDest += remain_len;
				*pDest = '\0';
				return pDest - dest;
			}
		}

		if (new_len < remain_len)
		{
			memcpy(pDest, new_str, new_len);
			pDest += new_len;
			remain_len -= new_len;
		}
		else
		{
			memcpy(pDest, new_str, remain_len);
			pDest += remain_len;
			*pDest = '\0';
			return pDest - dest;
		}

		pStart = p + old_len;
	}

	len = pEnd - pStart;
	if (len > 0)
	{
		if (len > remain_len)
		{
			len = remain_len;
		}
		memcpy(pDest, pStart, len);
		pDest += len;
	}
	*pDest = '\0';
	return pDest - dest;
}

bool fileExists(const char *filename)
{
	return access(filename, 0) == 0;
}

bool isDir(const char *filename)
{
	struct stat buf;
	if (stat(filename, &buf) != 0)
	{
		return false;
	}

	return S_ISDIR(buf.st_mode);
}

bool isFile(const char *filename)
{
	struct stat buf;
	if (stat(filename, &buf) != 0)
	{
		return false;
	}

	return S_ISREG(buf.st_mode);
}

void chopPath(char *filePath)
{
	int lastIndex;
	if (*filePath == '\0')
	{
		return;
	}

	lastIndex = strlen(filePath) - 1;
	if (filePath[lastIndex] == '/')
	{
		filePath[lastIndex] = '\0';
	}
}

int getFileContent(const char *filename, char **buff, int64_t *file_size)
{
	int fd;

    errno = 0;
    if (!isFile(filename))
    {
		*buff = NULL;
		*file_size = 0;
        if (errno != 0)
        {
            if (errno == ENOENT)
            {
                logError("file: "__FILE__", line: %d, "
                        "file %s not exist", __LINE__, filename);
            }
            else
            {
                logError("file: "__FILE__", line: %d, "
                        "stat %s fail, errno: %d, error info: %s",
                        __LINE__, filename, errno, STRERROR(errno));
            }
            return errno != 0 ? errno : ENOENT;
        }
        else
        {
            logError("file: "__FILE__", line: %d, "
                    "%s is not a regular file", __LINE__, filename);
            return EINVAL;
        }
    }

	fd = open(filename, O_RDONLY);
	if (fd < 0)
	{
		*buff = NULL;
		*file_size = 0;
		logError("file: "__FILE__", line: %d, " \
			"open file %s fail, " \
			"errno: %d, error info: %s", __LINE__, \
			filename, errno, STRERROR(errno));
		return errno != 0 ? errno : ENOENT;
	}

	if ((*file_size=lseek(fd, 0, SEEK_END)) < 0)
	{
		*buff = NULL;
		*file_size = 0;
		close(fd);
		logError("file: "__FILE__", line: %d, " \
			"lseek file %s fail, " \
			"errno: %d, error info: %s", __LINE__, \
			filename, errno, STRERROR(errno));
		return errno != 0 ? errno : EIO;
	}

	*buff = (char *)malloc(*file_size + 1);
	if (*buff == NULL)
	{
		*file_size = 0;
		close(fd);

		logError("file: "__FILE__", line: %d, " \
			"malloc %d bytes fail", __LINE__, \
			(int)(*file_size + 1));
		return errno != 0 ? errno : ENOMEM;
	}

	if (lseek(fd, 0, SEEK_SET) < 0)
	{
		*buff = NULL;
		*file_size = 0;
		close(fd);
		logError("file: "__FILE__", line: %d, " \
			"lseek file %s fail, " \
			"errno: %d, error info: %s", __LINE__, \
			filename, errno, STRERROR(errno));
		return errno != 0 ? errno : EIO;
	}
	if (read(fd, *buff, *file_size) != *file_size)
	{
		free(*buff);
		*buff = NULL;
		*file_size = 0;
		close(fd);
		logError("file: "__FILE__", line: %d, " \
			"read from file %s fail, " \
			"errno: %d, error info: %s", __LINE__, \
			filename, errno, STRERROR(errno));
		return errno != 0 ? errno : EIO;
	}

	(*buff)[*file_size] = '\0';
	close(fd);

	return 0;
}

int getFileContentEx(const char *filename, char *buff, \
		int64_t offset, int64_t *size)
{
	int fd;
	int read_bytes;

	if (*size <= 0)
	{
		logError("file: "__FILE__", line: %d, " \
			"invalid size: %"PRId64, \
			__LINE__, *size);
		return EINVAL;
	}
	
	fd = open(filename, O_RDONLY);
	if (fd < 0)
	{
		*size = 0;
		logError("file: "__FILE__", line: %d, " \
			"open file %s fail, " \
			"errno: %d, error info: %s", __LINE__, \
			filename, errno, STRERROR(errno));
		return errno != 0 ? errno : ENOENT;
	}

	if (offset > 0 && lseek(fd, offset, SEEK_SET) < 0)
	{
		*size = 0;
		close(fd);
		logError("file: "__FILE__", line: %d, " \
			"lseek file %s fail, " \
			"errno: %d, error info: %s", __LINE__, \
			filename, errno, STRERROR(errno));
		return errno != 0 ? errno : EIO;
	}

	if ((read_bytes=read(fd, buff, *size - 1)) < 0)
	{
		*size = 0;
		close(fd);
		logError("file: "__FILE__", line: %d, " \
			"read from file %s fail, " \
			"errno: %d, error info: %s", __LINE__, \
			filename, errno, STRERROR(errno));
		return errno != 0 ? errno : EIO;
	}

	*size = read_bytes;
	*(buff + (*size)) = '\0';
	close(fd);

	return 0;
}

int writeToFile(const char *filename, const char *buff, const int file_size)
{
	int fd;
	int result;

	fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC, 0644);
	if (fd < 0)
	{
		result = errno != 0 ? errno : EIO;
		logError("file: "__FILE__", line: %d, " \
			"open file %s fail, " \
			"errno: %d, error info: %s", \
			__LINE__, filename, \
			result, STRERROR(result));
		return result;
	}

	if (write(fd, buff, file_size) != file_size)
	{
		result = errno != 0 ? errno : EIO;
		logError("file: "__FILE__", line: %d, " \
			"write file %s fail, " \
			"errno: %d, error info: %s", \
			__LINE__, filename, \
			result, STRERROR(result));
		close(fd);
		return result;
	}

	if (fsync(fd) != 0)
	{
		result = errno != 0 ? errno : EIO;
		logError("file: "__FILE__", line: %d, " \
			"fsync file \"%s\" fail, " \
			"errno: %d, error info: %s", \
			__LINE__, filename, \
			result, STRERROR(result));
		close(fd);
		return result;
	}

	close(fd);
	return 0;
}

int safeWriteToFile(const char *filename, const char *buff, \
		const int file_size)
{
	char tmpFilename[MAX_PATH_SIZE];
	int result;

	snprintf(tmpFilename, sizeof(tmpFilename), "%s.tmp", filename);
	if ((result=writeToFile(tmpFilename, buff, file_size)) != 0)
	{
		return result;
	}

	if (rename(tmpFilename, filename) != 0)
	{
		result = errno != 0 ? errno : EIO;
		logError("file: "__FILE__", line: %d, " \
			"rename file \"%s\" to \"%s\" fail, " \
			"errno: %d, error info: %s", \
			__LINE__, tmpFilename, filename, \
			result, STRERROR(result));
		return result;
	}

	return 0;
}

void short2buff(const short n, char *buff)
{
	unsigned char *p;
	p = (unsigned char *)buff;
	*p++ = (n >> 8) & 0xFF;
	*p++ = n & 0xFF;
}

short buff2short(const char *buff)
{
	return  (short)((((unsigned char)(*(buff))) << 8) | \
		((unsigned char)(*(buff+1))));
}

void int2buff(const int n, char *buff)
{
	unsigned char *p;
	p = (unsigned char *)buff;
	*p++ = (n >> 24) & 0xFF;
	*p++ = (n >> 16) & 0xFF;
	*p++ = (n >> 8) & 0xFF;
	*p++ = n & 0xFF;
}

int buff2int(const char *buff)
{
	return  (((unsigned char)(*buff)) << 24) | \
		(((unsigned char)(*(buff+1))) << 16) |  \
		(((unsigned char)(*(buff+2))) << 8) | \
		((unsigned char)(*(buff+3)));
}

void long2buff(int64_t n, char *buff)
{
	unsigned char *p;
	p = (unsigned char *)buff;
	*p++ = (n >> 56) & 0xFF;
	*p++ = (n >> 48) & 0xFF;
	*p++ = (n >> 40) & 0xFF;
	*p++ = (n >> 32) & 0xFF;
	*p++ = (n >> 24) & 0xFF;
	*p++ = (n >> 16) & 0xFF;
	*p++ = (n >> 8) & 0xFF;
	*p++ = n & 0xFF;
}

int64_t buff2long(const char *buff)
{
	unsigned char *p;
	p = (unsigned char *)buff;
	return  (((int64_t)(*p)) << 56) | \
		(((int64_t)(*(p+1))) << 48) |  \
		(((int64_t)(*(p+2))) << 40) |  \
		(((int64_t)(*(p+3))) << 32) |  \
		(((int64_t)(*(p+4))) << 24) |  \
		(((int64_t)(*(p+5))) << 16) |  \
		(((int64_t)(*(p+6))) << 8) | \
		((int64_t)(*(p+7)));
}

int fd_gets(int fd, char *buff, const int size, int once_bytes)
{
	char *pDest;
	char *p;
	char *pEnd;
	int read_bytes;
	int remain_bytes;
	int rewind_bytes;

	if (once_bytes <= 0)
	{
		once_bytes = 1;
	}

	pDest = buff;
	remain_bytes = size - 1;
	while (remain_bytes > 0)
	{
		if (once_bytes > remain_bytes)
		{
			once_bytes = remain_bytes;
		}

		read_bytes = read(fd, pDest, once_bytes);
		if (read_bytes < 0)
		{
			return -1;
		}
		if (read_bytes == 0)
		{
			break;
		}

		pEnd = pDest + read_bytes;
		for (p=pDest; p<pEnd; p++)
		{
			if (*p == '\n')
			{
				break;
			}
		}

		if (p < pEnd)
		{
			pDest = p + 1;  //find \n, skip \n
			rewind_bytes = pEnd - pDest;
			if (lseek(fd, -1 * rewind_bytes, SEEK_CUR) < 0)
			{
				return -1;
			}

			break;
		}

		pDest = pEnd;
		remain_bytes -= read_bytes;
	}

	*pDest = '\0';
	return pDest - buff;
}

int set_rlimit(int resource, const rlim_t value)
{
	struct rlimit limit;

	if (getrlimit(resource, &limit) != 0)
	{
		logError("file: "__FILE__", line: %d, " \
			"call getrlimit fail, resource=%d, " \
			"errno: %d, error info: %s", \
			__LINE__, resource, errno, STRERROR(errno));
		return errno != 0 ? errno : EPERM;
	}

	if (limit.rlim_cur == RLIM_INFINITY || \
            (value != RLIM_INFINITY && limit.rlim_cur >= value))
	{
		return 0;
	}

	limit.rlim_cur = value;
	if (setrlimit(resource, &limit) != 0)
	{
        const char *label;
        switch (resource)
        {
            case RLIMIT_CPU:
                label = "CPU time in sec";
                break;
            case RLIMIT_STACK:
                label = "max stack size";
                break;
            case RLIMIT_DATA:
                label = "max data size";
                break;
            case RLIMIT_FSIZE:
                label = "max file size";
                break;
            case RLIMIT_RSS:
                label = "max RSS";
                break;
            case RLIMIT_CORE:
                label = "max core file size";
                break;
            case RLIMIT_NPROC:
                label = "max processes";
                break;
            case RLIMIT_NOFILE:
                label = "max open files";
                break;
#ifdef RLIMIT_MSGQUEUE
            case RLIMIT_MSGQUEUE:
                label = "max bytes in msg queues";
                break;
#endif
            case RLIMIT_MEMLOCK:
                label = "max locked-in-memory address space";
                break;
            default:
                label = "unkown";
                break;
        }

		logError("file: "__FILE__", line: %d, " \
			"call setrlimit fail, resource=%d (%s), value=%"PRId64", " \
			"errno: %d, error info: %s", \
			__LINE__, resource, label, (int64_t)value, \
			errno, STRERROR(errno));
		return errno != 0 ? errno : EPERM;
	}

	return 0;
}

bool is_filename_secure(const char *filename, const int len)
{
	if (len < 3)
	{
		return true;
	}

	if (memcmp(filename, "../", 3) == 0)
	{
		return false;
	}

	return (strstr(filename, "/../") == NULL);
}

void load_log_level(IniContext *pIniContext)
{
	set_log_level(iniGetStrValue(NULL, "log_level", pIniContext));
}

int load_log_level_ex(const char *conf_filename)
{
	int result;
	IniContext iniContext;

	if ((result=iniLoadFromFileEx(conf_filename, &iniContext,
                    FAST_INI_ANNOTATION_DISABLE, NULL, 0,
                    FAST_INI_FLAGS_NONE)) != 0)
	{
		logError("file: "__FILE__", line: %d, " \
			"load conf file \"%s\" fail, ret code: %d", \
			__LINE__, conf_filename, result);
		return result;
	}

	load_log_level(&iniContext);
	iniFreeContext(&iniContext);
	return 0;
}

void set_log_level(char *pLogLevel)
{
	if (pLogLevel != NULL)
	{
		toUppercase(pLogLevel);
		if ( strncmp(pLogLevel, "DEBUG", 5) == 0 || \
		     strcmp(pLogLevel, "LOG_DEBUG") == 0)
		{
			g_log_context.log_level = LOG_DEBUG;
		}
		else if ( strncmp(pLogLevel, "INFO", 4) == 0 || \
		     strcmp(pLogLevel, "LOG_INFO") == 0)
		{
			g_log_context.log_level = LOG_INFO;
		}
		else if ( strncmp(pLogLevel, "NOTICE", 6) == 0 || \
		     strcmp(pLogLevel, "LOG_NOTICE") == 0)
		{
			g_log_context.log_level = LOG_NOTICE;
		}
		else if ( strncmp(pLogLevel, "WARN", 4) == 0 || \
		     strcmp(pLogLevel, "LOG_WARNING") == 0)
		{
			g_log_context.log_level = LOG_WARNING;
		}
		else if ( strncmp(pLogLevel, "ERR", 3) == 0 || \
		     strcmp(pLogLevel, "LOG_ERR") == 0)
		{
			g_log_context.log_level = LOG_ERR;
		}
		else if ( strncmp(pLogLevel, "CRIT", 4) == 0 || \
		     strcmp(pLogLevel, "LOG_CRIT") == 0)
		{
			g_log_context.log_level = LOG_CRIT;
		}
		else if ( strncmp(pLogLevel, "ALERT", 5) == 0 || \
		     strcmp(pLogLevel, "LOG_ALERT") == 0)
		{
			g_log_context.log_level = LOG_ALERT;
		}
		else if ( strncmp(pLogLevel, "EMERG", 5) == 0 || \
		     strcmp(pLogLevel, "LOG_EMERG") == 0)
		{
			g_log_context.log_level = LOG_EMERG;
		}
	}
}

int fcntl_add_flags(int fd, int get_cmd, int set_cmd, int adding_flags)
{
	int flags;

	flags = fcntl(fd, get_cmd, 0);
	if (flags < 0)
	{
		logError("file: "__FILE__", line: %d, " \
			"fcntl fail, errno: %d, error info: %s.", \
			__LINE__, errno, STRERROR(errno));
		return errno != 0 ? errno : EACCES;
	}

	if (fcntl(fd, set_cmd, flags | adding_flags) == -1)
	{
		logError("file: "__FILE__", line: %d, " \
			"fcntl fail, errno: %d, error info: %s.", \
			__LINE__, errno, STRERROR(errno));
		return errno != 0 ? errno : EACCES;
	}

	return 0;
}

int fd_add_flags(int fd, int adding_flags)
{
    return fcntl_add_flags(fd, F_GETFL, F_SETFL, adding_flags);
}

int fd_set_cloexec(int fd)
{
    return fcntl_add_flags(fd, F_GETFD, F_SETFD, FD_CLOEXEC);
}

int set_run_by(const char *group_name, const char *username)
{
#ifndef WIN32
	struct group *pGroup;
	struct passwd *pUser;
	int nErrNo;
	if (group_name != NULL && *group_name != '\0')
	{
     		pGroup = getgrnam(group_name);
		if (pGroup == NULL)
		{
			nErrNo = errno != 0 ? errno : ENOENT;
			logError("file: "__FILE__", line: %d, " \
				"getgrnam %s fail, errno: %d, error info: %s.", \
				__LINE__, group_name, nErrNo, STRERROR(nErrNo));
			return nErrNo;
		}

		if (setegid(pGroup->gr_gid) != 0)
		{
			nErrNo = errno != 0 ? errno : EPERM;
			logError("file: "__FILE__", line: %d, " \
				"setegid fail, errno: %d, error info: %s.", \
				__LINE__, nErrNo, STRERROR(nErrNo));
			return nErrNo;
		}
	}

	if (username != NULL && *username != '\0')
	{
     		pUser = getpwnam(username);
		if (pUser == NULL)
		{
			nErrNo = errno != 0 ? errno : ENOENT;
			logError("file: "__FILE__", line: %d, " \
				"getpwnam %s fail, errno: %d, error info: %s.", \
				__LINE__, username, nErrNo, STRERROR(nErrNo));
			return nErrNo;
		}

		if (seteuid(pUser->pw_uid) != 0)
		{
			nErrNo = errno != 0 ? errno : EPERM;
			logError("file: "__FILE__", line: %d, " \
				"seteuid fail, errno: %d, error info: %s.", \
				__LINE__, nErrNo, STRERROR(nErrNo));
			return nErrNo;
		}
	}
#endif

	return 0;
}

static int check_realloc_allow_ips(in_addr_t **allow_ip_addrs,
	int *alloc_count, const int target_ip_count)
{
	int bytes;
	if (*alloc_count < target_ip_count)
	{
		*alloc_count = target_ip_count;
		bytes = sizeof(in_addr_t) * (*alloc_count);
		*allow_ip_addrs = (in_addr_t *)realloc(*allow_ip_addrs, bytes);
		if (*allow_ip_addrs == NULL)
		{
			logError("file: "__FILE__", line: %d, "\
				"malloc %d bytes fail, " \
				"errno: %d, error info: %s", \
				__LINE__, bytes, errno, STRERROR(errno));

			return errno != 0 ? errno : ENOMEM;
		}
	}

	return 0;
}

static int parse_cidr_ips(const char *ip_addr, in_addr_t **allow_ip_addrs,
	int *alloc_count, int *allow_ip_count, const int remain_items)
{
	char *pSlash;
	char *pReservedEnd;
	char ip_part[IP_ADDRESS_SIZE];
	int ip_len;
	int network_bits;
	int host_bits;
	int bits;
	uint32_t hip;
	in_addr_t nip;
	int h;
	int count;
	int result;
	struct in_addr addr;

	pSlash = strchr(ip_addr, '/');
	if (pSlash == NULL)
	{
		return EINVAL;
	}

	ip_len = pSlash - ip_addr;
	if (ip_len == 0 || ip_len >= IP_ADDRESS_SIZE)
	{
		logError("file: "__FILE__", line: %d, " \
			"invalid ip address: %s", \
			__LINE__, ip_addr);
		return EINVAL;
	}
	memcpy(ip_part, ip_addr, ip_len);
	*(ip_part + ip_len) = '\0';
	
	pReservedEnd = NULL;
	network_bits = strtol(pSlash + 1, &pReservedEnd, 10);
	if (!(pReservedEnd == NULL || *pReservedEnd == '\0'))
	{
		logError("file: "__FILE__", line: %d, " \
			"ip address: %s, invalid network bits: %s",
			__LINE__, ip_addr, pSlash + 1);
		return EINVAL;
	}

	if (network_bits < 16 || network_bits >= 32)
	{
		logError("file: "__FILE__", line: %d, " \
			"ip address: %s, invalid network bits: %d, " \
			"it should >= 16 and < 32", \
			__LINE__, ip_addr, network_bits);
		return EINVAL;
	}

	if (inet_pton(AF_INET, ip_part, &addr) != 1)
	{
		logError("file: "__FILE__", line: %d, " \
			"ip address: %s, invalid ip part: %s", \
			__LINE__, ip_addr, ip_part);
		return EINVAL;
	}

	host_bits = 32 - network_bits;
	count = 2;
	bits = host_bits;
	while (--bits)
	{
		count *= 2;
	}

	if ((result=check_realloc_allow_ips(allow_ip_addrs,
		alloc_count, (*allow_ip_count) + count + remain_items)) != 0)
	{
		return result;
	}

	logDebug("CIDR ip: %s, ip count: %d", ip_addr, count);

	hip = ntohl(addr.s_addr);
	for (h=0; h<count; h++)
	{
		struct sockaddr_in a;

		nip = htonl(hip | h);
		(*allow_ip_addrs)[*allow_ip_count] = nip;
		(*allow_ip_count)++;

		a.sin_addr.s_addr = nip;
		if (inet_ntop(AF_INET, &a.sin_addr, ip_part, IP_ADDRESS_SIZE) != NULL)
		{
			logDebug("%d. %s", h + 1, ip_part);
		}
	}

	return 0;
}

static int parse_range_hosts(const char *value, char *pStart, char *pEnd,
	char *hostname, const int nHeadLen, in_addr_t **allow_ip_addrs,
	int *alloc_count, int *allow_ip_count, const int remain_items)
{
	char *pTail;
	char *p;
	int result;
	int i;
	in_addr_t addr;

	pTail = pEnd + 1;
	p = pStart + 1;  //skip [
	while (p <= pEnd)
	{
		char *pNumStart1;
		char *pNumStart2;
		int nStart;
		int nEnd;
		int nNumLen1;
		int nNumLen2;
		char end_ch1;
		char end_ch2;
		char szFormat[16];

		while (*p == ' ' || *p == '\t') //trim prior spaces
		{
			p++;
		}

		pNumStart1 = p;
		while (*p >='0' && *p <= '9')
		{
			p++;
		}

		nNumLen1 = p - pNumStart1;
		while (*p == ' ' || *p == '\t') //trim tail spaces
		{
			p++;
		}

		if (!(*p == ',' || *p == '-' || *p == ']'))
		{
			logError("file: "__FILE__", line: %d, " \
				"invalid char \"%c\" in host name: %s",\
				__LINE__, *p, value);
			return EINVAL;
		}

		end_ch1 = *p;
		*(pNumStart1 + nNumLen1) = '\0';

		if (nNumLen1 == 0)
		{
			logError("file: "__FILE__", line: %d, " \
				"invalid host name: %s, " \
				"empty entry before \"%c\"", \
				__LINE__, value, end_ch1);
			return EINVAL;
		}

		nStart = atoi(pNumStart1);
		if (end_ch1 == '-')
		{
			p++;   //skip -

			/* trim prior spaces */
			while (*p == ' ' || *p == '\t')
			{
				p++;
			}

			pNumStart2 = p;
			while (*p >='0' && *p <= '9')
			{
				p++;
			}

			nNumLen2 = p - pNumStart2;
			/* trim tail spaces */
			while (*p == ' ' || *p == '\t')
			{
				p++;
			}

			if (!(*p == ',' || *p == ']'))
			{
				logError("file: "__FILE__", line: %d, " \
					"invalid char \"%c\" in host name: %s",\
					__LINE__, *p, value);
				return EINVAL;
			}

			end_ch2 = *p;
			*(pNumStart2 + nNumLen2) = '\0';

			if (nNumLen2 == 0)
			{
				logError("file: "__FILE__", line: %d, " \
					"invalid host name: %s, " \
					"empty entry before \"%c\"", \
					__LINE__, value, end_ch2);
				return EINVAL;
			}

			nEnd = atoi(pNumStart2);
		}
		else
		{
			nEnd = nStart;
		}


		if ((result=check_realloc_allow_ips(allow_ip_addrs,
			alloc_count, (*allow_ip_count) + remain_items +
			(nEnd - nStart + 1))) != 0)
		{
			return result;
		}

		sprintf(szFormat, "%%0%dd%%s",  nNumLen1);
		for (i=nStart; i<=nEnd; i++)
		{
			sprintf(hostname + nHeadLen, szFormat, \
				i, pTail);

			addr = getIpaddrByName(hostname, NULL, 0);
			if (addr == INADDR_NONE)
			{
				logWarning("file: "__FILE__", line: %d, " \
					"invalid host name: %s", \
					__LINE__, hostname);
			}
			else
			{
				(*allow_ip_addrs)[*allow_ip_count] = addr;
				(*allow_ip_count)++;
			}

		}

		p++;
	}

	return 0;
}

int load_allow_hosts(IniContext *pIniContext, \
		in_addr_t **allow_ip_addrs, int *allow_ip_count)
{
	int result;
	int count;
	IniItem *pItem;
	IniItem *pItemStart;
	IniItem *pItemEnd;
	char item_value[256];
	char *pStart;
	char *pEnd;
	int alloc_count;
	int nHeadLen;
	int nValueLen;
	int i;
	in_addr_t addr;
	char hostname[256];

	if ((pItemStart=iniGetValuesEx(NULL, "allow_hosts", \
		pIniContext, &count)) == NULL)
	{
		*allow_ip_count = -1; /* -1 means match any ip address */
		*allow_ip_addrs = NULL;
		return 0;
	}

	pItemEnd = pItemStart + count;
	for (pItem=pItemStart; pItem<pItemEnd; pItem++)
	{
		if (strcmp(pItem->value, "*") == 0)
		{
			*allow_ip_count = -1; /* -1 means match any ip address*/
			*allow_ip_addrs = NULL;
			return 0;
		}
	}

	alloc_count = count;
	*allow_ip_count = 0;
	*allow_ip_addrs = (in_addr_t *)malloc(sizeof(in_addr_t) * alloc_count);
	if (*allow_ip_addrs == NULL)
	{
		logError("file: "__FILE__", line: %d, " \
			"malloc %d bytes fail, errno: %d, error info: %s.", \
			__LINE__, (int)sizeof(in_addr_t) * alloc_count, \
			errno, STRERROR(errno));
		return errno != 0 ? errno : ENOMEM;
	}

	for (pItem=pItemStart; pItem<pItemEnd; pItem++)
	{
		if (*(pItem->value) == '\0')
		{
			continue;
		}

		pStart = strchr(pItem->value, '[');
		if (pStart == NULL)
		{
			if (strchr(pItem->value, '/') != NULL) //CIDR addresses
			{
				if ((result=parse_cidr_ips(pItem->value,
					allow_ip_addrs, &alloc_count,
					allow_ip_count, pItemEnd - pItem)) != 0)
				{
					return result;
				}
				continue;
			}

			addr = getIpaddrByName(pItem->value, NULL, 0);
			if (addr == INADDR_NONE)
			{
				logWarning("file: "__FILE__", line: %d, " \
					"invalid host name: %s", \
					__LINE__, pItem->value);
			}
			else
			{
				if ((result=check_realloc_allow_ips(allow_ip_addrs,
					&alloc_count, (*allow_ip_count) + (pItemEnd - pItem))) != 0)
				{
					return result;
				}

				(*allow_ip_addrs)[*allow_ip_count] = addr;
				(*allow_ip_count)++;
			}

			continue;
		}

		
		pEnd = strchr(pStart, ']');
		if (pEnd == NULL)
		{
			logError("file: "__FILE__", line: %d, " \
				"invalid host name: %s, expect \"]\"", \
				__LINE__, pItem->value);
			return EINVAL;
		}

		nValueLen = strlen(pItem->value);
		if (nValueLen >= (int)sizeof(item_value))
		{
			logError("file: "__FILE__", line: %d, " \
				"hostname too long, exceeds %d bytes", \
				__LINE__, (int)sizeof(item_value));
			return EINVAL;
		}
		memcpy(item_value, pItem->value, nValueLen + 1);
		nHeadLen = pStart - pItem->value;
		memcpy(hostname, pItem->value, nHeadLen);

		result = parse_range_hosts(pItem->value, item_value + nHeadLen,
				item_value + (pEnd - pItem->value),
				hostname, nHeadLen, allow_ip_addrs,
				&alloc_count, allow_ip_count, pItemEnd - pItem);
		if (result != 0)
		{
			return result;
		}
	}

	if (*allow_ip_count == 0)
	{
		logWarning("file: "__FILE__", line: %d, " \
			"allow ip count: 0", __LINE__);
	}

	if (*allow_ip_count > 0)
	{
		qsort(*allow_ip_addrs,  *allow_ip_count, sizeof(in_addr_t), \
			cmp_by_ip_addr_t);
	}

	logDebug("allow_ip_count=%d", *allow_ip_count);
	for (i=0; i<*allow_ip_count; i++)
	{
		struct in_addr address;
        char buff[INET_ADDRSTRLEN];
		address.s_addr = (*allow_ip_addrs)[i];
		logDebug("%d. %s", i + 1, inet_ntop(AF_INET, &address,
                    buff, sizeof(buff)));
	}

	return 0;
}

int cmp_by_ip_addr_t(const void *p1, const void *p2)
{
        return memcmp((in_addr_t *)p1, (in_addr_t *)p2, sizeof(in_addr_t));
}

int parse_bytes(char *pStr, const int default_unit_bytes, int64_t *bytes)
{
	char *pReservedEnd;

	pReservedEnd = NULL;
	*bytes = strtol(pStr, &pReservedEnd, 10);
	if (*bytes < 0)
	{
		logError("file: "__FILE__", line: %d, " \
			"bytes: %"PRId64" < 0", __LINE__, *bytes);
		return EINVAL;
	}

	if (pReservedEnd == NULL || *pReservedEnd == '\0')
	{
		*bytes *= default_unit_bytes;
	}
	else if (*pReservedEnd == 'G' || *pReservedEnd == 'g')
	{
		*bytes *= 1024 * 1024 * 1024;
	}
	else if (*pReservedEnd == 'M' || *pReservedEnd == 'm')
	{
		*bytes *= 1024 * 1024;
	}
	else if (*pReservedEnd == 'K' || *pReservedEnd == 'k')
	{
		*bytes *= 1024;
	}

	return 0;
}

int set_rand_seed()
{
	struct timeval tv;

	if (gettimeofday(&tv, NULL) != 0)
	{
		logError("file: "__FILE__", line: %d, " \
			 "call gettimeofday fail, " \
			 "errno=%d, error info: %s", \
			 __LINE__, errno, STRERROR(errno));
		return errno != 0 ? errno : EPERM;
	}

	srand(tv.tv_sec ^ tv.tv_usec);
	return 0;
}

int get_time_item_from_conf(IniContext *pIniContext, \
		const char *item_name, TimeInfo *pTimeInfo, \
		const byte default_hour, const byte default_minute)
{
	char *pValue;
	pValue = iniGetStrValue(NULL, item_name, pIniContext);
    return get_time_item_from_str(pValue, item_name, pTimeInfo,
		default_hour, default_minute);
}

int get_time_item_from_str(const char *pValue, const char *item_name,
        TimeInfo *pTimeInfo, const byte default_hour,
        const byte default_minute)
{
	int hour;
	int minute;
	int second;
    int count;

	if (pValue == NULL)
	{
		pTimeInfo->hour = default_hour;
		pTimeInfo->minute = default_minute;
		pTimeInfo->second = 0;
		return 0;
	}

    second = 0;
    count = sscanf(pValue, "%d:%d:%d", &hour, &minute, &second);
	if (count != 2 && count != 3)
	{
		logError("file: "__FILE__", line: %d, " \
			"item \"%s\" 's value \"%s\" is not an valid time", \
			__LINE__, item_name, pValue);
		return EINVAL;
	}

	if ((hour < 0 || hour > 23) || (minute < 0 || minute > 59)
             || (second < 0 || second > 59))
	{
		logError("file: "__FILE__", line: %d, " \
			"item \"%s\" 's value \"%s\" is not an valid time", \
			__LINE__, item_name, pValue);
		return EINVAL;
	}

	pTimeInfo->hour = (byte)hour;
	pTimeInfo->minute = (byte)minute;
	pTimeInfo->second = (byte)second;

	return 0;
}

char *urlencode(const char *src, const int src_len, char *dest, int *dest_len)
{
	static unsigned char hex_chars[] = "0123456789ABCDEF";
	const unsigned char *pSrc;
	const unsigned char *pEnd;
	char *pDest;

	pDest = dest;
	pEnd = (unsigned char *)src + src_len;
	for (pSrc=(unsigned char *)src; pSrc<pEnd; pSrc++)
	{
		if ((*pSrc >= '0' && *pSrc <= '9') || 
	 	    (*pSrc >= 'a' && *pSrc <= 'z') ||
	 	    (*pSrc >= 'A' && *pSrc <= 'Z') ||
		    (*pSrc == '_' || *pSrc == '-' || *pSrc == '.'))
		{
			*pDest++ = *pSrc;
		}
		else if (*pSrc == ' ')
		{
			*pDest++ = '+';
		}
		else
		{
			*pDest++ = '%';
			*pDest++ = hex_chars[(*pSrc) >> 4];
			*pDest++ = hex_chars[(*pSrc) & 0x0F];
		}
	}

	*pDest = '\0';
	*dest_len = pDest - dest;

	return dest;
}

char *urldecode(const char *src, const int src_len, char *dest, int *dest_len)
{
    (void)urldecode_ex(src, src_len, dest, dest_len);
    *(dest + *dest_len) = '\0';
    return dest;
}

char *urldecode_ex(const char *src, const int src_len, char *dest, int *dest_len)
{
#define IS_HEX_CHAR(ch) \
	((ch >= '0' && ch <= '9') || \
	 (ch >= 'a' && ch <= 'f') || \
	 (ch >= 'A' && ch <= 'F'))

#define HEX_VALUE(ch, value) \
	if (ch >= '0' && ch <= '9') \
	{ \
		value = ch - '0'; \
	} \
	else if (ch >= 'a' && ch <= 'f') \
	{ \
		value = ch - 'a' + 10; \
	} \
	else \
	{ \
		value = ch - 'A' + 10; \
	}

	const unsigned char *pSrc;
	const unsigned char *pEnd;
	char *pDest;
	unsigned char cHigh;
	unsigned char cLow;
	int valHigh;
	int valLow;

	pDest = dest;
	pSrc = (unsigned char *)src;
	pEnd = (unsigned char *)src + src_len;
	while (pSrc < pEnd)
	{
		if (*pSrc == '%' && pSrc + 2 < pEnd)
		{
			cHigh = *(pSrc + 1);
			cLow = *(pSrc + 2);

			if (IS_HEX_CHAR(cHigh) && IS_HEX_CHAR(cLow))
			{
				HEX_VALUE(cHigh, valHigh)
				HEX_VALUE(cLow, valLow)
				*pDest++ = (valHigh << 4) | valLow;
				pSrc += 3;
			}
			else
			{
				*pDest++ = *pSrc;
				pSrc++;
			}
		}
		else if (*pSrc == '+')
		{
			*pDest++ = ' ';
			pSrc++;
		}
		else
		{
			*pDest++ = *pSrc;
			pSrc++;
		}
	}

	*dest_len = pDest - dest;
	return dest;
}

int buffer_strcpy(BufferInfo *pBuff, const char *str)
{
	pBuff->length = strlen(str);
	if (pBuff->alloc_size <= pBuff->length)
	{
		if (pBuff->buff != NULL)
		{
			free(pBuff->buff);
		}

		pBuff->alloc_size = pBuff->length + 1;
		pBuff->buff = (char *)malloc(pBuff->alloc_size);
		if (pBuff->buff == NULL)
		{
			logError("file: "__FILE__", line: %d, " \
				"malloc %d bytes fail, " \
				"errno: %d, error info: %s", \
				__LINE__, pBuff->alloc_size, \
				errno, STRERROR(errno));
			pBuff->alloc_size = 0;
			return errno != 0 ? errno : ENOMEM;
		}
	}

	memcpy(pBuff->buff, str, pBuff->length + 1);
	return 0;
}

int buffer_memcpy(BufferInfo *pBuff, const char *buff, const int len)
{
	pBuff->length = len;
	if (pBuff->alloc_size <= pBuff->length)
	{
		if (pBuff->buff != NULL)
		{
			free(pBuff->buff);
		}

		pBuff->alloc_size = pBuff->length;
		pBuff->buff = (char *)malloc(pBuff->alloc_size);
		if (pBuff->buff == NULL)
		{
			logError("file: "__FILE__", line: %d, " \
				"malloc %d bytes fail, " \
				"errno: %d, error info: %s", \
				__LINE__, pBuff->alloc_size, \
				errno, STRERROR(errno));
			pBuff->alloc_size = 0;
			return errno != 0 ? errno : ENOMEM;
		}
	}

	memcpy(pBuff->buff, buff, pBuff->length);
	return 0;
}

int set_timer(const int first_remain_seconds, const int interval, \
		void (*sighandler)(int))
{
	struct itimerval value;
	struct sigaction act;

	memset(&act, 0, sizeof(act));
	sigemptyset(&act.sa_mask);
	act.sa_handler = sighandler;
	if(sigaction(SIGALRM, &act, NULL) < 0)
	{
		logError("file: "__FILE__", line: %d, " \
			"call sigaction fail, errno: %d, error info: %s", \
			__LINE__, errno, STRERROR(errno));
		return errno != 0 ? errno : EINVAL;
	}

	memset(&value, 0, sizeof(value));
	value.it_interval.tv_sec = interval;
	value.it_value.tv_sec = first_remain_seconds;
	if (setitimer(ITIMER_REAL, &value, NULL) < 0)
	{
		logError("file: "__FILE__", line: %d, " \
			"call setitimer fail, errno: %d, error info: %s", \
			__LINE__, errno, STRERROR(errno));
		return errno != 0 ? errno : EINVAL;
	}

	return 0;
}

int set_file_utimes(const char *filename, const time_t new_time)
{
	struct timeval tvs[2];

	tvs[0].tv_sec = new_time;
	tvs[0].tv_usec = 0;
	tvs[1].tv_sec = new_time;
	tvs[1].tv_usec = 0;
	if (utimes(filename, tvs) != 0)
	{
		logWarning("file: "__FILE__", line: %d, " \
			"call utimes file: %s fail" \
			", errno: %d, error info: %s", \
			__LINE__, filename, errno, STRERROR(errno));
		return errno != 0 ? errno : ENOENT;
	}

	return 0;
}

int ignore_signal_pipe()
{
	struct sigaction act;

	memset(&act, 0, sizeof(act));
	sigemptyset(&act.sa_mask);
	act.sa_handler = SIG_IGN;
	if(sigaction(SIGPIPE, &act, NULL) < 0)
	{
		logError("file: "__FILE__", line: %d, " \
			"call sigaction fail, errno: %d, error info: %s", \
			__LINE__, errno, STRERROR(errno));
		return errno;
	}

	return 0;
}

double get_line_distance_km(const double lat1, const double lon1,
        const double lat2, const double lon2)
{
#define FAST_ABS(v) ((v) >= 0 ? (v) : -1 * (v))
#define DISTANCE_PER_LATITUDE 111.111

    double lat_value;
    double lng_distance;
    double lat_distance;

    lat_value = FAST_ABS(lat1) < FAST_ABS(lat2) ? lat1 : lat2;
    lat_distance = FAST_ABS(lat1 - lat2) * DISTANCE_PER_LATITUDE;
    lng_distance = FAST_ABS(lon1 - lon2) * DISTANCE_PER_LATITUDE *
        cos(lat_value * 3.1415926 / 180.0);

    return sqrt(lat_distance * lat_distance + lng_distance * lng_distance);
}

bool is_private_ip(const char* ip)
{
    if (ip == NULL || (int)strlen(ip) < 8)
    {
        return false;
    }

    if (memcmp(ip, "10.", 3) == 0 || memcmp(ip, "192.168.", 8) == 0)
    {
        return true;
    }
    if (memcmp(ip, "172.", 4) == 0)
    {
        int b;
        b = atoi(ip + 4);
        if (b >= 16 && b < 32)
        {
            return true;
        }
    }

    return false;
}

int64_t get_current_time_us()
{
	struct timeval tv;

	if (gettimeofday(&tv, NULL) != 0)
	{
		logError("file: "__FILE__", line: %d, " \
			 "call gettimeofday fail, " \
			 "errno: %d, error info: %s", \
			 __LINE__, errno, STRERROR(errno));
		return errno != 0 ? errno : EPERM;
	}

	return ((int64_t)tv.tv_sec * 1000 * 1000 + (int64_t)tv.tv_usec);
}

inline bool is_power2(const int64_t n)
{
    return ((n != 0) && !(n & (n - 1)));	
}

static inline int do_lock_file(int fd, int cmd, int type)
{
    struct flock lock;
    int result;

    memset(&lock, 0, sizeof(lock));
    lock.l_type = type;
    lock.l_whence = SEEK_SET;
    do
    {
        if ((result=fcntl(fd, cmd, &lock)) != 0)
        {
            result = errno != 0 ? errno : ENOMEM;
            fprintf(stderr, "call fcntl fail, "
                   "errno: %d, error info: %s\n",
                   result, STRERROR(result));
        }
    } while (result == EINTR);

    return result;
}

int file_read_lock(int fd)
{
    return do_lock_file(fd, F_SETLKW, F_RDLCK);
}

int file_write_lock(int fd)
{
    return do_lock_file(fd, F_SETLKW, F_WRLCK);
}

int file_unlock(int fd)
{
    return do_lock_file(fd, F_SETLKW, F_UNLCK);
}

int file_try_read_lock(int fd)
{
    return do_lock_file(fd, F_SETLK, F_RDLCK);
}

int file_try_write_lock(int fd)
{
    return do_lock_file(fd, F_SETLK, F_WRLCK);
}

int file_try_unlock(int fd)
{
    return do_lock_file(fd, F_SETLK, F_UNLCK);
}

bool isLeadingSpacesLine(const char *content, const char *current)
{
    const char *p;
    p = current - 1;
    while (p >= content)
    {
        if (!(*p == ' ' || *p == '\t'))
        {
            break;
        }
        --p;
    }
    return (p < content || *p == '\n');
}

bool isTrailingSpacesLine(const char *tail, const char *end)
{
    const char *p;
    p = tail;
    while (p < end)
    {
        if (!(*p == ' ' || *p == '\t'))
        {
            break;
        }
        ++p;
    }
    return (p == end || *p == '\n');
}

ssize_t fc_safe_write(int fd, const char *buf, const size_t nbyte)
{
    ssize_t n;
    ssize_t remain;
    const char *p;

    n = write(fd, buf, nbyte);
    if (n < 0)
    {
        if (errno != EINTR)
        {
            return -1;
        }
        n = 0;
    }
    else if (n == nbyte)
    {
        return nbyte;
    }

    p = buf + n;
    remain = nbyte - n;
    while (remain > 0)
    {
        n = write(fd, p, remain);
        if (n < 0)
        {
            ssize_t written;
            if (errno == EINTR)
            {
                continue;
            }

            written = nbyte - remain;
            return written > 0 ? written : -1;
        }

        p += n;
        remain -= n;
    }

    return nbyte;
}

ssize_t fc_lock_write(int fd, const char *buf, const size_t nbyte)
{
    int lock_result;
    ssize_t result;

    lock_result = file_write_lock(fd);
    result = fc_safe_write(fd, buf, nbyte);
    if (lock_result == 0)
    {
        file_unlock(fd);
    }

    return result;
}

ssize_t fc_safe_read(int fd, char *buf, const size_t count)
{
    ssize_t n;
    ssize_t remain;
    char *p;

    n = read(fd, buf, count);
    if (n < 0)
    {
        if (errno != EINTR)
        {
            return -1;
        }
        n = 0;
    }
    else
    {
        if (n == 0 || n == count)
        {
            return n;
        }
    }

    p = buf + n;
    remain = count - n;
    while (remain > 0)
    {
        n = read(fd, p, remain);
        if (n < 0)
        {
            ssize_t done;
            if (errno == EINTR)
            {
                continue;
            }

            done = count - remain;
            return done > 0 ? done : -1;
        }
        else if (n == 0)
        {
            break;
        }

        p += n;
        remain -= n;
    }

    return count - remain;
}

key_t fc_ftok(const char *path, const int proj_id)
{
    int hash_code;
    hash_code = simple_hash(path, strlen(path));
    return (((proj_id & 0xFF) << 24) | (hash_code & 0xFFFFFF));
}
