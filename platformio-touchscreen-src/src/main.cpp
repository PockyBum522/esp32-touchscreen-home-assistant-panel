#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <ESP32Time.h>
#include <ArduinoOTA.h>
#include <ESP_Panel_Library.h>
#include <WebServer.h>

#include "Logging/Logger.h"

bool m_debugSerialOn = true;

String m_versionNumber = "v07";
String m_applicationName = "Den Touchscreen";

bool m_otaUpdateStarted = false;

//auto m_logger = *new Logger(Information, &m_debugSerialOn);
//WebServer m_server(80);
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

    ArduinoOTA.setPort(46343);

    ArduinoOTA
          .onStart([]() {
            String type;
            if (ArduinoOTA.getCommand() == U_FLASH) {
              type = "sketch";
            } else {  // U_SPIFFS
              type = "filesystem";
            }

            // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
            Serial.println("Start updating " + type);
          })
          .onEnd([]() {
            Serial.println("\nEnd");
          })
          .onProgress([](unsigned int progress, unsigned int total) {
            Serial.printf("Progress: %u%%\r", (progress / (total / 100)));

            if (!m_otaUpdateStarted) {
              m_otaUpdateStarted = true;
              Serial.println("Setting OTA flag to be the only thing that happens");
            }
          })
          .onError([](ota_error_t error) {
            Serial.printf("Error[%u]: ", error);
            if (error == OTA_AUTH_ERROR) {
              Serial.println("Auth Failed");
            } else if (error == OTA_BEGIN_ERROR) {
              Serial.println("Begin Failed");
            } else if (error == OTA_CONNECT_ERROR) {
              Serial.println("Connect Failed");
            } else if (error == OTA_RECEIVE_ERROR) {
              Serial.println("Receive Failed");
            } else if (error == OTA_END_ERROR) {
              Serial.println("End Failed");
            }
          });

    ArduinoOTA.begin();
}

long counter = 10;

void loop()
{
  // All of these checks are just to make it so if something takes a long time, don't run the others until loop()
  //      has finished. I don't know if LVGL runs things in between loop() but I figure it can't hurt.
  //      ...I mean it can, but...

  ArduinoOTA.handle();

  if (counter-- < 3)
  {
    counter = 3000;
    Serial.println(m_versionNumber);
  }

  // while(m_otaUpdateStarted)
  // {
  //   ArduinoOTA.handle();
  //   yield();
  // }
  //
  // // MQTT
  // long long before = millis();
  // m_mqttClient.loop();
  // const long long afterMqtt = millis() - before;
  //
  // ArduinoOTA.handle();
  //
  // // OTA
  // before = millis();
  //
  // if (afterMqtt < 10)
  // {
  //   ArduinoOTA.handle();
  // }
  //
  // const long long afterOta = millis() - before;
  //
  // // Server
  // before = millis();
  //
  // // if (afterMqtt < 10 && afterOta < 10)
  // //   m_server.handleClient();
  //
  // ArduinoOTA.handle();
  //
  // const auto afterServer = millis() - before;
  //
  // // Don't do anything beyond here if we don't have debug flag on
  // if (!m_debugSerialOn) return;
  //
  // if (afterServer > 10 ||
  //   afterOta > 10 ||
  //   afterMqtt > 10)
  // {
  //   Serial.println();
  //   Serial.print("server: ");
  //   Serial.print(afterServer);
  //   Serial.print(" ota: ");
  //   Serial.print(afterOta);
  //   Serial.print(" mqtt: ");
  //   Serial.println(afterMqtt);
  //   Serial.println();
  // }
}
