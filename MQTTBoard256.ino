#include <ESP8266WiFi.h>
#include <ESP8266WiFiAP.h>
#include <ESP8266WiFiGeneric.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266WiFiScan.h>
#include <ESP8266WiFiSTA.h>
#include <ESP8266WiFiType.h>
#include <WiFiClient.h>
#include <WiFiClientSecure.h>
#include <WiFiServer.h>
#include <WiFiServerSecure.h>
#include <WiFiUdp.h>

# include <Adafruit_NeoPixel.h>
#include <SPI.h>
#include <MQTT.h>


#ifndef MQTT_MAX_PACKET_SIZE
#define MQTT_MAX_PACKET_SIZE 512
#endif

#define LED_PIN D2
#define LED_COUNT 256



int brightness;


Adafruit_NeoPixel board = Adafruit_NeoPixel(LED_COUNT, LED_PIN, NEO_GRB);
const char ssid[] = "Linksys_2.4G";
const char pass[] = "19902522";



WiFiClient net;
MQTTClient client;

unsigned long lastMillis = 0;

void connect() {
  Serial.print("checking wifi...");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(1000);
  }

  Serial.print("\nconnecting...");
  while (!client.connect("arduino", "try", "try")) {
    Serial.print(".");
    delay(1000);
  }

  Serial.println("\nconnected!");

  client.subscribe("ESP8266");
  // client.unsubscribe("/hello");
}
char n3,n2,n1,n0,n,r,b,g;  
void messageReceived(String &topic, String &payload) {  
 //Serial.println("incoming: " + topic + " - " + payload);
 Serial.print("=");
 char* plptr = &payload[0]; 
  int cmd = plptr[0];
  switch(cmd){
    case 'b':      
      brightness = payload[1];
      //Serial.println("incoming: " + topic + " - Set Brightness:" + brightness);  
      break;
    case 's':            
      n3 = plptr[1];
      n2 = plptr[2];
      n1 = plptr[3];
      n0 = plptr[4];
      r = plptr[5];
      g = plptr[6];
      b = plptr[7];
      n = (n3&0x0f)*1000+(n2&0x0f)*100+(n1&0x0f)*10+(n0&0x0f);
      //Serial.println("incoming: " + topic + " - Set LED count:" + (int)(n));     
               
      board.setPixelColor((n&0xff),r&0xff,g&0xff,b&0xff);
      break;
    case 'd':
      Serial.print("\n_"); 
      board.setBrightness(brightness);
      board.show();        
      digitalWrite(LED_BUILTIN, HIGH);
      break;
    }  
    
  
}

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);
  Serial.begin(115200);
  WiFi.begin(ssid, pass);
  
  // Note: Local domain names (e.g. "Computer.local" on OSX) are not supported by Arduino.
  // You need to set the IP address directly.
  client.begin("192.168.1.109",1883, net);
  client.onMessage(messageReceived);
  connect();
  board.begin();
}

void loop() {
  client.loop();

  if (!client.connected()) {
    connect();
  }

}
