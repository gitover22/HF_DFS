#!/bin/bash

# array of services
declare -a services=(
    "10000 ../build/bin/login LOGIN"
    "10001 ../build/bin/register REGISTER"
    "10002 ../build/bin/upload UPLOAD"
    "10003 ../build/bin/md5 MD5"
    "10004 ../build/bin/myfiles MYFILES"
    "10005 ../build/bin/dealfile DEALFILE"
    "10006 ../build/bin/sharefiles SHAREFILES"
    "10007 ../build/bin/dealsharefile DEALSHAREFILE"
)

stop_process() {
    echo "close CGI process..."
    for service in "${services[@]}"; do
        read -r port path name <<< "$service"
        local pids=$(pgrep -f "$path")
        if [ -n "$pids" ]; then
            kill $pids > /dev/null 2>&1
        fi
    done
    echo "all closed"
}

start_process() {
    echo "start CGI process..."
    for service in "${services[@]}"; do
        read -r port path name <<< "$service"
        echo -n "$name: "
        spawn-fcgi -a 127.0.0.1 -p "$port" -f "$path"
    done
}

case "$1" in
    start)
        start_process
        ;;
    stop)
        stop_process
        ;;
    *)
    echo "invalid args! use :'start' or 'stop'"
        ;;
esac
