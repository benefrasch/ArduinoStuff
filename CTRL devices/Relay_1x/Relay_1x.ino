#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <EEPROM.h>

WiFiClient espClient;
PubSubClient mqttclient(espClient);

struct {
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
  EEPROM.begin(512);
  EEPROM.get(0, storage);
  pinMode(D7, OUTPUT);


  //-----------print values to serial (except wifi)
  Serial.println("\ncurrent config:");
  Serial.print("wifiSSID: ");
  Serial.println(storage.wifiSSID);
  Serial.print("mqttServer: ");
  Serial.println(storage.mqttServer);
  Serial.print("mqttPort: ");
  Serial.println(storage.mqttPort);
  Serial.print("mqttUser: ");
  Serial.println(storage.mqttUser);
  Serial.print("mqttPwd: ");
  Serial.println(storage.mqttPwd);
  Serial.print("mqttChannel: ");
  Serial.println(storage.mqttChannel);
  Serial.print("minimumValue: ");
  Serial.println(storage.minimumValue);



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
  Serial.println("\nconnected, meine IP:" + WiFi.localIP().toString());

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

  if (Serial.available()) {
    String message = Serial.readStringUntil('\n');

    if (message.indexOf("wifiSSID") == 0) {
      message.substring(9).toCharArray(storage.wifiSSID, sizeof(storage.wifiSSID));
      Serial.print(storage.wifiSSID);

    } else if (message.indexOf("wifiPwd") == 0) {
      message.substring(8).toCharArray(storage.wifiPwd, sizeof(storage.wifiPwd));
      Serial.print(storage.wifiPwd);

    } else if (message.indexOf("mqttServer") == 0) {
      message.substring(11).toCharArray(storage.mqttServer, sizeof(storage.mqttServer));
      Serial.print(storage.mqttServer);

    } else if (message.indexOf("mqttPort") == 0) {
      storage.mqttPort = message.substring(9).toInt();
      Serial.print(storage.mqttPort);

    } else if (message.indexOf("mqttUser") == 0) {
      message.substring(9).toCharArray(storage.mqttUser, sizeof(storage.mqttUser));
      Serial.print(storage.mqttUser);

    } else if (message.indexOf("mqttPwd") == 0) {
      message.substring(8).toCharArray(storage.mqttPwd, sizeof(storage.mqttPwd));
      Serial.print(storage.mqttPwd);

    } else if (message.indexOf("mqttChannel") == 0) {
      message.substring(12).toCharArray(storage.mqttChannel, sizeof(storage.mqttChannel));
      Serial.print(storage.mqttChannel);

    } else if (message.indexOf("minimumValue") == 0) {
      storage.minimumValue = message.substring(13).toInt();
      Serial.print(storage.minimumValue);

    } else {
      Serial.print('wrong keyword - "');
      Serial.print(message);
      Serial.print('"');
    }

    Serial.println(" - done");
    EEPROM.put(0, storage);
    EEPROM.commit();
  }
}

void mqttcallback(char *to, byte *pay, unsigned int len) {
  String topic = String(to);
  //String payload = String((char *)pay);
  char pay_char[len];
  for (int i=0;i<len;i++) {
    pay_char[i] = (char)pay[i];
  }
  String payload = String(pay_char);
  Serial.print("received message in: ");
  Serial.print(topic);
  Serial.print(": ");
  Serial.println(payload);

  if (topic == storage.mqttChannel) {
    if (payload.toInt() >= storage.minimumValue) digitalWrite(D7, HIGH);
    else digitalWrite(D7, LOW);
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