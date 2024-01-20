#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include "WiFiConexion.h"
#include "config.h"

const char* topicLed = "guillermovalverdebaez/feeds/led";
const char* topicMsg = "guillermovalverdebaez/feeds/msg";

WiFiClient espClient;
PubSubClient client(espClient);

unsigned long lastMillis = 0;

void callback(char* topic, byte* payload, unsigned int length);
void reconnect( void );

void setup( void ) {
  Serial.begin(9600);
 
  conectToWiFi(ssid, password);

  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);


  pinMode(2, OUTPUT);

  lastMillis = millis();
}

void loop( void ) {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  if (millis() - lastMillis > 5000) {
    client.publish(topicMsg, "Hola desde ESP32");
    lastMillis = millis();
  }
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Mensaje nuevo en el topico: ");
  Serial.print(topic);
  Serial.print(". Mensaje: ");
  String messageTemp;
  
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
    messageTemp += (char)payload[i];
  }
  Serial.println();

  if (String(topic) == topicLed) {
    Serial.print("Changing output to ");
    if(messageTemp == "on"){
      Serial.println("on");
      digitalWrite(2, HIGH);
    }
    else if(messageTemp == "off"){
      Serial.println("off");
      digitalWrite(2, LOW);
    }
  }
}

void reconnect( void ) {
  while (!client.connected()) {
    Serial.println("Intentando conexión a servidor MQTT...");

    if (client.connect("ESP32-Guillermo", username, pass)) {
      Serial.println("Conectado");
      client.subscribe(topicLed);
    } else {
      Serial.print("Falló, rc=");
      Serial.print(client.state());
      Serial.println(" intentando en 5 seg...");
      delay(5000);
    }
  }
}