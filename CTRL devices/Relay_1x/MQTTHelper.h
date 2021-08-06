#ifndef MQTTHelper_h
#define MQTTHelper_h

#include  "Arduino.h"
#include <PubSubClient.h>
#include <ESP8266WiFi.h>

class MQTTHelper : PubSubClient{
    public:
        struct MQTTMessage{
            String topic;
            String payload;
        };

        PubSubClient client;
        MQTTMessage messages[10];
        int messages_count = 0;
        MQTTHelper(PubSubClient);
        void MQTTConnect(char broker[], int port, char user[], char password[]);
        void Reconnect(char user[], char password[]);



};

#endif