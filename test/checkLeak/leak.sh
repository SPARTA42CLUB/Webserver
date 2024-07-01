#!/bin/bash

# webserv의 PID 찾기
get_pid() {
    ps aux | grep '[w]ebserv' | awk '{print $2}'
}

# 현재 스크립트 위치에서 두 디렉토리 위로 이동
WEBSEV_DIR=$(dirname $(dirname $(realpath $0)))

# webserv의 PID 확인
while true; do
    PID=$(get_pid)
    if [ -z "$PID" ]; then
        echo "webserv 프로세스를 찾을 수 없습니다."
        exit 1
    fi
    leaks --list $PID | grep 'total leaked bytes'
    sleep 1
done
