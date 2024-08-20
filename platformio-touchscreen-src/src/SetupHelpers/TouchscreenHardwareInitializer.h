#ifndef INITIALIZERS_H
#define INITIALIZERS_H

#include "Lvgl/lvgl_port_v8.h"
#include <ESP_IOExpander_Library.h>
#include "../lvgl/lvgl.h"

// Extender IO Pin define
#define m_TP_RST 1
#define m_LCD_BL 2
#define m_LCD_RST 3
#define m_SD_CS 4
#define m_USB_SEL 5

class TouchscreenHardwareInitializer
{
public:
    static void InitLcdTouchscreenHardware()
    {
        pinMode(GPIO_INPUT_IO_4, OUTPUT);

        /**
        * These development boards require the use of an IO expander to configure the screen,
        * so it needs to be initialized in advance and registered with the panel for use.
        */

        Serial.println("Initialize IO expander");
        ESP_IOExpander *expander = new ESP_IOExpander_CH422G((i2c_port_t)I2C_MASTER_NUM, ESP_IO_EXPANDER_I2C_CH422G_ADDRESS_000, I2C_MASTER_SCL_IO, I2C_MASTER_SDA_IO);

        expander->init();
        expander->begin();
        expander->multiPinMode(m_TP_RST | m_LCD_BL | m_LCD_RST | m_SD_CS | m_USB_SEL, OUTPUT);
        expander->multiDigitalWrite(m_TP_RST | m_LCD_BL | m_LCD_RST, HIGH);
        delay(100);

        //gt911 initialization, must be added, otherwise the touch screen will not be recognized
        expander->multiDigitalWrite(m_TP_RST | m_LCD_RST, LOW);
        delay(100);
        digitalWrite(GPIO_INPUT_IO_4, LOW);
        delay(100);
        expander->multiDigitalWrite(m_TP_RST | m_LCD_RST, HIGH);
        delay(200);
        //initialization end

        Serial.println("Initialize panel device");

        panel->init();

        // #if LVGL_PORT_AVOID_TEAR (When avoid tearing function is enabled, configure the RGB bus according to the LVGL configuration)
        const auto rgb_bus = static_cast<ESP_PanelBus_RGB *>(panel->getLcd()->getBus());
        rgb_bus->configRgbFrameBufferNumber(LVGL_PORT_DISP_BUFFER_NUM);
        rgb_bus->configRgbBounceBufferSize(LVGL_PORT_RGB_BOUNCE_BUFFER_SIZE);

        panel->begin();
    }
};


#endif //INITIALIZERS_H
