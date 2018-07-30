#include <ESP8266WiFi.h>
#include <Adafruit_NeoPixel.h>

#include <PubSubClient.h>



#ifndef MQTT_MAX_PACKET_SIZE
#define MQTT_MAX_PACKET_SIZE 512
#endif

#define LED_PIN D2
#define LED_COUNT 256

Adafruit_NeoPixel board = Adafruit_NeoPixel(LED_COUNT, LED_PIN, NEO_GRB);
const char ssid[] = "Linksys_2.4G";
const char pass[] = "19902522";




WiFiClient net;
PubSubClient client;

unsigned long lastMillis = 0;

void connect() {
  Serial.print("checking wifi>>>");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(">");
    delay(200);
  }

 Serial.print("\nconnecting...");
  while (!client.connect("wemos1")) {
    Serial.print(".");
    delay(200);
  }
  client.subscribe("ESP8266");

  
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}
char r,b,g,brightness,led;  

void callback(char* topic, byte* payload, unsigned int length) 
{
//	Serial.print("Data Received [");
//	Serial.print(topic);
//	Serial.print("] ");
//	for (int i = 0; i < length; i++) {
//		Serial.print((char)payload[i]);
//	}
//  Serial.println();  
	char cmd = (char)payload[0];
	
	switch(cmd){
		case 's':
			led = 0;
			for (int i = 1; i < length; i+=3) {
				r=payload[i];
				g=payload[i+1];
				b=payload[i+2];
				board.setPixelColor(led,r,g,b);
				//Serial.print(led +", ");
				led ++;				
			}
			//Serial.print("\n");
			board.show();
		break;
		case 'b':
			brightness = payload[1];
			board.setBrightness(brightness);
			board.show();
		break;
		case 'c':
			board.clear();
			board.show();
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
  client = PubSubClient("192.168.1.109",1883,net);
  client.setCallback (callback);
  
  connect();
  board.begin();
}

void loop() {
  client.loop();

  if (!client.connected()) {
    connect();
  }

}
