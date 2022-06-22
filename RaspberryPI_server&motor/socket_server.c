#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include <sys/socket.h>
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

struct sockaddr_in s_addr;
struct sockaddr_in c_addr;
socklen_t len;
int n;

char textdata[11]="01234567890";
char buf[BUFSIZE];

void server_init(){
	if((srcSocket = socket(AF_INET,SOCK_STREAM,0))<0){
		perror("socket error");
		exit(EXIT_FAILURE);
	}

	int on = 1;
	printf("Server Started\n");
	setsockopt(srcSocket,SOL_SOCKET,SO_REUSEADDR,&on,sizeof(on));

	memset((char*)&s_addr,0,sizeof(s_addr));
	s_addr.sin_family = AF_INET;
	s_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	s_addr.sin_port = htons(TCP_PORT);

	if(bind(srcSocket,(struct sockaddr*)&s_addr,sizeof(s_addr))<0){
		perror("bind");
		exit(EXIT_FAILURE);
	}

	listen(srcSocket,5);

	len=sizeof(dstSocket);

}

void server_main(){
	if((dstSocket=accept(srcSocket,(struct sockaddr*)&c_addr,&len))<0){
		perror("accept error");
		exit(EXIT_FAILURE);
	}
	printf("connected from '%s'\n",inet_ntoa(s_addr.sin_addr));
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
		recv(dstSocket,buf,sizeof(buf),0);
		printf("Ricived from client %s\n",buf);
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
