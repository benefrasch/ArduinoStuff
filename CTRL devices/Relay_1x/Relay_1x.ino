#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <EEPROM.h>
#include <SoftwareSerial.h>


SoftwareSerial bt(D1, D2);  //RX,TX

WiFiClient espClient;
PubSubClient mqttclient(espClient);

struct storage {
  char wifiSSID[30];
  char wifiPwd[80];

  char mqttServer[40];
  int mqttPort;
  char mqttUser[20];
  char mqttPwd[40];

  char mqttChannel[40];
  int minimumValue;
} storage;

void setup() {
  Serial.begin(9600);
  bt.begin(9600);
  EEPROM.begin(512);
  EEPROM.get(0, storage);
  pinMode(D7, OUTPUT);

  printConfig();

  //------------ WLAN initialisieren
  WiFi.disconnect();
  WiFi.persistent(false);
  WiFi.mode(WIFI_STA);
  delay(100);
  Serial.print("\nWLAN connect: ");
  WiFi.begin(storage.wifiSSID, storage.wifiPwd);
  int WifiTries = 0;
  while (WiFi.status() != WL_CONNECTED && WifiTries < 10) {
    delay(500);
    Serial.print(".");
    ++WifiTries;
  };
  Serial.println("\nconnected, meine IP:" + WiFi.localIP().toString() + "\n");

  //----------------------------------MQTT-Client
  mqttclient.setServer(storage.mqttServer, storage.mqttPort);
  mqttclient.setCallback(mqttcallback);

  int MQTTTries = 0;
  Serial.print("Connecting to MQTT: ");
  while (!mqttclient.connected() && MQTTTries < 10) {
    if (mqttclient.connect("CTRL device", storage.mqttUser, storage.mqttPwd)) {
      Serial.println("connected");
    } else {
      Serial.print(".");
      delay(500);
      ++MQTTTries;
    }
  }
  mqttclient.subscribe(/*storage.mqttChannel*/ "Relays/Relay1/1");

  mqttreconnect();
}

void loop() {
  mqttreconnect();
  mqttclient.loop();

  if (Serial.available() || bt.available()) {
    String message;
    if (Serial.available())
      message = Serial.readStringUntil('\n');
    if (bt.available())
      message = bt.readStringUntil('\n');

    if (message.indexOf("config") == 0) {  //config
      printConfig();
      return;

    } else if (message.indexOf("wifiSSID") == 0) {  //wifi SSID
      message.substring(9).toCharArray(storage.wifiSSID, sizeof(storage.wifiSSID));
      bsprint(String(storage.wifiSSID));

    } else if (message.indexOf("wifiPwd") == 0) {  //wifi password
      message.substring(8).toCharArray(storage.wifiPwd, sizeof(storage.wifiPwd));
      bsprint(String(storage.wifiPwd));

    } else if (message.indexOf("mqttServer") == 0) {  //mqtt server
      message.substring(11).toCharArray(storage.mqttServer, sizeof(storage.mqttServer));
      bsprint(String(storage.mqttServer));

    } else if (message.indexOf("mqttPort") == 0) {  //mqtt port
      storage.mqttPort = message.substring(9).toInt();
      bsprint(String(storage.mqttPort));

    } else if (message.indexOf("mqttUser") == 0) {  //mqtt username (not relevant)
      message.substring(9).toCharArray(storage.mqttUser, sizeof(storage.mqttUser));
      bsprint(String(storage.mqttUser));

    } else if (message.indexOf("mqttPwd") == 0) {  //mqtt password
      message.substring(8).toCharArray(storage.mqttPwd, sizeof(storage.mqttPwd));
      bsprint(String(storage.mqttPwd));

    } else if (message.indexOf("mqttChannel") == 0) {  //mqtt channel
      message.substring(12).toCharArray(storage.mqttChannel, sizeof(storage.mqttChannel));
      bsprint(String(storage.mqttChannel));

    } else if (message.indexOf("minimumValue") == 0) {  //minimum value
      storage.minimumValue = message.substring(13).toInt();
      bsprint(String(storage.minimumValue));

    } else {  //if no recognized keyword was used
      bsprint(String("wrong keyword: ") + String(message));
      return;
    }

    EEPROM.put(0, storage);
    EEPROM.commit();
  }
}

void printConfig() {
  //-----------print values to serial

  bsprint(String("wifiSSID: ") + String(storage.wifiSSID));
  bsprint(String("wifiPwd: ") + String(storage.wifiPwd));
  bsprint(String("mqttServer: ") + String(storage.mqttServer));
  bsprint(String("mqttPort: ") + String(storage.mqttPort));
  bsprint(String("mqttPwd: ") + String(storage.mqttPwd));
  bsprint(String("mqttChannel: ") + String(storage.mqttChannel));
  bsprint(String("minimumValue: ") + String(storage.minimumValue));
}

void bsprint(String message) {
  Serial.print(message + String('\n'));
  bt.print(message + String('\n'));
}

void mqttcallback(char *to, byte *pay, unsigned int len) {
  String topic = String(to);
  //String payload = String((char *)pay);
  char pay_char[len];
  for (int i = 0; i < len; i++) {
    pay_char[i] = (char)pay[i];
  }
  String payload = String(pay_char);
  Serial.print("received message in: ");
  Serial.print(topic);
  Serial.print(": ");
  Serial.println(payload);

  if (topic == storage.mqttChannel) {
    if (payload.toInt() >= storage.minimumValue) digitalWrite(D7, HIGH);
    else
      digitalWrite(D7, LOW);
  }
}

void mqttreconnect() {  // Loop until we're reconnected
  if (!mqttclient.connected()) {
    if (!mqttclient.connected()) {
      mqttclient.connect("CTRL device", storage.mqttUser, storage.mqttPwd);
    } else {
      mqttclient.loop();
    }
  }
}