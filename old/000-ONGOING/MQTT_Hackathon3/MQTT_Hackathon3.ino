//-----------------------------------------------------------------------------------------------------------------
const char* ssid = "";
const char* password =  ""
const char* mqttServer = "infinilight";
const int mqttPort = 1883;
const char* mqttUser = "infinilight";
const char* mqttPassword = "InfiniLight";

String channel_name = "devices/dmx_1/"; //der channel ist das und dahinter noch die zahlen der channels
//-----------------------------------------------------------------------------------------------------------------
//----------|CLients intitialisieren|-------------------------------------------------------
#include <ESPDMX.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <PubSubClient.h>

WiFiClient espClient;
PubSubClient client(espClient);
DMXESPSerial DMX;
//----------|WLAN verbinden|-----------------------------------------------------------------
void connectwifi() {
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi..");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("Connected to the WiFi network");
}
//----------|MQTT verbinden|-----------------------------------------------------------------
void connectmqtt() {-
  client.setServer(mqttServer, mqttPort);
  client.setCallback(callback);
  while (!client.connected()) {
    Serial.println();
    Serial.print("Connecting to MQTT...");
    if (client.connect("ESP8266Client", mqttUser, mqttPassword )) {
      Serial.print("connected");
    } else {
      Serial.print("failed with state ");
      Serial.print(client.state());
      delay(2000);
    }
    Serial.println();
  }
}

void setup() {
  //----------|WLAN und MQTT initialisieren|----------
  Serial.begin(115200);
  connectwifi();
  connectmqtt();
  DMX.init();

  //----------|subscriben|----------
  for (int i = 1; i < 256; i++) {
    String cache = channel_name + i;
    char mqttchannel[channel_name.length() + 4];
    cache.toCharArray(mqttchannel, channel_name.length() + 4);
    client.subscribe(mqttchannel);
    Serial.println(mqttchannel);
  }
}

//----------|falls MQTT Nachricht eintrifft|----------
void callback(char* topic, byte* payload, unsigned int length) {
  //topic als string verfassen
  String topic_string;
  for (int i = 0; i < (channel_name.length() + 3); i++) {
    topic_string = topic_string + (char)topic[i];
  }
  //channel_name entfernen, damit nur noch channel als int vorhanden
  topic_string.remove(0, channel_name.length());

  //nachricht als string verfassen
  String message_string;
  for (int i = 0; i < length; i++) {
    message_string = message_string + (char)payload[i];
  }

  Serial.println("Nachricht für Channel: " + topic_string + "    mit Wert: " + message_string);

  DMX.write(topic_string.toInt(), message_string.toInt());
  DMX.update();
}


void loop() {
  //----------|falls Verbindung verloren geht|----------
  if (WiFi.status() != WL_CONNECTED) {
    connectwifi();
  }
  if (!client.connected()) {
    connectmqtt();
  }
  //----------|MQTT abfragen|----------
  client.loop();

}
