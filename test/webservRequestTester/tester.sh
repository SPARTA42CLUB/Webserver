#!/bin/bash

CR="\r"

# 테스트할 요청과 기대하는 응답 코드 배열
requests=(
"GET / HTTP/1.1$CR
Host: localhost$CR
Connection: close$CR
Content-Length: 10000000$CR
Number: 1$CR
Connection: close$CR
$CR
dsadsadsa" 413

"GET / HTTP/1.1$CR
Host: localhost$CR
Connection: close$CR
Content-Length: 0$CR
Connection: close$CR
Number: 2$CR
$CR
" 200

"GET /error HTTP/1.1$CR
Host: localhost$CR
Connection: close$CR
Number: 3$CR
$CR
" 404

"POST / HTTP/1.1$CR
Host: localhost$CR
Connection: close$CR
Content-Length: 1$CR
Number: 4$CR
$CR
a" 200

"GET / HTTP/1.1$CR
Host: localhost:8080$CR
Connection: close$CR
Number: 5$CR
$CR
" 200

"GET / HTTP/1.1$CR
Number: 6$CR
$CR
" 400

"GET / HTTP/1.1$CR
Host: seunan:8081$CR
Connection: close$CR
Number: 7$CR
$CR
" 200

"GET /permission_denied/forbidden HTTP/1.1$CR
Host: localhost:8080$CR
Connection: close$CR
Number: 8$CR
$CR
" 403

"GET / HTTP/1.1$CR
Host: localhost:8080$CR
Connection: close$CR
Number: 9$CR
$CR
" 200

"GET / HTTP/1.1$CR
Host: localhost:8080$CR
Connection: close$CR
Number: 10$CR
$CR
" 200

"GET / HTTP/1.1$CR
Host: localhost:8080$CR
Connection: close$CR
Date: Sun Nov 6 08:49:37 1994$CR
Number: 11$CR
$CR
" 200

"GET / HTTP/1.1       a$CR
Host: localhost:8080$CR
Connection: close$CR
Number: 12$CR
$CR
" 400

"GET / HTTP/1.1$CR
Host: localhost:8080$CR
Connection: close$CR
Number: 13$CR
$CR
Connection: close$CR
Content-Type: text/html$CR
$CR
bodybody" 200

"GET / HTTP/1.1$CR
Host: localhost:8080$CR
Connection: close$CR
Content-Type: text/html$CR
Number: 14$CR
$CR
bodybody" 200

"GET /uritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolonguritoolong HTTP/1.1$CR
Host: localhost:8080$CR
Connection: close$CR
Content-Type: text/html$CR
Number: 15$CR
$CR
" 414

"DELETE /notfound.html HTTP/1.1$CR
Host: localhost:8080$CR
Connection: close$CR
Number: 16$CR
$CR
" 404

"DELETE /delete HTTP/1.1$CR
Host: localhost:8080$CR
Connection: close$CR
Number: 17$CR
$CR
" 200

"DELETE /permission_denied/not_allow HTTP/1.1$CR
Host: localhost:8080$CR
Connection: close$CR
Number: 18$CR
$CR
" 403

"GET / HTTP/1.1$CR
Host: localhost$CR
Connection: close$CR
Content-Length: 100$CR
Number: 19$CR
Connection: close$CR
$CR
dsadsadsadsadsadsadsadsaddsadsadsadsadsadsadsadsaddsadsadsadsadsadsadsadsaddsadsadsadsadsadsadsadsad" 200

"GET / HTTP/1.1$CR
Host: localhost$CR
Connection: close$CR
Content-Length: 101$CR
Number: 20$CR
Connection: close$CR
$CR
dsadsadsadsadsadsadsadsaddsadsadsadsadsadsadsadsaddsadsadsadsadsadsadsadsaddsadsadsadsadsadsadsadsad" 413

"GET / HTTP/1.1$CR
Host: localhost$CR
Connection: close$CR
Content-Length: 100$CR
Number: 21$CR
Connection: close$CR
$CR
dsadsadsadsadsadsadsadsaddsadsadsadsadsadsadsadsaddsadsadsadsadsadsadsadsaddsadsadsadsadsadsadsadsada" 200

"PUT / HTTP/1.1$CR
Host: localhost$CR
Connection: close$CR
Content-Length: 100$CR
Number: 22$CR
$CR
" 405

"GET / HTTP/1.2$CR
Host: localhost$CR
Connection: close$CR
Number: 23$CR
$CR
" 505

"GET /index.py HTTP/1.1$CR
Host: localhost$CR
Connection: close$CR
Number: 24$CR
$CR
" 200

"GET /redirect HTTP/1.1$CR
Host: localhost$CR
Connection: close$CR
Number: 25$CR
$CR
" 302

"POST /upload HTTP/1.1$CR
Host: localhost$CR
Connection: close$CR
Content-Length: 100$CR
Number: 26$CR
Content-Disposition: form-data; name=\"file\"; filename=\"test.txt\"$CR
Content-Type: text/plain$CR
$CR
dsadsadsadsadsadsadsadsaddsadsadsadsadsadsadsadsaddsadsadsadsadsadsadsadsaddsadsadsadsadsadsadsadsad" 201

"POST /upload/index.html HTTP/1.1$CR
Host: localhost$CR
Connection: close$CR
Content-Length: 101$CR
Number: 27$CR
Content-Disposition: form-data; name=\"file\"; filename=\"test.txt\"$CR
Content-Type: text/html$CR
$CR
<html><body><h1>It works!</h1><p>lorem ipsumlorem ipsumlorem ipsumlorem ipsum dasad</p></body></html>" 413

"POST /upload/index.html HTTP/1.1$CR
Host: localhost$CR
Connection: close$CR
Content-Length: 100$CR
Number: 28$CR
Content-Disposition: form-data; name=\"file\"; filename=\"test.txt\"$CR
Content-Type: text/html$CR
$CR
<html><body><h1>It works!</h1><p>lorem ipsumlorem ipsumlorem ipsumlorem ipsumipsumlorem ipsumdad</p>" 201

"POST /upload/index.html HTTP/1.1$CR
Host: localhost$CR
Connection: close$CR
Content-Length: 14$CR
Number: 29$CR
Content-Disposition: form-data; name=\"file\"; filename=\"test.txt\"$CR
Content-Type: text/html$CR
$CR
</body></html>" 200

"DELETE /upload/test.txt HTTP/1.1$CR
Host: localhost$CR
Connection: close$CR
Number: 30$CR
$CR
" 200

"DELETE /upload/test.txt HTTP/1.1$CR
Host: localhost$CR
Connection: close$CR
Number: 31$CR
$CR
" 404

"DELETE /upload/index.html HTTP/1.1$CR
Host: localhost$CR
Connection: close$CR
Number: 32$CR
$CR
" 200
)

