#include <Arduino.h>
#include <ESP_Panel_Library.h>
#include <lvgl.h>
#include <cstdint>
#include <src/widgets/lv_label.h>
#include <ESP_IOExpander_Library.h>

#include "lvgl_port_v8.h"

bool m_debugSerialOn = true;

String m_versionNumber = "v01";
String m_applicationName = "Den Touchscreen";

// Extend IO Pin define
#define TP_RST 1
#define LCD_BL 2
#define LCD_RST 3
#define SD_CS 4
#define USB_SEL 5

// LVGL stuff
static lv_style_t style_text_muted;
static lv_style_t style_title;

static const lv_font_t * font_large;
static const lv_font_t * font_normal;

// Mine
static lv_obj_t * garageButton;
static lv_obj_t * denLightsButton;
static lv_obj_t * tvButton;


void connectWifi();
void connectMqtt();
void mqttCallback(char *topic, byte *payload, unsigned int length);
void lvglInitComponents();

void setup()
{
    if (m_debugSerialOn)
    {
        Serial.begin(115200);

        Serial.print(m_applicationName + " start - ");
        Serial.println(m_versionNumber);

        Serial.println("Initialize panel device");
    }

     pinMode(GPIO_INPUT_IO_4, OUTPUT);

    /**
     * These development boards require the use of an IO expander to configure the screen,
     * so it needs to be initialized in advance and registered with the panel for use.
     */

    Serial.println("Initialize IO expander");
    ESP_IOExpander *expander = new ESP_IOExpander_CH422G((i2c_port_t)I2C_MASTER_NUM, ESP_IO_EXPANDER_I2C_CH422G_ADDRESS_000, I2C_MASTER_SCL_IO, I2C_MASTER_SDA_IO);

    expander->init();
    expander->begin();
    expander->multiPinMode(TP_RST | LCD_BL | LCD_RST | SD_CS | USB_SEL, OUTPUT);
    expander->multiDigitalWrite(TP_RST | LCD_BL | LCD_RST, HIGH);
    delay(100);

    //gt911 initialization, must be added, otherwise the touch screen will not be recognized
    expander->multiDigitalWrite(TP_RST | LCD_RST, LOW);
    delay(100);
    digitalWrite(GPIO_INPUT_IO_4, LOW);
    delay(100);
    expander->multiDigitalWrite(TP_RST | LCD_RST, HIGH);
    delay(200);
    //initialization end
    
    Serial.println("Initialize panel device");
    ESP_Panel *panel = new ESP_Panel();
    panel->init();
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

    lvglInitComponents();

    /* Release the mutex */
    lvgl_port_unlock();
}

void loop()
{
    Serial.println("IDLE loop");
    delay(1000);
}


void make_background_dark()
{
    /*Change the active screen's background color*/
    lv_obj_set_style_bg_color(lv_scr_act(), lv_color_hex(0x111111), LV_PART_MAIN);
}

void static event_button(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);
    //lv_obj_t * label = lv_event_get_user_data(e);

    // Cast a dynamically allocated string to 'void*'.
    void *vp = static_cast<void*>(new std::string("it's easy to break stuff like this!"));

    // Then, in the function that's using the UserEvent:
    // Cast it back to a string pointer.
    auto *sp = static_cast<std::string*>(lv_event_get_user_data(e));

    // You could use 'sp' directly, or this, which does a copy.
    std::string s = *sp;

    switch(code)
    {
        case LV_EVENT_PRESSED:
            Serial.println("Event: Button pressed");
            //mqttClient.publish(SECRETS::MqttTopicButtonsPressed,  (s + "_pressed").c_str());
        break;

        case LV_EVENT_CLICKED:
            // Don't think we need this as pressed always fires
                //mqttClient.publish(SECRETS::MqttTopicButtonsPressed, "den_lights_button_pressed");
                    break;

        case LV_EVENT_LONG_PRESSED:
            Serial.println("Event: Button LONG pressed");
            //mqttClient.publish(SECRETS::MqttTopicButtonsPressed, (s + "_long_pressed").c_str());
        break;

        case LV_EVENT_LONG_PRESSED_REPEAT:
            Serial.println("Event: Button long pressed (repeat)");
            //mqttClient.publish(SECRETS::MqttTopicButtonsPressed, (s + "_long_pressed_repeat").c_str());
        break;

        default:
            break;
    }

    // Don't forget to destroy the memory that you've allocated.
    // delete sp;
}

void make_garage_door_button_with_style()
{
    static constexpr lv_style_prop_t props[] = {LV_STYLE_BG_COLOR, LV_STYLE_BORDER_COLOR, LV_STYLE_BORDER_WIDTH};

    /* A default transition, make it fast (100ms) and start with some delay (200 ms)*/
    static lv_style_transition_dsc_t trans_def;
    lv_style_transition_dsc_init(&trans_def, props, lv_anim_path_linear, 400, 3000, NULL);

    /* A special transition when going to pressed state. Make it slow (500 ms) but start  without delay*/
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

    // Make the actual button
    garageButton = lv_btn_create(lv_scr_act());
    lv_obj_set_size(garageButton, 140, 300);
    lv_obj_set_pos(garageButton, 20, 20);

    lv_obj_t * btn_label = lv_label_create(garageButton);

    lv_label_set_text(btn_label, "Garage Door");
    static std::string buttonName = "garage_button";

    lv_obj_center(btn_label);

    lv_obj_add_event_cb(garageButton, event_button, LV_EVENT_ALL, &buttonName);

    /*Create an object with the new style_pr*/
    lv_obj_add_style(garageButton, &style_def, 0);
    lv_obj_add_style(garageButton, &style_pr, LV_STATE_PRESSED);
}

