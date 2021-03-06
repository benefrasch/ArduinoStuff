#include <EEPROM.h>
#include <SoftwareSerial.h>
#include "WiFiHelper.h"
#include "MQTTHelper.h"

WiFiHelper wifi;
PubSubClient mqttclient(wifi.client);
MQTTHelper mqtt(mqttclient);

SoftwareSerial bt(D1, D2); //RX,TX

struct config
{
  char wifiSSID[30];
  char wifiPwd[80];

  char mqttServer[40];
  int mqttPort;
  char mqttUser[20];
  char mqttPwd[40];

  char mqttChannel[40];
  int minimumValue;
} config;

void setup()
{
  Serial.begin(9600);
  bt.begin(9600);
  EEPROM.begin(512);
  EEPROM.get(0, config);
  wifi.WiFiConnect(config.wifiSSID, config.wifiPwd);
  mqtt.MQTTConnect(config.mqttServer, config.mqttPort, config.mqttUser, config.mqttPwd);
  printConfig();

  pinMode(D7, OUTPUT);
  mqtt.client.subscribe(config.mqttChannel);

  mqtt.client.publish("test1", "0");
}

void loop()
{
  mqtt.Reconnect(config.wifiSSID, config.wifiPwd);

  if (mqtt.messages_count > 0)
  {
    Serial.print(mqtt.messages_count);
    Serial.print("  ");
    Serial.println(mqtt.messages[0].payload);
    if (mqtt.messages[0].topic == config.mqttChannel)
    {
      if (mqtt.messages[0].payload.toInt() >= config.minimumValue)
        digitalWrite(D7, HIGH);
      else
        digitalWrite(D7, LOW);
    }
    for(int i = 0; i<10; ++i){
      mqtt.messages[i] = mqtt.messages[i+1];
    }    
    --mqtt.messages_count;
  }

  if (Serial.available() || bt.available())
  {
    String message;
    if (Serial.available())
      message = Serial.readStringUntil('\n');
    if (bt.available())
      message = bt.readStringUntil('\n');

    if (message.indexOf("config") == 0)
    { //config
      printConfig();
      return;
    }
    else if (message.indexOf("reboot") == 0)
    { //reboot for changes
      ESP.restart();
    }
    else if (message.indexOf("wifiSSID") == 0)
    { //wifi SSID
      message.substring(9).toCharArray(config.wifiSSID, sizeof(config.wifiSSID));
      bsprint(String(config.wifiSSID));
    }
    else if (message.indexOf("wifiPwd") == 0)
    { //wifi password
      message.substring(8).toCharArray(config.wifiPwd, sizeof(config.wifiPwd));
      bsprint(String(config.wifiPwd));
    }
    else if (message.indexOf("mqttServer") == 0)
    { //mqtt server
      message.substring(11).toCharArray(config.mqttServer, sizeof(config.mqttServer));
      bsprint(String(config.mqttServer));
    }
    else if (message.indexOf("mqttPort") == 0)
    { //mqtt port
      config.mqttPort = message.substring(9).toInt();
      bsprint(String(config.mqttPort));
    }
    else if (message.indexOf("mqttUser") == 0)
    { //mqtt username (not relevant)
      message.substring(9).toCharArray(config.mqttUser, sizeof(config.mqttUser));
      bsprint(String(config.mqttUser));
    }
    else if (message.indexOf("mqttPwd") == 0)
    { //mqtt password
      message.substring(8).toCharArray(config.mqttPwd, sizeof(config.mqttPwd));
      bsprint(String(config.mqttPwd));
    }
    else if (message.indexOf("mqttChannel") == 0)
    { //mqtt channel
      message.substring(12).toCharArray(config.mqttChannel, sizeof(config.mqttChannel));
      bsprint(String(config.mqttChannel));
    }
    else if (message.indexOf("minimumValue") == 0)
    { //minimum value
      config.minimumValue = message.substring(13).toInt();
      bsprint(String(config.minimumValue));
    }
    else
    { //if no recognized keyword was used
      bsprint(String("wrong keyword: ") + String(message));
      return;
    }

    EEPROM.put(0, config);
    EEPROM.commit();
  }
}

void printConfig()
{
  //-----------print values to serial

  bsprint("CONFIG: ");
  bsprint(String("wifiSSID: ") + String(config.wifiSSID));
  bsprint(String("wifiPwd: ") + String(config.wifiPwd));
  bsprint(String("mqttServer: ") + String(config.mqttServer));
  bsprint(String("mqttPort: ") + String(config.mqttPort));
  bsprint(String("mqttUser: ") + String(config.mqttUser));
  bsprint(String("mqttPwd: ") + String(config.mqttPwd));
  bsprint("");

  bsprint(String("mqttChannel: ") + String(config.mqttChannel));
  bsprint(String("minimumValue: ") + String(config.minimumValue));
}

void bsprint(String message)
{
  Serial.print(message + String('\n'));
  bt.print(message + String('\n'));
}
