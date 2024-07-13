cmake_minimum_required(VERSION 3.10)
project(HF_DFS)


set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall")

# .h
include_directories(
    ./include
    /usr/include/fastdfs
    /usr/include/fastcommon
    /usr/local/include/hiredis
    /usr/include/mysql
)

# .so
link_directories(
    /usr/lib
)

# 共用的源代码文件列表:COMMON_SRC
set(COMMON_SRC
    ${CMAKE_SOURCE_DIR}/common/make_log.c
    ${CMAKE_SOURCE_DIR}/common/cJSON.c
    ${CMAKE_SOURCE_DIR}/common/deal_mysql.c
    ${CMAKE_SOURCE_DIR}/common/redis_op.c
    ${CMAKE_SOURCE_DIR}/common/config.c
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
foreach(cgi_bin ${CGI_BINARIES})
    add_executable(${cgi_bin}
        cgi/${cgi_bin}_cgi.c
        ${COMMON_SRC}
    )
    target_link_libraries(${cgi_bin} fdfsclient fastcommon hiredis fcgi m mysqlclient)
endforeach()

# 设置输出路径
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/build/bin)

# 添加清理命令
add_custom_target(clean-all
    COMMAND ${CMAKE_COMMAND} -P ${CMAKE_SOURCE_DIR}/CleanAll.cmake
)
