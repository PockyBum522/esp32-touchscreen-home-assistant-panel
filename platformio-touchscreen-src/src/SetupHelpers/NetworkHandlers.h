#ifndef NETWORK_HANDLERS_H
#define NETWORK_HANDLERS_H


#include <SECRETS/SECRETS.h>

class NetworkHandlers
{
public:
    static void ConnectWifi()
    {
        if (m_debugSerialOn)
        {
            Serial.print("Connecting to wifi");
            //Serial.println(ssid);
        }

        WiFi.begin(SECRETS::WifiSsid, SECRETS::WifiPsk);

        while (WiFi.status() != WL_CONNECTED)
        {
            delay(500);

            if (m_debugSerialOn)
            {
                Serial.print(".");
            }
        }

        if (m_debugSerialOn)
        {
            Serial.println("");
            Serial.println("WiFi connected.");
            Serial.println("IP address: ");
            Serial.println(WiFi.localIP());
        }
    }

    static void ConnectMqtt()
    {

        m_mqttClient.setServer(SECRETS::MqttBroker, SECRETS::MqttPort);
        m_mqttClient.setCallback(mqttCallback);

        while (!m_mqttClient.connected())
        {
            //m_server.handleClient();

            String client_id = "esp32-client-";
            client_id += String(WiFi.macAddress());

            if (m_debugSerialOn)
                Serial.printf("The client %s connects to the public MQTT broker\n", client_id.c_str());

            if (m_mqttClient.connect(client_id.c_str(), SECRETS::MqttUsername, SECRETS::MqttPassword))
            {
                if (m_debugSerialOn)
                    Serial.println("Public EMQX MQTT broker connected");
            }
            else
            {
                if (m_debugSerialOn)
                {
                    Serial.print("failed with state ");
                    Serial.print(m_mqttClient.state());
                }

                delay(100);
            }
        }

        String subscribeMessage = "Subscribing to: ";
        m_mqttClient.publish(SECRETS::MqttTopicGeneralStatus, subscribeMessage.c_str());

        String helloMessage = m_applicationName + " ESP32 up! ";
        helloMessage += m_versionNumber;
        helloMessage += " @ ";
        helloMessage += String(WiFi.localIP().toString());

        m_mqttClient.publish(SECRETS::MqttTopicGeneralStatus, helloMessage.c_str());
        m_mqttClient.publish(SECRETS::MqttTopicDeviceStatus, helloMessage.c_str());
    }

    static void SetupOtaServer()
    {
        // m_server.on("/", []() {
        //     m_server.send(200, "text/plain", "Hi! This is " + m_applicationName + " " + m_versionNumber);
        // });
        //
        // m_server.begin();
    }

private:
    static String getIncomingPayloadAsString(uint8_t *payload, unsigned int payloadLength)
    {
        String payloadStr = "";

        // Convert payload to string
        for (int i = 0; i < payloadLength; i++)
        {
            payloadStr += ((char)payload[i]);
        }

        return payloadStr;
    }

    static String getIncomingTopicAsString(char *topic)
    {
        String topicStr = "";

        // Convert topic to string
        for (int i = 0; i < strlen(topic); i++)
        {
            topicStr += ((char)topic[i]);
        }

        return topicStr;
    }

    static void mqttCallback(char *topic, byte *payload, unsigned int length)
    {
        if (m_debugSerialOn)
        {
            Serial.print("Message arrived in topic: ");
            Serial.println(topic);

            Serial.print("Message:");
        }

        String topicStr = getIncomingTopicAsString(topic);
        String payloadStr = getIncomingPayloadAsString(payload, length);

        String newMessage = "New message on: '";
        newMessage += topicStr;
        newMessage += "' with payload of: '";
        newMessage += payloadStr;
        newMessage += "'";

        m_mqttClient.publish(SECRETS::MqttTopicGeneralStatus,  newMessage.c_str());
        m_mqttClient.publish(SECRETS::MqttTopicDeviceStatus,  newMessage.c_str());

        if (topicStr == SECRETS::MqttTopicGeneralStatus)
        {
            if (payloadStr == "info")
            {
                String incomingMessage = payloadStr;
                incomingMessage += " <= New";

                m_mqttClient.publish(SECRETS::MqttTopicDeviceStatus,  incomingMessage.c_str());
            }
        }
    }
};


#endif // NETWORK_HANDLERS_H
