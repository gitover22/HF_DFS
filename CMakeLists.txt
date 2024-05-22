cmake_minimum_required(VERSION 3.0)
project(Test)

# 设置编译器为 clang
set(CMAKE_C_COMPILER gcc)
set(CMAKE_CXX_COMPILER g++)

# 设置 C++ 标准和编译选项
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")

# 编译器选项
add_definitions("-g -Wall -std=c++11")

# 设置头文件搜索路径 
include_directories("/home/huafeng/HF_DFS/fastDFS/fastdfs-5.10/client/")
include_directories("/home/huafeng/HF_DFS/fastDFS/fastdfs-5.10/tracker/")
include_directories("/home/huafeng/HF_DFS/fastDFS/fastdfs-5.10/common/")
include_directories("/home/huafeng/HF_DFS/fastDFS/libfastcommon-master/src/")

# 添加链接库的搜索路径  .so等搜索路径 也需要把库名添加到 target_link_libraries 中
link_directories("/home/huafeng/HF_DFS/fastDFS/fastdfs-5.10/client/") 
link_directories("/home/huafeng/HF_DFS/fastDFS/libfastcommon-master/src/") 

# 添加可执行文件生成规则
add_executable(test fdfs_upload.c main.c)

# 链接所需的库
target_link_libraries(test fdfsclient fastcommon)