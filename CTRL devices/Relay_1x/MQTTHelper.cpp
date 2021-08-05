#include "MQTTHelper.h"

MQTTHelper::MQTTHelper(PubSubClient mqtt)
{
    client = mqtt;
}

void MQTTHelper::MQTTConnect(char broker[], int port, char user[], char password[])
{
    //----------------------------------MQTT-Client
    client.setServer(broker, port);
    client.setCallback([this](char *to, byte *pay, unsigned int len)
                       {
                           String topic = String(to);
                           char pay_char[len];
                           for (int i = 0; i < len; i++)
                           {
                               pay_char[i] = (char)pay[i];
                           }
                           String payload = String(pay_char);
                           String feed[2] = {topic, payload}; //idk why i cannot directly  insert the lamda into the vector
                           messages.push_back(feed);

                           Serial.print("received message in: ");
                           Serial.print(topic);
                           Serial.print(": ");
                           Serial.println(payload);
                       });
    int MQTTTries = 0;
    Serial.print("Connecting to MQTT: ");

    client.connect("CTRL device", user, password);
    while (!client.connected() && MQTTTries < 10)
    {
        if (client.connect("CTRL device", user, password))
        {
            Serial.println("connected");
        }
        else
        {
            Serial.print(".");
            delay(500);
            ++MQTTTries;
        }
    }
}

void MQTTHelper::mqttcallback(char *to, byte *pay, unsigned int len)
{
    String topic = String(to);
    char pay_char[len];
    for (int i = 0; i < len; i++)
    {
        pay_char[i] = (char)pay[i];
    }
    String payload = String(pay_char);
    String feed[2] = {topic, payload}; //idk why i cannot directly  insert the lamda into the vector
    messages.push_back(feed);

    Serial.print("received message in: ");
    Serial.print(topic);
    Serial.print(": ");
    Serial.println(payload);
}

void MQTTHelper::Reconnect(char user[], char password[])
{ // Loop until we're reconnected
    if (!client.connected())
    {
        if (!client.connected())
        {
            client.connect("CTRL device", user, password);
        }
        else
        {
            client.loop();
        }
    }
}