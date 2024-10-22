#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <ESP32Time.h>
#include <ElegantOTA.h>
#include <ESP_Panel_Library.h>
#include <elapsedMillis.h>

#include "Logging/Logger.h"

bool m_debugSerialOn = false;

String m_versionNumber = "v14";
String m_applicationName = "Den Touchscreen";

elapsedSeconds m_sinceLastHeartbeatMessage;

auto m_logger = *new Logger(Information, &m_debugSerialOn);
WebServer m_server(80);
WiFiClient m_espClient;
PubSubClient m_mqttClient(m_espClient);
ESP32Time m_rtc(0);

auto panel = new ESP_Panel();

#include "SetupHelpers/TouchscreenHardwareInitializer.h"
#include "SetupHelpers/LvglInitializer.h"
#include "SetupHelpers/NetworkHandlers.h"

void setup()
{
    Serial.begin(115200);

    Serial.print(m_applicationName + " start - ");
    Serial.println(m_versionNumber);

    Serial.println("Initialize panel device");

    TouchscreenHardwareInitializer::InitLcdTouchscreenHardware();
    LvglInitializer::InitLvglToTouchscreen();

    NetworkHandlers::ConnectWifi();
    NetworkHandlers::SetupOtaServer();
    NetworkHandlers::ConnectMqtt();

    delay(1000);
    Serial.println("Finished setup(), starting loop() - " + m_versionNumber);
}

void loop()
{
    // All of these checks are just to make it so if something takes a long time, don't run the others until loop()
    //      has finished. I don't know if LVGL runs things in between loop() but I figure it can't hurt.
    //      ...I mean it can, but...

    // MQTT
    long long before = millis();
    m_mqttClient.loop();
    const long long afterMqtt = millis() - before;

    // OTA
    before = millis();

    if (afterMqtt < 10)
        ElegantOTA.loop();

    const long long afterOta = millis() - before;

    // Server
    before = millis();

    if (afterMqtt < 10 && afterOta < 10)
        m_server.handleClient();

    const long long afterServer = millis() - before;

    // Reset countdown, 10 minutes
    if (m_rtc.getLocalEpoch() > 600)
    {
        // reset local epoch counter
        m_rtc.setTime(0);

        m_mqttClient.publish(SECRETS::MqttTopicDeviceStatus, "10 minutes elapsed, about to restart");

        ESP.restart();
    }

    // Send heartbeat message every 20 seconds
    if (m_sinceLastHeartbeatMessage > 20)
    {
        m_mqttClient.publish(SECRETS::MqttTopicDeviceStatus, ("heartbeat_wd_" + m_versionNumber).c_str());

        Serial.println(("heartbeat_wd_" + m_versionNumber).c_str());

        m_sinceLastHeartbeatMessage = 0;
    }

    // Don't do anything beyond here if we don't have debug flag on
    if (!m_debugSerialOn) return;

    if (afterServer > 10 ||
        afterOta > 10 ||
        afterMqtt > 10)
    {
        Serial.println();
        Serial.print("server: ");
        Serial.print(afterServer);
        Serial.print(" ota: ");
        Serial.print(afterOta);
        Serial.print(" mqtt: ");
        Serial.println(afterMqtt);
        Serial.println();
    }
}

