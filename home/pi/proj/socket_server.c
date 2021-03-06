#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>		//프로토콜 및 호스트 이름을 숫자 주소로 변환하는 기능을 정의. 로컬 데이터와 DNS를 검색
#include <sys/socket.h>		// 소켓 사용 함수
#include <netinet/in.h>		//AF_INET 및 AF_INET6 주소 패밀리. 인터넷에서 널리 사용되는 IP 주소와 TCP / UDP 포트 번호가 포함
#include <arpa/inet.h>		//숫자로 IP 주소를 조작하는 기능의 정의

#include <wiringPi.h>

#define BUFSIZE 8192

#define TCP_PORT 12

#define MOTOR1 0
#define MOTOR2 3
#define MOTOR3 4
#define MOTOR4 5

int srcSocket;
int dstSocket;

				// sockaddr_in 구조체
struct sockaddr_in s_addr;
struct sockaddr_in c_addr;
socklen_t len;			//소켓길이 및 크기 값

				//char textdata[11]="01234567890";
char buf[BUFSIZE];

void server_init(){
	//--소켓 생성--
	if((srcSocket = socket(AF_INET,SOCK_STREAM,0))<0){
		//socket:소켓 생성함수
		//SOCK_STREAM : 연결 지향형 소켓
		//AF_INET: IPv4인터넷 프로토콜

		//통신 오류
		perror("socket error");
		exit(EXIT_FAILURE);
	}

	int on = 1;
	printf("Server Started\n");
	setsockopt(srcSocket,SOL_SOCKET,SO_REUSEADDR,&on,sizeof(on));
	//setsockopt:소켓에 속성값을 변경하는 함수
	//SO_REUSEADDR: 이미 사용된 주소를 재사용하도록한다.

	//--sockaddr_in 구조체 설정--
	memset((char*)&s_addr,0,sizeof(s_addr));//메모리 초기화 함수
	s_addr.sin_family = AF_INET;//IPv4인터넷 프로토콜
	s_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	//htonl : 호스트 long intger(일반적으로 4byte)데이터를 네트워크 byte order로 변경
	//INADDR_ANY: 자동으로 이 컴퓨터에 존재하는 랜카드 중 사용가능한 랜카드의 IP주소를 사용하라는 의미
	s_addr.sin_port = htons(TCP_PORT);
	//htons : 호스트short intger(일반적으로 2byte)데이터를 네트워크 byte order로 변경

	//--소켓 바인드 설정--
	if(bind(srcSocket,(struct sockaddr*)&s_addr,sizeof(s_addr))<0){
		//bind :함수 와 객체를 묶는 함수
		perror("bind");
		exit(EXIT_FAILURE);
	}
	//--접속 허가--
	listen(srcSocket,5); //연결 요청 소켓이 대기하는 연결 대기열 생성 함수
	//클라이언트로부터 접속 요구를 수신
	len=sizeof(dstSocket);

}

void server_main(){
	//--소켓 접속--
	if((dstSocket=accept(srcSocket,(struct sockaddr*)&c_addr,&len))<0){
		//accept:연결대기 중 클라이언트의 연결 요청을 수락하는 기능 함수
		//소켓 접속 실패시
		perror("accept error");
		printf("loading...\n");
		exit(EXIT_FAILURE);
	}
	//printf("connected from '%s'\n",inet_ntoa(s_addr.sin_addr));
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
	//--서버 초기화--
	server_init();

	while(1){
		//--소켓 접속 설정--
		server_main();
		//메모리 초기화
		memset(buf,0,sizeof(buf));
		//--패킷 수신--
		recv(dstSocket,buf,sizeof(buf),0);// 소켓으로부터 자료 수신 함수
		printf("Ricived from client %s\n",buf);
		//sever에서 받은 명령어 검토 후 모터 실행
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
