#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

#include <wiringPi.h>
#include <wiringSerial.h>
#include <mysql/mysql.h>

char device[] = "/dev/ttyUSB0";
int fd;
unsigned long baud = 9600;

static char *host = "localhost";
static char *user = "root";
static char *pass = "raspberry";
static char *dbname = "test";

int main (){
    MYSQL *conn;
    MYSQL_RES *res_ptr;
    MYSQL_ROW sqlrow;
    conn = mysql_init(NULL);
    char in_sql[255];

    if(!(mysql_real_connect(conn, host, user, pass, dbname, 0, NULL, 0)))
    {
        fprintf(stderr, "ERROR: %s[%d]\n",
        mysql_error(conn),mysql_errno(conn));
        exit(1);
    }

    char ser_buff[10] = {0};
    int index = 0 , cds, sound, str_len;
    char *pArry[2] = {0};
    char *pToken;
    printf("Raspberry Startup\n");
    fflush(stdout);
    
    //try connecting db
    if((fd = serialOpen(device, baud)) < 0){
        fprintf(stderr, "Unable %s\n", strerror(errno));
        exit(1);
    }
    else{
        printf("Connection Successful!\n\n");
    }

    if(wiringPiSetup() == -1){
        return 1;
    }
        while(1)
        {
        if(serialDataAvail(fd))
        {
            printf("serial_done!! \n");
            ser_buff[index++] = serialGetchar(fd);
            printf("ser_buff = %s\n", ser_buff);

            //---------------------------------------------
            if(ser_buff[index-1] == 'L'){
                ser_buff[index-1] = '\0';
                str_len = strlen(ser_buff);
                printf("ser_buff = %s\n", ser_buff);
                pToken = strtok(ser_buff, ":");
                int i = 0 ;
                while (pToken != NULL)
                {
                    pArry[i] = pToken;
                    if(++i > 3){
                        break;
                    }
                    pToken = strtok(NULL," ");
                }
                
                cds = atoi(pArry[0]);
                sound = atoi(pArry[1]);
                sprintf(in_sql,"insert into sensing(ID,DATE,TIME,CDS,SOUND) values( null, curdate(), curtime(), %d, %d)",cds,sound);
                int res =  mysql_query(conn, in_sql);
                printf("cds = %d, sound = %d \n",cds,sound);
                printf("res = %d\n",res);
                
                if(!res){
                    printf("inserted %lu rows\n",(unsigned long)mysql_affected_rows(conn));
                }else{
                    fprintf(stderr, "insert error %d: %s\n", mysql_errno(conn), mysql_error(conn));
                }

                for(int i = 0 ; i < str_len; i++){
                    ser_buff[i] = 0;
                }
                index = 0;
            }
        }
        fflush(stdout);
        
    }
    mysql_free_result(res_ptr);
    mysql_close(conn);
    return EXIT_SUCCESS;
}