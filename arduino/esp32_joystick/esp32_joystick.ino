#include <WiFi.h> //라이브러리를 가져와 코드를 시작하므로 ESP32를 무선 네트워크에 연결
#include <WiFiAP.h>
#include <WiFiClient.h>
#include <WiFiMulti.h>
#include <WiFiScan.h>
#include <WiFiServer.h>

WiFiMulti WiFiMulti;
WiFiClient client;  //클라이언트 대리자를 선언
void setup() {
 
  Serial.begin(115200);
  delay(10);

  WiFiMulti.addAP("iptime_C6H12O6","123456789"); //각자의 통신환경에 따라 설정해 준다. . 네트워크 이름 및 암호 저장.

  Serial.println();
  Serial.println();
  Serial.print("Waiting for WiFi...");

  while(WiFiMulti.run()!=WL_CONNECTED){
    Serial.print(".");
    delay(500);
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  delay(500);
}

void connections(){
  const uint16_t port=12; //포트번호 설정
  const char *host="192.168.0.12"; //IP설정

  Serial.print("Connecting to");
  Serial.println(host);

  if(!client.connect(host,port)) {
    Serial.println("Connection failed.");
    Serial.println("Waiting 5 seconds before retrying...");
    delay(5000);
    return;
  }
}
 
void loop() {
  connections();

  int X = analogRead(32);
  int Y = analogRead(34);
  Serial.print("X: ");
  Serial.print(X);
  Serial.print(" ");
  Serial.print("Y: ");
  Serial.print(Y);

  if(Y<100){
    client.println("down");
  }else if(Y>4000){
    client.println("up");
  }else if(X<100){
    client.println("left");
  }else if(X>4000){
    client.println("right");
  }else if(X<1900 && X>1700){
    client.println("stop");
  }
  
  delay(200);
}
