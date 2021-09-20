#include <ESP8266WiFi.h>
#include <PubSubClient.h>


char WLAN_SSID[] = ("benefrasch@gmail.com");
char WLAN_PWD[] =("D@sistM1WLAN");
char BROKER_IP[] = ("192.168.178.50");



String matrixausgabe_text  = " "; // Ausgabetext als globale Variable
volatile int matrixausgabe_index = 0;// aktuelle Position in Matrix

//-------------- definition mqtt-object ueber WiFi
WiFiClient   wiFiClient; 
PubSubClient mqttclient(wiFiClient);

//--------- list of mqtt callback functions 
#define MAX_MQTT_SUB 10 // maximal 10 subscriptions erlaubt
typedef void (*mqtthandle) (byte*,unsigned int);
typedef struct {       // Typdeklaration Callback
  String topic;        // mqtt-topic
  mqtthandle fun;      // callback function 
}
subscribe_type; 
subscribe_type mqtt_sub[MAX_MQTT_SUB];
int mqtt_sub_count=0;

String MQTT_Rx_Payload = "" ;
//--------- mqtt callback function 
void mqttcallback(char* to, byte* pay, unsigned int len) {
  String topic   = String(to);
  String payload = String((char*)pay);
  MQTT_Rx_Payload=payload.substring(0,len);
  Serial.println("\ncallback topic:" + topic + ", payload:" + MQTT_Rx_Payload);
  for (int i=0;i<mqtt_sub_count;i++) { // durchsuche alle subscriptions, bis topic passt 
    if (topic==mqtt_sub[i].topic) 
      mqtt_sub[i].fun(pay,len);         // Aufruf der richtigen callback-Funktion
  }
}

//------------ reconnect mqtt-client
void mqttreconnect() { // Loop until we're reconnected 
  if (!mqttclient.connected()) { 
    while (!mqttclient.connected()) { 
      Serial.print("Attempting MQTT connection...");
      if (mqttclient.connect("infinilight" , "infinilight", "InfiniLight" )) {
        Serial.println("connected");
        for (int i=0;i<mqtt_sub_count;i++) { // subscribe topic
          mqttclient.subscribe(mqtt_sub[i].topic.c_str());
          Serial.println("\nsubscribe");
          Serial.print(mqtt_sub[i].topic);
        }
      } 
      else { 
        Serial.print("failed, rc=");
        Serial.print(mqttclient.state());
        Serial.println(" try again in 5 seconds");
        delay(5000);
      }
    }
  } 
  else { 
    mqttclient.loop(); 
  }
}

int hey = 0 ;
String channel = "" ;
void mqtt_callback_topic_hanne(byte* pay, unsigned int len){ // ---------- my callbackfunction mqtt
  String payload = String((char*)pay); // payload als String interpretieren
  MQTT_Rx_Payload=payload.substring(0,len);    // mit Länge von len Zeichen
  Serial.println("\n in callback payload:" + MQTT_Rx_Payload +"len:"+String(len));
  Serial.print("Wert:"+MQTT_Rx_Payload);
  Serial.println();
}



void setup(){ // Einmalige Initialisierung
  Serial.begin(115200);
  //----------------------------------MQTT-Client 
  mqttclient.setServer(BROKER_IP , 1883);                                                  //|||||||||||||||||||||||||||||||||||||||||
  mqttclient.setCallback(mqttcallback);

  Serial.println();
  mqttclient.setCallback(mqttcallback);

  //--------- prepare mqtt subscription 
  mqtt_sub_count++; // add new element 
  if (mqtt_sub_count < MAX_MQTT_SUB) { 
    mqtt_sub[mqtt_sub_count-1].topic = channel;
    mqtt_sub[mqtt_sub_count-1].fun = mqtt_callback_topic_hanne; //callback function
  } 
  else Serial.println(" err max. mqtt subscription");


  //------------ WLAN initialisieren 
  WiFi.persistent(false);
  WiFi.mode(WIFI_STA);
  delay(100);
  Serial.print ("\nWLAN connect to:");
  Serial.print("WLAN_SSID");
  WiFi.begin("WLAN_SSID","WLAN_PWD");                                                         //|||||||||||||||||||||||||||||||||||||||||||||
  while (WiFi.status() != WL_CONNECTED) { // Warte bis Verbindung steht 
    delay(500); 
    Serial.print(".");
  };
  Serial.println ("\nconnected, meine IP:"+ WiFi.localIP().toString());
  matrixausgabe_text = " Meine IP:" + WiFi.localIP().toString();
  matrixausgabe_index=0;


}

void loop() { // Kontinuierliche Wiederholung 
  for (hey= 1; hey<= ( 255 ); hey=hey+1)
  {
    channel = "c"+String(hey) ;
    mqttreconnect();
  }
}
