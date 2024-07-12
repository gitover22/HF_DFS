cmake_minimum_required(VERSION 3.10)  # CMake最小版本
project(HF_DFS)  # 项目

# 设置C++编译器标志
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall")

# 包含的头文件目录
include_directories(
    ./include
    /usr/include/fastdfs
    /usr/include/fastcommon
    /usr/local/include/hiredis
    /usr/include/mysql
)

# 链接的库文件目录
link_directories(
    /usr/lib
)

# 添加common目录下的源文件
set(COMMON_SRC
    ${CMAKE_SOURCE_DIR}/common/make_log.c
    ${CMAKE_SOURCE_DIR}/common/cJSON.c
    ${CMAKE_SOURCE_DIR}/common/deal_mysql.c
    ${CMAKE_SOURCE_DIR}/common/redis_op.c
    ${CMAKE_SOURCE_DIR}/common/cfg.c
    ${CMAKE_SOURCE_DIR}/common/util_cgi.c
    ${CMAKE_SOURCE_DIR}/common/des.c
    ${CMAKE_SOURCE_DIR}/common/base64.c
    ${CMAKE_SOURCE_DIR}/common/md5.c
)

# 添加测试目标
add_executable(main
    test/main.c
    test/fdfs_api.c
    ${COMMON_SRC}
)
target_link_libraries(main fdfsclient fastcommon hiredis fcgi m mysqlclient)

add_executable(redis
    test/myredis.c
    ${COMMON_SRC}
)
target_link_libraries(redis fdfsclient fastcommon hiredis fcgi m mysqlclient)

# 添加各CGI可执行目标
set(CGI_BINARIES login register upload md5 myfiles dealfile sharefiles dealsharefile)
foreach(cgi ${CGI_BINARIES})
    add_executable(${cgi}
        src_cgi/${cgi}_cgi.c
        ${COMMON_SRC}
    )
    target_link_libraries(${cgi} fdfsclient fastcommon hiredis fcgi m mysqlclient)
endforeach()

# 设置输出路径
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/bin_cgi)

# 添加清理命令
add_custom_target(clean-all
    COMMAND ${CMAKE_COMMAND} -P ${CMAKE_SOURCE_DIR}/CleanAll.cmake
)
