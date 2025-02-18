#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <ESP32Time.h>
#include <ElegantOTA.h>
#include <ESP_Panel_Library.h>
#include <elapsedMillis.h>

#include "Logging/Logger.h"

bool m_debugSerialOn = false;

String m_versionNumber = "v15";
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

	// TODO: If touchscreen connection is reliable for a while, re-add OTA and see if that can be gotten working

	// TODO: If that works, see if you can download LVGL config from the network on startup and just render it

	// TODO: Add camera stills?
	// TODO: Add back porch lights, laundry room lights, cat last pilled, washer status

void setup()
{
    Serial.begin(115200);

    Serial.print(m_applicationName + " start - ");
    Serial.println(m_versionNumber);

    Serial.println("Initialize panel device");

    TouchscreenHardwareInitializer::InitLcdTouchscreenHardware();
    LvglInitializer::InitLvglToTouchscreen();

	NetworkHandlers::ConnectWifi();
	//NetworkHandlers::SetupOtaServer();

    delay(1000);
    Serial.println("Finished setup(), starting loop() - " + m_versionNumber);
}

void reconnectNetworksIfDisconnected();
void sendMqttHeartbeatEvery20Seconds();

void loop()
{
	reconnectNetworksIfDisconnected();

	m_mqttClient.loop();

	sendMqttHeartbeatEvery20Seconds();

	// TODO: Once connection is reliable, clean all this mess up

    // MQTT
	//    long long before = millis();
	//
	// const long long afterMqtt = millis() - before;

	// All of these checks are just to make it so if something takes a long time, don't run the others until loop()
	//      has finished. I don't know if LVGL runs things in between loop() but I figure it can't hurt.
	//      ...I mean it can, but...

	// // OTA
    // before = millis();
    //
    // // if (afterMqtt < 10)
    //     // ElegantOTA.loop();
    //
    // const long long afterOta = millis() - before;
    //
    // // Server
    // before = millis();
    //
    // if (afterMqtt < 10 && afterOta < 10)
    //     m_server.handleClient();
    //
    // const long long afterServer = millis() - before;

    // Reset countdown, 10 minutes
    // if (m_rtc.getLocalEpoch() > 600)
    // {
    //     // reset local epoch counter
    //     m_rtc.setTime(0);
    //
    //     m_mqttClient.publish(SECRETS::MqttTopicDeviceStatus, "10 minutes elapsed, about to restart");
    //
    //     ESP.restart();
    // }



    // Don't do anything beyond here if we don't have debug flag on
    if (!m_debugSerialOn) return;

    // if (afterServer > 10 ||
    //     afterOta > 10 ||
    //     afterMqtt > 10)
    // {
    //     Serial.println();
    //     Serial.print("server: ");
    //     Serial.print(afterServer);
    //     Serial.print(" ota: ");
    //     Serial.print(afterOta);
    //     Serial.print(" mqtt: ");
    //     Serial.println(afterMqtt);
    //     Serial.println();
    // }
}

void reconnectNetworksIfDisconnected()
{
	// Reconnect to Wi-Fi if disconnected
	if (WiFi.status() != WL_CONNECTED)
	{
		NetworkHandlers::ConnectWifi();
	}

	// Reconnect to MQTT broker if disconnected
	if (!m_mqttClient.connected())
	{
		NetworkHandlers::ConnectMqtt();
	}
}

void sendMqttHeartbeatEvery20Seconds()
{
	// Send heartbeat message every 20 seconds
	if (m_sinceLastHeartbeatMessage > 20)
	{
		m_mqttClient.publish(SECRETS::MqttTopicDeviceStatus, ("heartbeat_wd_" + m_versionNumber).c_str());

		Serial.println(("heartbeat_wd_" + m_versionNumber).c_str());

		m_sinceLastHeartbeatMessage = 0;
	}
}

