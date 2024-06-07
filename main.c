#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "fdfs_upload.h"
int main(int argc, char *argv[]){
    char fileID[1024]={0};
    upload_file1("/etc/fdfs/client.conf.sample",argv[1],fileID);
    printf("%s\n",fileID);
    upload_file2("/etc/fdfs/client.conf.sample",argv[1],fileID,sizeof(fileID));
    printf("%s\n",fileID);
    return 0;
}