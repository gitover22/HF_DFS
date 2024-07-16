#!/bin/bash

NAME=../redis
FILE=redis.pid
is_directory()
{
    if [ ! -d $1 ]; then
        echo "mkdir: $1 "
        mkdir $1
        if [ $? -ne 0 ];then
            echo "mkdir $1 fail"
            exit 1
        fi
    fi
}


is_regular_file()
{
    if [ ! -f $1 ]; then
        echo "$1 file not exist..."
        return 1
    fi
    return 0 # true
}

if [[ $1 = "" ]];then
    echo "please input argument:"
    echo "  start: start redis server"
    echo "  stop: stop redis server"
    echo "  status: show the redis server status"
    exit 1
fi
# check redis directory
is_directory $NAME

case $1 in
    start)
        ps aux | grep "redis-server" | grep -v grep > /dev/null
        if [ $? -eq 0 ];then
            echo "Redis server is runing ..."
        else
            unlink "$NAME/$FILE"

            echo "Redis starting ..."
            redis-server ../conf/redis.conf
            if [ $? -eq 0 ];then
                echo "Redis server start success!!!"
                sleep 1
                if is_regular_file "$NAME/$FILE";then
                    printf "Redis server PID: [ %s ]\n" $(cat "$NAME/$FILE")
                    printf "Redis server PORT: [ %s ]\n" $(awk '/^port /{print $2}' "../conf/redis.conf")
                fi
            fi
        fi
        ;;
    stop)
        ps aux | grep "redis-server" | grep -v grep > /dev/null
        if [ $? -ne 0 ];then
            echo "Redis server is not runing..."
            exit 1
        fi
        echo "Redis stopping ..."
        if is_regular_file "$NAME/$FILE"; then
            echo "close process by redis.pid"
            PID=$(cat "$NAME/$FILE")
        else
            echo "close process by pid"
            PID=$(ps aux | grep "redis-server" | grep -v grep | awk '{print $2}')
        fi
        echo Redis server pid = $PID
        kill -9 $PID
        if [ $? -ne 0 ]; then
            echo "Redis server stop fail ..."
        else
            echo "Redis server stop success!!!"
        fi
        ;;
    status)
        ps aux | grep "redis-server" | grep -v grep > /dev/null
        if [ $? -eq 0 ];then
            echo "Redis server is running..."
        else
            echo "Redis server is not running ..."
        fi
        ;;
    *)
        echo "do nothing ..."
        ;;
esac
