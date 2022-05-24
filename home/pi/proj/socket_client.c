#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include <sys/socket.h>// 소켓 사용 함수
#include <netinet/in.h>
#include <arpa/inet.h>

#include <wiringPi.h>

#define BUFSIZE 8192

#define TCP_PORT 12

#define MOTOR1 0
#define MOTOR2 3
#define MOTOR3 4
#define MOTOR4 5

int srcSocket;
int dstSocket;

// struct 전역 변수 선언
struct sockaddr_in s_addr;
struct sockaddr_in c_addr;
socklen_t len;//소켓길이 및 크기 값
int n;

char textdata[11]="01234567890";
char buf[BUFSIZE];

void server_init(){
	// 소켓 생성 유무 확인
	if((srcSocket = socket(AF_INET,SOCK_STREAM,0))<0){//소켓 생성
		//SOCK_STREAM : 연결 지향형 소켓
		perror("socket error");
		exit(EXIT_FAILURE);
	}

	int on = 1;
	printf("Server Started\n");
	setsockopt(srcSocket,SOL_SOCKET,SO_REUSEADDR,&on,sizeof(on));
	//setsockopt:소켓에 속성값을 변경하는 함수
	//SO_REUSEADDR: 이미 사용된 주소를 재사용하도록한다.

	memset((char*)&s_addr,0,sizeof(s_addr));//메모리 초기화 함수
	s_addr.sin_family = AF_INET;//IPv4인터넷 프로토콜
	s_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	s_addr.sin_port = htons(TCP_PORT);

	//
	if(bind(srcSocket,(struct sockaddr*)&s_addr,sizeof(s_addr))<0){
		//bind :함수 와 객체를 묶는 함수
		perror("bind");
		exit(EXIT_FAILURE);
	}

	listen(srcSocket,5); //연결 요청 소켓이 대기하는 연결 대기열 생성 함수

	len=sizeof(dstSocket);

}

void server_main(){
	if((dstSocket=accept(srcSocket,(struct sockaddr*)&c_addr,&len))<0){
		//accept:연결대기 중 클라이언트의 연결 요청을 수락하는 기능 함수
		perror("accept error");
		exit(EXIT_FAILURE);
	}
	printf("connected from '%s'\n",inet_ntoa(s_addr.sin_addr));
	//inet_ntoa: 인터넷 네트웍 주소(ipv4)를 인터넷 표준표기법 형태의 주소로 변환
}

int main(){
	int stop,up,down,left,right;
	if(wiringPiSetup() == -1){
		return 1;
	}
	pinMode(MOTOR1,OUTPUT);
	pinMode(MOTOR2,OUTPUT);
	pinMode(MOTOR3,OUTPUT);
	pinMode(MOTOR4,OUTPUT);

	server_init();

	while(1){
		server_main();
		memset(buf,0,sizeof(buf));
		recv(dstSocket,buf,sizeof(buf),0);// 소켓으로부터 자료 수신 함수
		printf("Ricived from client %s\n",buf);
		//sever에서 받은 명령어 비교 후 모터 실행
		stop=strcmp(buf, "stop");
		if(stop==0){
                        digitalWrite(MOTOR1,0);
                        digitalWrite(MOTOR2,0);
                        digitalWrite(MOTOR3,0);
                        digitalWrite(MOTOR4,0);
		}
                up=strcmp(buf, "up");
                if(up==0){
                        digitalWrite(MOTOR1,0);
                        digitalWrite(MOTOR2,1);
                        digitalWrite(MOTOR3,0);
                        digitalWrite(MOTOR4,1);

                }
                down=strcmp(buf, "down");
                if(down==0){
                        digitalWrite(MOTOR1,1);
                        digitalWrite(MOTOR2,0);
                        digitalWrite(MOTOR3,1);
                        digitalWrite(MOTOR4,0);
                }
                left=strcmp(buf, "left");
                if(left==0){
                        digitalWrite(MOTOR1,0);
                        digitalWrite(MOTOR2,1);
                        digitalWrite(MOTOR3,0);
                        digitalWrite(MOTOR4,0);
                }
                right=strcmp(buf, "right");
                if(right==0){
                        digitalWrite(MOTOR1,0);
                        digitalWrite(MOTOR2,0);
                        digitalWrite(MOTOR3,0);
                        digitalWrite(MOTOR4,1);
                }
		close(dstSocket);
	}
close(srcSocket);

return EXIT_SUCCESS;
}
