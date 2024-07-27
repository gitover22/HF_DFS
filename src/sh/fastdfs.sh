#!/bin/bash

tracker_start()
{
    ps aux | grep fdfs_trackerd | grep -v grep > /dev/null
    if [ $? -eq 0 ];then
        echo "fdfs_trackerd running already ..."
    else
        sudo fdfs_trackerd  /etc/fdfs/tracker.conf
        if [ $? -eq 0 ];then
            echo "tracker start success ..."
        else
            echo "tracker start failed ..."
        fi
    fi
}

storage_start()
{
    ps aux | grep fdfs_storaged | grep -v grep > /dev/null
    if [ $? -eq 0 ];then
        echo "fdfs_trackerd running already ..."
    else
        sudo fdfs_storaged  /etc/fdfs/storage.conf
        if [ $? -eq 0 ];then
            echo "storage start success ..."
        else
            echo "storage start failed ..."
        fi
    fi
}

main() {
    if [ $# -eq 0 ];then
        echo "Please input the argument:"
        echo "start storage please input argument: storage"
        echo "start tracker please input argument: tracker"
        echo "start storage && tracker please input argument: start"
        echo "stop storage && tracker input argument: stop"
        exit 0
    fi
    case $1 in
        storage)
            storage_start
            ;;
        tracker)
            tracker_start
            ;;
        start)
            storage_start
            tracker_start
            ;;
        stop)
            sudo fdfs_trackerd /etc/fdfs/tracker.conf stop
            sudo fdfs_storaged /etc/fdfs/storage.conf stop
            ;;
        *)
            echo "error argument"
    esac
}

main "$@"