#ifndef COROLLA_2003_NFC_CONTROLLER_SECRETS_H
#define COROLLA_2003_NFC_CONTROLLER_SECRETS_H


class SECRETS
{
public:
    static String WifiSsid;
    static String WifiPsk;

    // MQTT Broker
    static const char* MqttBroker;
    static const char* MqttUsername;
    static const char* MqttPassword;
    static const int   MqttPort;

    // Device publish topics
    static const char* MqttTopicGeneralStatus;
    static const char* MqttTopicDeviceStatus;
    static const char* MqttTopicButtonsPressed;
};


#endif //COROLLA_2003_NFC_CONTROLLER_SECRETS_H