# ---------------------------------------------------------------------------------------------------

# 웹서버 kqueue, Conpig 설정 시간 (초)
WEBSERVER_CREATE_TIME=1

# 서버 응답 대기 시간 (초)
RESPONSE_WAIT_TIME=0.1

# netcat 명령어 타임아웃 시간 (초)
NC_TIMEOUT=5

# 색상 코드
BLACK='\033[90m'
RED='\033[91m'
GREEN='\033[92m'
YELLOW='\033[93m'
BLUE='\033[94m'
MAGENTA='\033[95m'
CYAN='\033[96m'
WHITE='\033[97m'
NC='\033[0m'

# 테스트를 위한 파일 생성
mkdir -p ../../www/permission_denied log && \
touch ../../www/permission_denied/not_allow ../../www/permission_denied/forbidden ../../www/delete && \
rm -f log/* && \
chmod 000  ../../www/permission_denied/forbidden && \
chmod 444 ../../www/permission_denied/not_allow && \
chmod 544 ../../www/permission_denied/

# webserv 프로그램을 백그라운드에서 실행
clear && echo -e "${WHITE}webserv tester$NC" && \
echo -e "${BLUE}webserver building...$NC" && \
make -C ../../ mem -j > /dev/null 2>&1 && \
echo -e "${BLUE}webserver running...$NC"

../../webserv test.conf & WEBSERV_PID=$! && sleep $WEBSERVER_CREATE_TIME

# 응답을 저장할 파일을 생성 (기존 파일이 있으면 내용을 지우고 새로 생성)
echo -n > unexpected_response.txt

# 요청을 반복하면서 테스트
for ((i=0; i<${#requests[@]}; i+=2)); do
    request="${requests[$i]}"
    expected_status_code="${requests[$((i+1))]}"

    # 정수형 변수 예약
    declare -i status_code

    # 요청을 파일로 저장 (-e 옵션으로 이스케이프 문자 처리)
    # 네트워크 타이밍 문제:
    # nc 명령어는 매우 빠르게 실행되는 도구입니다. 따라서 echo 명령어가 요청을 보내자마자 nc가 연결을 닫아버릴 수 있습니다. sleep을 사용하면 요청을 보낸 후 잠시 동안 연결을 유지하게 되어 서버가 응답을 전송할 시간을 확보하게 됩니다.
    # 서버의 처리 시간:
    # CGI 스크립트가 실행되고 응답을 생성하는 데 시간이 걸릴 수 있습니다. sleep을 통해 연결을 잠시 동안 유지하면 서버가 응답을 생성하여 클라이언트에게 전송할 시간을 가질 수 있습니다. 요청을 보내고 바로 연결을 닫으면 서버가 응답을 전송하기 전에 연결이 끊어질 수 있습니다.
    # 버퍼링 문제:
    # 네트워크 통신에서는 데이터가 버퍼링될 수 있습니다. sleep을 통해 약간의 시간을 주면 데이터가 완전히 전송되고 처리될 수 있습니다. 즉, 클라이언트가 데이터를 전송하고 서버가 이를 완전히 수신 및 처리할 수 있는 시간을 확보하게 됩니다.
    # 프로세스 동기화:
    # nc와 같은 도구는 입력을 읽고 처리하는 동안 매우 짧은 시간 안에 종료될 수 있습니다. sleep을 사용하면 입력을 보낸 후 nc가 조금 더 오래 실행되어 서버의 응답을 기다리게 됩니다. 이는 클라이언트와 서버 간의 동기화 문제를 해결하는 데 도움이 됩니다.
    # 요약하자면, sleep을 사용하여 연결을 잠시 동안 유지하면 클라이언트가 요청을 보낸 후 서버의 응답을 받을 수 있는 충분한 시간을 확보하게 됩니다. 이는 네트워크 타이밍 문제, 서버의 처리 시간, 버퍼링 문제, 프로세스 동기화 문제 등을 해결하는 데 기여할 수 있습니다.
    response=$((echo -ne "${request}"; sleep $RESPONSE_WAIT_TIME) | nc -w$NC_TIMEOUT localhost 8080) # nc -w 1 옵션으로 1초 대기 후 응답 없으면 종료, nc 명령어의 출력을 없애기 위함
    status_code=$(echo "${response}" | awk 'NR==1 {print $2}') # Response의 Status Line에서 HTTP 상태 코드 추출

    # 요청을 보내고 응답 코드 확인
    if [ "$status_code" -eq "$expected_status_code" ]; then
        echo -e "${GREEN}TEST $((i/2+1)): Success (HTTP Status: $status_code)$NC"
    else
        echo -e "${RED}TEST $((i/2+1)): Failed (Expected HTTP Status: $expected_status_code, HTTP Status: $status_code)$NC"
        echo -e "Test $((i/2+1))" >> unexpected_response.txt
        echo -e "\n--------------------------------------REQUEST--------------------------------------" >> unexpected_response.txt
        echo -ne "${request}" >> unexpected_response.txt
        echo -e "\n--------------------------------------RESPONSE--------------------------------------" >> unexpected_response.txt
        echo -ne "${response}" >> unexpected_response.txt
    fi
done

# sudo rm -rf ../../www/permission_denied access.log

# webserv 프로그램 종료
kill -9 $WEBSERV_PID > /dev/null 2>&1
