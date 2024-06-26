### 服务器部署步骤:
#### 1. 执行make生成可执行程序(需要使用的cgi程序)
#### 2. 修改cfg.json中的相关数据
#### 3. 配置好fastDFS, 保证已经可以正常使用
#### 4. 修改当前目录下的nginx.conf配置文件
```
location /group1/M00{
        root /home/huafeng/HF_DFS/fastDFS/storage_files/data;
        ngx_fastdfs_module;
}
```
找到该配置项, 需要修改root对应的路径, 修改为自己fastDFS Storage的数据存储目录
该文件最终会被拷贝到nginx默认的配置文件目录中: /usr/local/nginx/conf
#### 5. 执行脚本文件
```
./start.sh
```