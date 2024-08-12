#include <Arduino.h>
#include <ESP_Panel_Library.h>
#include <lvgl.h>
#include "lvgl_port_v8.h"
#include <demos/lv_demos.h>

/**
/* To use the built-in examples and demos of LVGL uncomment the includes below respectively.
 * You also need to copy `lvgl/examples` to `lvgl/src/examples`. Similarly for the demos `lvgl/demos` to `lvgl/src/demos`.
 */
// #include <demos/lv_demos.h>
// #include <examples/lv_examples.h>

constexpr uint8_t IO_EXPANDER_TOUCH_PANEL_RESET = 1 << 1;
constexpr uint8_t IO_EXPANDER_LCD_BACKLIGHT = 1 << 2;
constexpr uint8_t IO_EXPANDER_LCD_RESET = 1 << 3;
constexpr uint8_t IO_EXPANDER_SD_CS = 1 << 4;
constexpr uint8_t IO_EXPANDER_USB_SELECT = 1 << 5;

void setup()
{
    String title = "LVGL porting example";

    Serial.begin(115200);
    Serial.println(title + " start");





    Serial.println("Initialize panel device");
    ESP_Panel *panel = new ESP_Panel();
    panel->init();

    ESP_IOExpander *expander = new ESP_IOExpander_CH422G(I2C_NUM_0, ESP_IO_EXPANDER_I2C_CH422G_ADDRESS_000, GPIO_NUM_9, GPIO_NUM_8);
    expander->init();
    expander->begin();
    expander->multiPinMode(IO_EXPANDER_TOUCH_PANEL_RESET | IO_EXPANDER_LCD_BACKLIGHT | IO_EXPANDER_LCD_RESET | IO_EXPANDER_SD_CS | IO_EXPANDER_USB_SELECT, OUTPUT);
    expander->multiDigitalWrite(IO_EXPANDER_TOUCH_PANEL_RESET | IO_EXPANDER_LCD_BACKLIGHT | IO_EXPANDER_LCD_RESET, HIGH);
    delay(100);
    expander->multiDigitalWrite(IO_EXPANDER_TOUCH_PANEL_RESET | IO_EXPANDER_LCD_RESET, LOW);
    delay(100);
    digitalWrite(GPIO_NUM_4, LOW);
    delay(100);
    expander->multiDigitalWrite(IO_EXPANDER_TOUCH_PANEL_RESET | IO_EXPANDER_LCD_RESET, HIGH);
    delay(200);
#if LVGL_PORT_AVOID_TEAR
    // When avoid tearing function is enabled, configure the RGB bus according to the LVGL configuration
    ESP_PanelBus_RGB *rgb_bus = static_cast<ESP_PanelBus_RGB *>(panel->getLcd()->getBus());
    rgb_bus->configRgbFrameBufferNumber(LVGL_PORT_DISP_BUFFER_NUM);
    rgb_bus->configRgbBounceBufferSize(LVGL_PORT_RGB_BOUNCE_BUFFER_SIZE);
#endif
    panel->begin();

    Serial.println("Initialize LVGL");
    lvgl_port_init(panel->getLcd(), panel->getTouch());

    Serial.println("Create UI");
    /* Lock the mutex due to the LVGL APIs are not thread-safe */
    lvgl_port_lock(-1);

    /* Create a simple label */
    lv_obj_t *label = lv_label_create(lv_scr_act());
    lv_label_set_text(label, title.c_str());
    lv_obj_align(label, LV_ALIGN_CENTER, 0, 0);

    /**
     * Try an example. Don't forget to uncomment header.
     * See all the examples online: https://docs.lvgl.io/master/examples.html
     * source codes: https://github.com/lvgl/lvgl/tree/e7f88efa5853128bf871dde335c0ca8da9eb7731/examples
     */
    //  lv_example_btn_1();

    /**
     * Or try out a demo.
     * Don't forget to uncomment header and enable the demos in `lv_conf.h`. E.g. `LV_USE_DEMOS_WIDGETS`
     */
    // lv_demo_widgets();
    lv_demo_benchmark();
    // lv_demo_music();
    // lv_demo_stress();

    /* Release the mutex */
    lvgl_port_unlock();

    Serial.println(title + " end");
}

void loop()
{
    Serial.println("IDLE loop");
    delay(1000);
}