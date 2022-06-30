/*
 WiFiEsp example: WebClient

 This sketch connects to google website using an ESP8266 module to
 perform a simple web search.

 For more details see: http://yaab-arduino.blogspot.com/p/wifiesp-example-client.html
*/

#include "WiFiEsp.h"

// Emulate Serial1 on pins 6/7 if not present
#ifndef HAVE_HWSERIAL1
#include "SoftwareSerial.h"
SoftwareSerial Serial1(10, 9); // RX, TX
#endif

char ssid[] = "iot6";            // your network SSID (name)
char pass[] = "iot60000";        // your network password
int status = WL_IDLE_STATUS;     // the Wifi radio's status

char server[] = "10.10.141.182";

// Initialize the Ethernet client object
WiFiEspClient client;

void setup()
{
  // initialize serial for debugging
  Serial.begin(9600);
  // initialize serial for ESP module
  Serial1.begin(9600);
  // initialize ESP module
  WiFi.init(&Serial1);

  // check for the presence of the shield
  if (WiFi.status() == WL_NO_SHIELD) {
    Serial.println("WiFi shield not present");
    // don't continue
    while (true);
  }

  // attempt to connect to WiFi network
  while ( status != WL_CONNECTED) {
    Serial.print("Attempting to connect to WPA SSID: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network
    status = WiFi.begin(ssid, pass);
  }

  // you're connected now, so print out the data
  Serial.println("You're connected to the network");
  
  printWifiStatus();

  Serial.println();
  Serial.println("Starting connection to server...");
  // if you get a connection, report back via serial
  if (client.connect(server, 5000)) {
    Serial.println("Connected to server");
  }
}
void loop()
{
  // if there are incoming bytes available
  // from the server, read them and print them
  char sensing[10] = {0};
  int index = 0;
  //client.print("[2] hello\n");
  
//  while (client.available()) {
//   sensing[index++] = client.read();    
//  }
  int X = analogRead(A0);
  int Y = analogRead(A1);
  Serial.print("X: ");
  Serial.print(X);
  Serial.print(" ");
  Serial.print("Y: ");
  Serial.print(Y);
  Serial.print("\n");

  if(Y<400){
    client.println("down");
  }else if(Y>600){
    client.println("up");
  }else if(X<400){
    client.println("left");
  }else if(X>600){
    client.println("right");
  }else{
    client.println("stop");
  }
  sensing[index] = '\0';
  Serial.println(sensing);
  delay(100);
  // if the server's disconnected, stop the client
  if (!client.connected()) {
    Serial.println();
    Serial.println("Disconnecting from server...");
    client.stop();

    // do nothing forevermore
    while (true);
  }
}


void printWifiStatus()
{
  // print the SSID of the network you're attached to
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your WiFi shield's IP address
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print the received signal strength
  long rssi = WiFi.RSSI();
  Serial.print("Signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
}
