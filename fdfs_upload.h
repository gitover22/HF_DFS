#ifndef FDFS_UPLOAD_H
#define FDFS_UPLOAD_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "fdfs_client.h"


int upload_file1(const char *confFile,const char* myFile, char* fileID);
int upload_file2(const char *confFile,const char* myFile, char* fileID,int size);

#endif
