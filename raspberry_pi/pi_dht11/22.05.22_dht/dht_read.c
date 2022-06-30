#define _CRT_SECURE_NO_WARNINGS    // strcat 보안 경고로 인한 컴파일 에러 방지

//dht header 선언
#include <wiringPi.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

//mariadb header 선언       
#include <mysql/mysql.h>

#define MAXTIMINGS 83
#define DHTPIN 6

int dht11_dat[5] = {0, } ;
int erro = 0;

static char *host = "localhost";
static char *user = "root";
static char *pass = "raspberry";
static char *dbname = "project";

void read_dht11_dat()
{
  uint8_t laststate = HIGH ;
  uint8_t counter = 0 ;
  uint8_t j = 0, i ;
  uint8_t flag = HIGH ;
  uint8_t state = 0 ;
  float f ;

  dht11_dat[0] = dht11_dat[1] = dht11_dat[2] = dht11_dat[3] = dht11_dat[4] = 0 ;

  pinMode(DHTPIN, OUTPUT) ;//출력으로 선언
  digitalWrite(DHTPIN, LOW) ;// low로 출력
  delay(18);//18ms 

  digitalWrite(DHTPIN, HIGH) ;
  delayMicroseconds(30) ;//30us == 0.03ms
  pinMode(DHTPIN, INPUT) ;

  for (i = 0; i < MAXTIMINGS; i++) {
    counter = 0 ;
    while ( digitalRead(DHTPIN) == laststate) {
      counter++ ;
      delayMicroseconds(1) ;//1us
      if (counter == 200) break ; //200us
    }

    laststate = digitalRead(DHTPIN) ;
    if (counter == 200) break ; // if while breaked by timer, break for
    if ((i >= 4) && (i % 2 == 0)) {
      dht11_dat[j / 8] <<= 1 ;//bit 연산자 0 추가
      if (counter > 20) dht11_dat[j / 8] |= 1 ;//bit 연산자 1 추가
      j++ ;
    }

  }

  if ((j >= 40) && (dht11_dat[4] == ((dht11_dat[0] + dht11_dat[1] + dht11_dat[2] + dht11_dat[3]) & 0xff))) {
    erro = 0;
    printf("humidity = %d.%d %% Temperature = %d.%d *C \n", dht11_dat[0], dht11_dat[1], dht11_dat[2], dht11_dat[3]) ;
  }
  else{
      erro = 1;
      printf("Data get failed\n") ;
  } 
}



int main(void)
{
    //connect maria DB
    MYSQL *conn;
    MYSQL_RES *res_ptr;
    MYSQL_ROW sqlrow;
    conn = mysql_init(NULL);

    //error handler
    if(!(mysql_real_connect(conn, host, user, pass, dbname, 0, NULL, 0)))
    {
        fprintf(stderr, "ERROR: %s[%d]\n",
        mysql_error(conn),mysql_errno(conn));
        exit(1);
    }
    printf("Connection Successful!\n\n");

  printf("dht11 Raspberry pi\n") ;
  if (wiringPiSetup() == -1) exit(1) ;

  while (1) {
    read_dht11_dat() ;
    delay(1000) ;//1000ms -> 1s
    char temp[100],tmep_1[20];
    sprintf(temp,"insert into tank(ID,DATE,TEMP,HUM)values(null,null,");
    sprintf(tmep_1,"%d",dht11_dat[2]);
    strcat(temp,tmep_1);
    strcat(temp,",");
    sprintf(tmep_1,"%d",dht11_dat[0]);
    strcat(temp,tmep_1);
    strcat(temp,")");
    printf("temp : %s\n\n",temp);

    //INSERT INTO `tank`(`ID`, `DATE`, `TEMP`, `HUM`) VALUES (NULL,NULL,24,47)
    if(erro == 0){
        int res =  mysql_query(conn, temp);
        if(!res){
            printf("inserted %lu rows\n",(unsigned long)mysql_affected_rows(conn));
        }else{
            fprintf(stderr, "insert error %d: %s\n", mysql_errno(conn), mysql_error(conn));
            }
    }    
  }

  mysql_free_result(res_ptr);
  mysql_close(conn);
  return 0 ;
}
