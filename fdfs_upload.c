/**
* Copyright (C) 2008 Happy Fish / YuQing
*
* FastDFS may be copied only under the terms of the GNU General
* Public License V3, which may be found in the FastDFS source kit.
* Please visit the FastDFS Home Page http://www.csource.org/ for more detail.
**/

#include "fdfs_upload.h"

int upload_file1(const char *confFile,const char* myFile, char* fileID)
{
	char group_name[FDFS_GROUP_NAME_MAX_LEN + 1];
	ConnectionInfo *pTrackerServer;
	int result;
	int store_path_index;
	ConnectionInfo storageServer;


	log_init();
	g_log_context.log_level = LOG_ERR;
	ignore_signal_pipe();

	if ((result=fdfs_client_init(confFile)) != 0)
	{
		return result;
	}

	pTrackerServer = tracker_get_connection();
	if (pTrackerServer == NULL)
	{
		fdfs_client_destroy();
		return errno != 0 ? errno : ECONNREFUSED;
	}

	*group_name = '\0';
	
	if((result=tracker_query_storage_store(pTrackerServer, \
	                &storageServer, group_name, &store_path_index)) != 0)
	{
		fdfs_client_destroy();
		fprintf(stderr, "tracker_query_storage fail, " \
			"error no: %d, error info: %s\n", \
			result, STRERROR(result));
		return result;
	}

	result = storage_upload_by_filename1(pTrackerServer, \
			&storageServer, store_path_index, \
			myFile, NULL, \
			NULL, 0, group_name, fileID);
	if (result == 0)
	{
		printf("%s\n", fileID);
	}
	else
	{
		fprintf(stderr, "upload file fail, " \
			"error no: %d, error info: %s\n", \
			result, STRERROR(result));
	}

	tracker_disconnect_server_ex(pTrackerServer, true);
	fdfs_client_destroy();

	return result;
}

int upload_file2(const char *confFile,const char* myFile, char* fileID,int size)
{
    // 创建管道
    int fd[2];
    int ret =pipe(fd);
    if(ret ==-1){
        perror("pipe error");
        exit(EXIT_FAILURE);
    }
    pid_t pid = fork();
    if(pid == 0){
        // 子进程
        close(fd[0]); // 关闭读端
        dup2(fd[1],STDOUT_FILENO);
        execlp("fdfs_upload_file","fdfs_upload_file",confFile,myFile,NULL);
        perror("execlp error");
        exit(EXIT_FAILURE);

    }else if(pid>0){
        close(fd[1]);
        read(fd[0],fileID,size);
        wait(NULL);
    }else{
        perror("fork error");
        exit(EXIT_FAILURE);
    }
}
