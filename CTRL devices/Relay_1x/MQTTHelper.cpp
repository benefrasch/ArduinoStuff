#include "MQTTHelper.h"

MQTTHelper::MQTTHelper(PubSubClient mqtt)
{
    client = mqtt;
}

void MQTTHelper::MQTTConnect(char broker[], int port, char user[], char password[])
{
    //----------------------------------MQTT-Client
    client.setServer(broker, port);
    client.setCallback([this](char *topic, byte *payload, unsigned int length)
                       {
                           String payload_str;
                           for (int i = 0; i < length; i++)
                           {
                               payload_str += (char)payload[i];
                           }

                           MQTTMessage feed = {String(topic), String(payload_str)};
                           messages[messages_count] = feed;
                           ++messages_count;

                           Serial.print("received message in: ");
                           Serial.print(topic);
                           Serial.print(": ");
                           Serial.println(payload_str);
                       });
    Serial.print("Connecting topic MQTT: ");

    if (client.connect("CTRL device", user, password))
    {
        Serial.print(" ...connected\n\n");
    }
    else
    {
        Serial.print("  ...could not connect\n\n");
    }
}

void MQTTHelper::Reconnect(char user[], char password[])
{
    if (!client.connected())
    {
        if (!client.connected())
        {
            client.connect("CTRL device", user, password);
        }
    }
    client.loop();
}