void make_den_lights_button_with_style()
{
    static constexpr lv_style_prop_t props[] = {LV_STYLE_BG_COLOR, LV_STYLE_BORDER_COLOR, LV_STYLE_BORDER_WIDTH};

    /* A default transition, make it fast (100ms) and start with some delay (200 ms)*/
    static lv_style_transition_dsc_t trans_def;
    lv_style_transition_dsc_init(&trans_def, props, lv_anim_path_linear, 200, 200, NULL);

    /* A special transition when going to pressed state. Make it slow (500 ms) but start  without delay*/
    static lv_style_transition_dsc_t trans_pr;
    lv_style_transition_dsc_init(&trans_pr, props, lv_anim_path_linear, 500, 0, NULL);

    static lv_style_t style_def;
    lv_style_init(&style_def);
    lv_style_set_transition(&style_def, &trans_def);

    static lv_style_t style_pr;
    lv_style_init(&style_pr);
    lv_style_set_bg_color(&style_pr, lv_palette_darken(LV_PALETTE_RED, 200));
    lv_style_set_border_width(&style_pr, 66);
    lv_style_set_border_color(&style_pr, lv_palette_darken(LV_PALETTE_RED, 100));
    lv_style_set_transition(&style_pr, &trans_pr);


    // Make the actual button
    denLightsButton = lv_btn_create(lv_scr_act());
    lv_obj_set_size(denLightsButton, 100, 300);
    lv_obj_set_pos(denLightsButton, 200, 20);

    lv_obj_t * btn_label = lv_label_create(denLightsButton);

    lv_label_set_text(btn_label, "Den Lights");
    static std::string buttonName = "den_lights_button";

    lv_obj_center(btn_label);
    lv_obj_add_event_cb(denLightsButton, event_button, LV_EVENT_ALL, &buttonName);

    /*Create an object with the new style_pr*/
    lv_obj_add_style(denLightsButton, &style_def, 0);
    lv_obj_add_style(denLightsButton, &style_pr, LV_STATE_PRESSED);

    //lv_obj_center(denLightsButton);
}

void make_tv_button_with_style()
{
    static constexpr lv_style_prop_t props[] = {LV_STYLE_BG_COLOR, LV_STYLE_BORDER_COLOR, LV_STYLE_BORDER_WIDTH};

    /* A default transition, make it fast (100ms) and start with some delay (200 ms)*/
    static lv_style_transition_dsc_t trans_def;
    lv_style_transition_dsc_init(&trans_def, props, lv_anim_path_linear, 200, 200, NULL);

    /* A special transition when going to pressed state. Make it slow (500 ms) but start  without delay*/
    static lv_style_transition_dsc_t trans_pr;
    lv_style_transition_dsc_init(&trans_pr, props, lv_anim_path_linear, 500, 0, NULL);

    static lv_style_t style_def;
    lv_style_init(&style_def);
    lv_style_set_transition(&style_def, &trans_def);

    static lv_style_t style_pr;
    lv_style_init(&style_pr);
    lv_style_set_bg_color(&style_pr, lv_palette_darken(LV_PALETTE_RED, 200));
    lv_style_set_border_width(&style_pr, 66);
    lv_style_set_border_color(&style_pr, lv_palette_darken(LV_PALETTE_RED, 100));
    lv_style_set_transition(&style_pr, &trans_pr);


    // Make the actual button
    tvButton = lv_btn_create(lv_scr_act());
    lv_obj_set_size(tvButton, 100, 300);
    lv_obj_set_pos(tvButton, 400, 20);

    lv_obj_t * btn_label = lv_label_create(tvButton);

    lv_label_set_text(btn_label, "TV Power");
    static std::string buttonName = "tv_power_button";

    lv_obj_center(btn_label);
    lv_obj_add_event_cb(tvButton, event_button, LV_EVENT_ALL, &buttonName);

    /*Create an object with the new style_pr*/
    lv_obj_add_style(tvButton, &style_def, 0);
    lv_obj_add_style(tvButton, &style_pr, LV_STATE_PRESSED);

    //lv_obj_center(denLightsButton);
}

void lvglInitComponents()
{
    font_large = LV_FONT_DEFAULT;
    font_normal = LV_FONT_DEFAULT;

    lv_theme_default_init(NULL, lv_palette_main(LV_PALETTE_GREEN), lv_palette_main(LV_PALETTE_RED), LV_THEME_DEFAULT_DARK, font_normal);

    lv_style_init(&style_text_muted);
    lv_style_set_text_opa(&style_text_muted, LV_OPA_50);

    lv_style_init(&style_title);
    lv_style_set_text_font(&style_title, font_large);

    make_background_dark();

    make_garage_door_button_with_style();

    make_den_lights_button_with_style();

    make_tv_button_with_style();
}