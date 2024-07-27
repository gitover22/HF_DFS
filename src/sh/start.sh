#!/bin/bash

GREEN='\033[0;32m'
NC='\033[0m' # No Color

echo_color() {
    echo -e "${GREEN}$1${NC}"
}

# restart
restart_service() {
    echo_color "restart $1 :"
    ./$1.sh stop
    ./$1.sh start
    echo
}

# copy
update_nginx_config() {
    echo_color "copy nginx.conf to /usr/local/nginx/conf"
    sudo mv /usr/local/nginx/conf/nginx.conf /usr/local/nginx/conf/nginx.conf.old
    sudo cp ../conf/nginx.conf /usr/local/nginx/conf
    echo_color "cp nginx.conf success"
    echo
}
show_logo() {
    echo -e "${GREEN}"
    echo "  _    _ _______ _____  _______  _____ "
    echo " | |  | |  _____|  __ \|  _____|/ ____|"
    echo " | |__| |  |__  | |  | |  |__  | (___  "
    echo " |  __  |  |__| | |  | |  |__|  \___ \ "
    echo " | |  | |  |    | |__| |  |     ____) |"
    echo " |_|  |_|__|    |_____/|__|    |_____/  "
    echo -e "${NC}"
}
main() {
    clear
    sudo echo ""
    show_logo
    update_nginx_config
    restart_service "fastdfs"
    restart_service "fcgi"
    restart_service "nginx"
    restart_service "redis"
}
main "$@"