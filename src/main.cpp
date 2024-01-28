#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <Arduino_JSON.h>
#include <PubSubClient.h>
#include "WiFiConexion.h"

#define LED_PIN 2

const char* ssid = "SALA 201";
const char* password = "CG.PAGES";

// Datos broker MQTT
const char* mqttServer = "192.200.201.66";
const char* mqttUsername = "memo";
const char* mqttPassword = "password123";
const int mqttPort = 1883;

const char* topicLed = "esp32/led";
const char* topicMsg = "esp32/msg";

const String baseURL = "https://api.openweathermap.org/data/2.5/weather?lat=19.1924201&lon=-96.1421051&units=metric&lang=es&appid=2bf5873e176fab2e510754fae8ffc595";
unsigned long lastMillis = 0;

float tempInMinValue = 0;
float tempInMaxValue = 0;
float humInMinValue = 0;
float humInMaxValue = 0;

WiFiClient espClient;
PubSubClient client(espClient);

void reconnect( void );
void callback(char* topic, byte* payload, unsigned int length);
JSONVar httpGetRequest( String url );
void setAlarm(String idAlarm, float min, float max);
void updateAlarmState( float tempIn, float humIn );

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

  if(millis() - lastMillis >= 5000) {
    JSONVar owRespone = httpGetRequest(baseURL);
    JSONVar data;
    String msg;

    float tempIn = dht.readTemperature(); 
    float humIn = dht.readHumidity();

    data["Estao interior"]["Temp"] = tempIn;
    data["Estao interior"]["Hume"] = humIn;
    data["Estado exterior"]["Temp"] = JSON.stringify(owRespone["main"]["temp"]);
    data["Estado exterior"]["Hume"] = JSON.stringify(owRespone["main"]["humidity"]);

    msg = JSON.stringify(data);
    char dataChar[msg.length() + 1];

    msg.toCharArray(dataChar, msg.length() + 1);
    client.publish("esp32/temp", dataChar);

    lastMillis = millis();
  }

}

void updateAlarmState( float tempIn, float humIn ) {
  if(tempInMinValue != 0 && tempInMaxValue != 0){
    if(tempIn > tempInMaxValue){
      // Activan alarma de temperatura interior alta
    } else if(tempIn < tempInMinValue) {
      // Activan alarma de temperatura interior muy baja
    } else {
      // Alarma desactivado
    }
  }

  if(humInMinValue != 0 && humInMaxValue != 0){
    if(humIn > humInMaxValue){
      // Activan alarma de temperatura interior alta
    } else if(humIn < humInMinValue) {
      // Activan alarma de temperatura interior muy baja
    } else {
      // Alarma desactivado
    }
  }
}

void setAlarm(String idAlarm, float min, float max) {
  if(idAlarm == "temp-in"){
    tempInMinValue = min;
    tempInMaxValue = max;
  } else if(idAlarm == "temp-ex") {

  } else if(idAlarm == "hum-in") {
    humInMinValue = min;
    humInMaxValue = max;
    
  } else if(idAlarm == "hum-ex") {
    
  }
}

void callback(char* topic, byte* payload, unsigned int length) {

  String payloadMsg;

  for(int i=0; i<length; i++){
    payloadMsg += (char)payload[i];
  }
  Serial.println(payloadMsg);

  if(String(topic) == "esp32/set-alarm"){
    JSONVar newMsg = JSON.parse(payloadMsg);

    String id = newMsg["ID"];
    float min = JSON.stringify(newMsg["values"]["min"]).toFloat();
    float max = JSON.stringify(newMsg["values"]["max"]).toFloat();

    Serial.print("ID Alarma: ");
    Serial.println(id);
    Serial.print("min: ");
    Serial.println(min);
    Serial.print("max: ");
    Serial.println(max);

    setAlarm(id, min, max);
  }

}

void reconnect( void ) {

  while (!client.connected()) {
    Serial.println("Intentando conexión a servidor MQTT...");

    if (client.connect("sgdfgsds", mqttUsername, mqttPassword)) {
      Serial.println("Conectado");
      client.subscribe("esp32/set-alarm");
    } else {
      Serial.print("Falló, rc=");
      Serial.print(client.state());
      Serial.println(" intentando en 5 seg...");
      delay(5000);
    }
  }

}

JSONVar httpGetRequest( String url ) {

  int httpCode;
  JSONVar jsonObject;
  HTTPClient http;
  
  http.begin(url);
  httpCode = http.GET();                                    
  
  if (httpCode > 0) {
  
    String jsonBuffer = http.getString();
    // Serial.println(httpCode);
    // Serial.println(jsonBuffer);

    jsonObject = JSON.parse(jsonBuffer);
  
    // if (JSON.typeof(jsonObject) == "undefined") {
    //   Serial.println("Fallo de conversion de JSON");
    //   return;
    // }

  } else {
    Serial.println("Error en la peticion");
  }
  
  http.end();

  return jsonObject;

}




