#ifndef MQTTHelper_h
#define MQTTHelper_h

#include  "Arduino.h"
#include <PubSubClient.h>
#include <Vector.h>
#include <ESP8266WiFi.h>

class MQTTHelper : PubSubClient{
    public:
        PubSubClient client;
        Vector<Vector<String>> messages;
        MQTTHelper(PubSubClient);
        void MQTTConnect(char broker[], int port, char user[], char password[]);
        void Reconnect(char user[], char password[]);

    private:
        void mqttcallback(char *to, byte *pay, unsigned int len);
};

#endif