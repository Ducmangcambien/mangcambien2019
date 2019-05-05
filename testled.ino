#include <NTPClient.h>
#include <Arduino_JSON.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <WiFiUdp.h>
#include <string.h>
#include "DHT.h"
#include "SharpGP2Y10.h"

#define mqttport  1883
#define mqttuser  "ducmqtt"
#define mqttpass  "duc"
#define mqtttopic "esptoppic"

#define ASPIN D2
#define LEDPIN D7
#define speaker 0

int   GP2YPIN     = A0;
int   LEDGP2Y     = D4;
const int DHTPIN = 5;
const int DHTTYPE = DHT11;
float dustDensity = 0;

const char* ssid     = "Tang 3";
const char* password = "camonquykhach";
const char* mqttip   = "192.168.3.104";

DHT dht(DHTPIN, DHTTYPE);
WiFiClient duc;
PubSubClient client(duc);
WiFiUDP udp;
NTPClient ntpc(udp,"asia.pool.ntp.org",7*3600);
SharpGP2Y10 dustSensor(GP2YPIN, LEDGP2Y);

void callback(char* toppic,byte* message,unsigned int length){
  Serial.print("Topic: ");
  Serial.print(toppic);
  Serial.print(". Message: ");
  String messageTemp;
  for (int i = 0; i < length; i++) {
    Serial.print((char)message[i]);
    messageTemp += (char)message[i];
  }
  Serial.println();
  if (String(toppic) == "Toppic 1") {
    Serial.print("setup led1 ");
    if (messageTemp.toInt() == 1){
      digitalWrite(LEDPIN, 1);
      Serial.println("led-on");
    }
    else if(messageTemp.toInt() == 0){
      digitalWrite(LEDPIN, 0);
      Serial.println("led-off");
    }else{
      analogWrite(LEDPIN,messageTemp.toInt());
    }
    Serial.println(messageTemp);
  }
}
/*---------------------------------setup wifi--------------------------------*/
void setupwifi(){
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(100);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}
/*--------------------------------setup mqtt ---------------------------------*/
void setupmqtt(){
  while (!client.connected()){
    Serial.println("Dang ket noi vao QMTT!");
    if (client.connect("ESPclient",mqttuser,mqttpass)){
      Serial.println("Da ket noi thanh cong vao MQTT!");
      client.publish(mqtttopic,"ESP da ket noi");
      client.subscribe(mqtttopic);
      client.subscribe("Toppic 1");
    }else{
      Serial.println("Ket noi that bai!");
      delay(5000);
      Serial.println("Dang thu lai!");
    }
  }
}
/*------------------------------set up cam bien---------------------------------*/

float Read_Dust(void){
  float dust = dustSensor.getDustDensity()*212;
  return dust;
}

/*-----------------------------setup publish--------------------------------------*/
void pubdata(){
  String temp = String(int(dht.readTemperature()));
  String hum = String(int(dht.readHumidity()));
  String dust = String(Read_Dust());
  String payload = "{";
  payload += "\"temp\": "; payload += temp; payload += ",";
  payload += "\"hum\": "; payload += hum; payload += ",";
  payload += "\"dust\": "; payload += dust;
  payload += "}";
  char datapub[100];
  payload.toCharArray(datapub, 100);
  client.publish(mqtttopic, datapub);
  delay(1000);
}
/*------------------------------------setup tong------------------------------------*/
void setup(){
  Serial.begin(115200);
  dht.begin();
  ntpc.begin();
  setupwifi();
  client.setServer(mqttip,mqttport);
  client.setCallback(callback);
  pinMode(LEDPIN, OUTPUT);
}
void loop(){
  if(!client.connected()){
    setupmqtt();
  }
  if(client.loop())
  client.connect("ESPclient");
  pubdata();
}
