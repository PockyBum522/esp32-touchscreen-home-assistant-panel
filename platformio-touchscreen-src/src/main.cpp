#include "Logging/Logger.h"

#include <Arduino.h>
#include <ESP32Time.h>
#include <Secrets/Secrets.h>
#include <WiFiManager.h>          //https://github.com/tzapu/WiFiManager WiFi Configuration Magic
#include <ElegantOTA.h>
#include <ESP_Panel_Library.h>
#include <lvgl.h>
#include <lvglComponentsInitializer.h>

#include "lvgl_port_v8.h"


// You must plug/unplug the USB-C cable physically if you are seeing i2c transaction failed/i2c read errors
//
// Once you get a boot where you do not see those errors and the only console output is the IDLE loop
//
// Then you can upload new code/or reset the board as much as you want and you shouldn't see i2c errors until you
// physically unplug the board again.


bool m_debugSerialOn = false;

constexpr uint8_t IO_EXPANDER_TOUCH_PANEL_RESET = 1;
constexpr uint8_t IO_EXPANDER_LCD_BACKLIGHT = 2;
constexpr uint8_t IO_EXPANDER_LCD_RESET = 3;
constexpr uint8_t IO_EXPANDER_SD_CS = 4;
constexpr uint8_t IO_EXPANDER_USB_SELECT = 5;

// Dependency setup
auto logger = *new Logger(Information, &m_debugSerialOn);

void setup()
{
    String title = "LVGL porting example";

    Serial.begin(115200);
    Serial.println(title + " start");

    Serial.println("Initialize panel device");
    ESP_Panel *panel = new ESP_Panel();
    delay(150);
    panel->init();
    delay(150);

#if LVGL_PORT_AVOID_TEAR
    // When avoid tearing function is enabled, configure the RGB bus according to the LVGL configuration
    ESP_PanelBus_RGB *rgb_bus = static_cast<ESP_PanelBus_RGB *>(panel->getLcd()->getBus());
    delay(150);
    rgb_bus->configRgbFrameBufferNumber(LVGL_PORT_DISP_BUFFER_NUM);
    delay(150);
    rgb_bus->configRgbBounceBufferSize(LVGL_PORT_RGB_BOUNCE_BUFFER_SIZE);
    delay(150);
#endif

    panel->begin();
    delay(150);
    Serial.println("Initialize LVGL");
    lvgl_port_init(panel->getLcd(), panel->getTouch());
    delay(150);

    lv_init();

    Serial.println("Create UI");
    /* Lock the mutex due to the LVGL APIs are not thread-safe */
    lvgl_port_lock(-1);

    // Set up my LVGL screen
    LvglComponentsInitializer::InitComponents();

    /* Release the mutex */
    lvgl_port_unlock();

    Serial.println(title + " end");

    /*Output prompt information to the console, you can also use printf() to print directly*/
    LV_LOG_USER("LVGL initialization completed!");
}


void loop()
{
    Serial.println("IDLE loop");
    delay(1000);
}
