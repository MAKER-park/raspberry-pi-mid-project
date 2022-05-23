#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <wiringPi.h>
#include <stdint.h>

#define BUFSIZE 8192
#define MAXTIMINGS 83
//소켓 port
#define TCP_PORT 12
//모터 pin.no
#define MOTOR1 0
#define MOTOR2 3
#define MOTOR3 4
#define MOTOR4 5
//온슨도 센서 pin .no
#define DHTPIN 6

int srcSocket;
int dstSocket;

//온습토전역 변수
int dht11_dat[5]={0, };

struct sockaddr_in s_addr;
struct sockaddr_in c_addr;
socklen_t len;
int n;

char textdata[11]="01234567890";
char buf[BUFSIZE];
// 온습도 일기
void read_dht11_dat()

{

  uint8_t laststate = HIGH ;
  uint8_t counter = 0 ;
  uint8_t j = 0, i ;
  uint8_t flag = HIGH ;
  uint8_t state = 0 ;
  float f ;

  dht11_dat[0] = dht11_dat[1] = dht11_dat[2] = dht11_dat[3] = dht11_dat[4] = 0 ;
  pinMode(DHTPIN, OUTPUT) ;
  digitalWrite(DHTPIN, LOW) ;
  delay(18) ;

  digitalWrite(DHTPIN, HIGH) ;
  delayMicroseconds(30) ;
  pinMode(DHTPIN, INPUT) ;

  for (i = 0; i < MAXTIMINGS; i++) {
    counter = 0 ;
    while ( digitalRead(DHTPIN) == laststate) { 
      counter++ ;
      delayMicroseconds(1) ;
      if (counter == 200) break ;
    }

    laststate = digitalRead(DHTPIN) ;

    if (counter == 200) break ; // if while breaked by timer, break for
    if ((i >= 4) && (i % 2 == 0)) {
      dht11_dat[j / 8] <<= 1 ;
      if (counter > 20) dht11_dat[j / 8] |= 1 ;
      j++ ;
   }
  }

  if ((j >= 40) && (dht11_dat[4] == ((dht11_dat[0] + dht11_dat[1] + dht11_dat[2] + dht11_dat[3]) & 0xff))) {
    printf("humidity = %d.%d %% Temperature = %d.%d *C \n", dht11_dat[0], dht11_dat[1], dht11_dat[2], dht11_dat[3]) ;
  }
  else printf("Data get failed\n") ;
}

// client_server 실행
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

		//온습도 실행
		read_dht11_dat() ;
		close(dstSocket);
	}
close(srcSocket);

return EXIT_SUCCESS;
}
