#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

#include <wiringPi.h>
#include <wiringSerial.h>

char device[] = "/dev/ttyUSB0";
int fd;
unsigned long baud = 9600;
int main (){
    char ser_buff[10] = {0};
    int index = 0 , cds, sound, str_len;
    char *pArry[2] = {0};
    char *pToken;
    printf("Raspberry Startup");
    fflush(stdout);
    if((fd = serialOpen(device, baud)) < 0){
        fprintf(stderr, "Unable %s\n", strerror(errno));
        exit(1);
    }
    if(wiringPiSetup() == -1){
        return 1;
    }
        while(1)
        {
        if(serialDataAvail(fd))
        {
            ser_buff[index++] = serialGetchar(fd);
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
                    /* code */
                }
                cds = atoi(pArry[0]);
                sound = atoi(pArry[1]);
                printf("cds = %d, sound = %d \n",cds,sound);
                for(int i = 0 ; i < str_len; i++){
                    ser_buff[i] = 0;
                }
                index = 0;
            }
        }
        fflush(stdout);
    }
    return 0;
}