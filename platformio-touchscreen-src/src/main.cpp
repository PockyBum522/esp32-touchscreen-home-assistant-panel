#include <Arduino.h>
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


constexpr uint8_t IO_EXPANDER_TOUCH_PANEL_RESET = 1;
constexpr uint8_t IO_EXPANDER_LCD_BACKLIGHT = 2;
constexpr uint8_t IO_EXPANDER_LCD_RESET = 3;
constexpr uint8_t IO_EXPANDER_SD_CS = 4;
constexpr uint8_t IO_EXPANDER_USB_SELECT = 5;

static lv_obj_t * garageButton;


void make_background_dark(void)
{
    /*Change the active screen's background color*/
    lv_obj_set_style_bg_color(lv_screen_active(), lv_color_hex(0x111111), LV_PART_MAIN);
}

void make_hello_label(void)
{
    /*Create a white label, set its text and align it to the center*/
    lv_obj_t * label = lv_label_create(lv_screen_active());
    lv_label_set_text(label, "Hello world");
    lv_obj_set_style_text_color(lv_screen_active(), lv_color_hex(0xEEEEEE), LV_PART_MAIN);
    lv_obj_align(label, LV_ALIGN_CENTER, 0, 0);
}

void static event_garage_door_button(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t * label = lv_event_get_user_data(e);

    switch(code)
    {
        case LV_EVENT_PRESSED:
            printf("The last button event:\nLV_EVENT_PRESSED");
        break;

        case LV_EVENT_CLICKED:
            printf("The last button event:\nLV_EVENT_CLICKED");
        break;

        case LV_EVENT_LONG_PRESSED:
            printf("The last button event:\nLV_EVENT_LONG_PRESSED");
        break;

        case LV_EVENT_LONG_PRESSED_REPEAT:
            printf("The last button event:\nLV_EVENT_LONG_PRESSED_REPEAT");
        break;

        default:
            break;
    }
}

void make_garage_door_open_button()
{
    garageButton = lv_button_create(lv_screen_active());
    lv_obj_set_size(garageButton, 140, 140);
    lv_obj_center(garageButton);

    lv_obj_t * btn_label = lv_label_create(garageButton);
    lv_label_set_text(btn_label, "Garage Door");
    lv_obj_center(btn_label);

    lv_obj_add_event_cb(garageButton, event_garage_door_button, LV_EVENT_ALL, NULL);
}

void make_garage_door_button_with_style(void)
{
    static const lv_style_prop_t props[] = {LV_STYLE_BG_COLOR, LV_STYLE_BORDER_COLOR, LV_STYLE_BORDER_WIDTH, 0};

    /* A default transition, make it fast (100ms) and start with some delay (200 ms)*/
    static lv_style_transition_dsc_t trans_def;
    lv_style_transition_dsc_init(&trans_def, props, lv_anim_path_linear, 400, 3000, NULL);

    /* A special transition when going to pressed state
     * Make it slow (500 ms) but start  without delay*/
    static lv_style_transition_dsc_t trans_pr;
    lv_style_transition_dsc_init(&trans_pr, props, lv_anim_path_linear, 4000, 0, NULL);

    static lv_style_t style_def;
    lv_style_init(&style_def);
    lv_style_set_transition(&style_def, &trans_def);

    static lv_style_t style_pr;
    lv_style_init(&style_pr);
    lv_style_set_bg_color(&style_pr, lv_palette_main(LV_PALETTE_RED));
    lv_style_set_border_width(&style_pr, 66);
    lv_style_set_border_color(&style_pr, lv_palette_darken(LV_PALETTE_RED, 100));
    lv_style_set_transition(&style_pr, &trans_pr);

    make_garage_door_open_button();

    /*Create an object with the new style_pr*/
    lv_obj_add_style(garageButton, &style_def, 0);
    lv_obj_add_style(garageButton, &style_pr, LV_STATE_PRESSED);

    lv_obj_center(garageButton);
}

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

    Serial.println("Create UI");
    /* Lock the mutex due to the LVGL APIs are not thread-safe */
    lvgl_port_lock(-1);

    /* Create a simple label */
    lv_obj_t *label = lv_label_create(lv_scr_act());
    lv_label_set_text(label, title.c_str());
    lv_obj_align(label, LV_ALIGN_CENTER, 0, 0);

    // Set up my LVGL screen
    LvglComponentsInitializer::InitComponents();

    /* Release the mutex */
    lvgl_port_unlock();

    Serial.println(title + " end");

    lv_init();

    /*Output prompt information to the console, you can also use printf() to print directly*/
    LV_LOG_USER("LVGL initialization completed!");

    make_background_dark();
    make_hello_label();
    make_garage_door_button_with_style();
}


void loop()
{
    Serial.println("IDLE loop");
    delay(1000);
}
