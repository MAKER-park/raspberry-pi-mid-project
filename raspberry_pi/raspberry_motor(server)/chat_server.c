#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <pthread.h>
#include <wiringPi.h>
#define BUF_SIZE 100
#define MAX_CLNT 256
#define MAXTIMINGS 83
#define MOTOR1 0
#define MOTOR2 3
#define MOTOR3 4
#define MOTOR4 5

//온슨도 센서 pin .no
#define DHTPIN 6
void* handle_clnt(void* arg);
void send_msg(char* msg, int len);
void error_handling(char*msg);

int clnt_cnt=0;
int clnt_socks[MAX_CLNT];
pthread_mutex_t mutx;

//온습토전역 변수
int dht11_dat[5]={0, };

struct sockaddr_in s_addr;
struct sockaddr_in c_addr;
socklen_t len;
int n;

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

int main(int argc, char* argv[]){
	int serv_sock, clnt_sock;
	struct sockaddr_in serv_adr, clnt_adr;
	int clnt_adr_sz;
	pthread_t t_id;
	
	if(wiringPiSetup()==-1) return 1;	
	pinMode(MOTOR1,OUTPUT);
	pinMode(MOTOR2,OUTPUT);
	pinMode(MOTOR3,OUTPUT);
	pinMode(MOTOR4,OUTPUT);
	if(argc!=2){
		printf("Usage : %s <port>\n",argv[0]);
		exit(1);
	}
	
	pthread_mutex_init(&mutx, NULL);
	serv_sock = socket(PF_INET, SOCK_STREAM,0);

	memset(&serv_adr, 0, sizeof(serv_adr));
	serv_adr.sin_family = AF_INET;
	serv_adr.sin_addr.s_addr=htonl(INADDR_ANY);
	serv_adr.sin_port = htons(atoi(argv[1]));

	if(bind(serv_sock, (struct sockaddr*)&serv_adr, sizeof(serv_adr))==-1)
		error_handling("bind() error");
	if(listen(serv_sock,5)==-1)
		error_handling("listen() error");

	while(1){
		clnt_adr_sz=sizeof(clnt_adr);
		clnt_sock = accept(serv_sock, (struct sockaddr*)&clnt_adr,&clnt_adr_sz);

		pthread_mutex_lock(&mutx);
		clnt_socks[clnt_cnt++]=clnt_sock;
		pthread_mutex_unlock(&mutx);

		pthread_create(&t_id, NULL, handle_clnt, (void*)&clnt_sock);
		pthread_detach(t_id);
		printf("Connected client IP : %s \n",inet_ntoa(clnt_adr.sin_addr));
	}
	close(serv_sock);
	return 0;
}

void* handle_clnt(void* arg){
	int clnt_sock=*((int*)arg);
	int str_len=0, i;
	char msg[BUF_SIZE];
	while((str_len=read(clnt_sock, msg, sizeof(msg)))>=2){
	//	send_msg(msg, str_len);
		printf("msg :%s\n",msg);
		
		if(strcmp(msg,"stop")==0){
			digitalWrite(MOTOR1,0);
			digitalWrite(MOTOR2,0);
			digitalWrite(MOTOR3,0);
			digitalWrite(MOTOR4,0);
		}else if(strcmp(msg,"up")==0){
			digitalWrite(MOTOR1,0);
			digitalWrite(MOTOR2,1);
			digitalWrite(MOTOR3,0);
			digitalWrite(MOTOR4,1);
		}else if(strcmp(msg,"down")==0){
			digitalWrite(MOTOR1,1);
			digitalWrite(MOTOR2,0);
			digitalWrite(MOTOR3,1);
			digitalWrite(MOTOR4,0);
		}else if(strcmp(msg,"left")==0){
			digitalWrite(MOTOR1,0);
			digitalWrite(MOTOR2,1);
			digitalWrite(MOTOR3,0);
			digitalWrite(MOTOR4,0);
		}else if(strcmp(msg,"right")==0){
			digitalWrite(MOTOR1,0);
			digitalWrite(MOTOR2,0);
			digitalWrite(MOTOR3,0);
			digitalWrite(MOTOR4,1);
		}
		memset(msg, NULL ,100);
		read_dht11_dat();
		//send_msg(msg, str_len);
		
	}
	pthread_mutex_lock(&mutx);
	for(i=0; i<clnt_cnt; i++){
		if(clnt_sock == clnt_socks[i]){
			while(i++<clnt_cnt-1)
				clnt_socks[i]=clnt_socks[i+1];		
			break;
		}
	}
	clnt_cnt--;
	pthread_mutex_unlock(&mutx);
	close(clnt_sock);
	return NULL;
}
void send_msg(char* msg, int len){
	int i;
	pthread_mutex_lock(&mutx);
	for(i=0;i<clnt_cnt;i++){
		write(clnt_socks[i], msg, len);
	}
	pthread_mutex_unlock(&mutx);

}
void error_handling(char* msg){
	fputs(msg,stderr);
	fputc('\n',stderr);
	exit(1);
}
