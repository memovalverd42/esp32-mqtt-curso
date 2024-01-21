#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include "WiFiConexion.h"

#define LED_PIN 2

const char* ssid = "";
const char* password = "";

// Datos broker MQTT
const char* mqttServer = "192.200.201.65";
const char* mqttUsername = "";
const char* mqttPassword = "";
const int mqttPort = 1883;

const char* topicLed = "guillermovalverdebaez/feeds/led";
const char* topicMsg = "guillermovalverdebaez/feeds/msg";

String message;

WiFiClient espClient;
PubSubClient client(espClient);

unsigned long lastMillis = 0;

void reconnect( void );
void callback(char* topic, byte* payload, unsigned int length);

void setup( void ) {
  Serial.begin(9600);
  pinMode(LED_PIN, OUTPUT);
 
  conectToWiFi(ssid, password);

  client.setServer(mqttServer, mqttPort);
  client.setCallback(callback);

  lastMillis = millis();
}

void loop( void ) {

  if(!client.connected()){
    reconnect();
  }
  client.loop();

  if(Serial.available()){
    message = Serial.readString();
    char messageChar[message.length() + 1];

    message.toCharArray(messageChar, message.length() + 1);
    client.publish(topicMsg, messageChar);
  }

}

void callback(char* topic, byte* payload, unsigned int length) {
  String payloadMsg;

  for(int i=0; i<length; i++){
    payloadMsg += (char)payload[i];
  }
  Serial.println(payloadMsg);

  if(String(topic) == topicLed){
    if(payloadMsg == "on")
      digitalWrite(LED_PIN, HIGH);
    else
      digitalWrite(LED_PIN, LOW);
  }

}

void reconnect( void ) {

  while (!client.connected()) {
    Serial.println("Intentando conexión a servidor MQTT...");

    if (client.connect("esp32Guillermo", mqttUsername, mqttPassword)) {
      Serial.println("Conectado");
      client.subscribe(topicLed);
      client.subscribe("esp32/ibox");
    } else {
      Serial.print("Falló, rc=");
      Serial.print(client.state());
      Serial.println(" intentando en 5 seg...");
      delay(5000);
    }
  }

}


