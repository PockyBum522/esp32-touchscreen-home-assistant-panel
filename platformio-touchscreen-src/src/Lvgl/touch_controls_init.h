#ifndef TOUCH_CONTROLS_INIT_H
#define TOUCH_CONTROLS_INIT_H


#include "lvgl_port_v8.h"
#include "SECRETS/SECRETS.h"

// LVGL stuff
static lv_style_t style_text_muted;
static lv_style_t style_title;

static const lv_font_t * font_large;
static const lv_font_t * font_normal;

// Mine
static lv_obj_t * garageButton;
static lv_obj_t * denLightsButton;
static lv_obj_t * tvButton;



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
            m_mqttClient.publish(SECRETS::MqttTopicButtonsPressed,  (s + "_pressed").c_str());
        break;

        case LV_EVENT_CLICKED:
            // Don't think we need this as pressed always fires
                //mqttClient.publish(SECRETS::MqttTopicButtonsPressed, "den_lights_button_pressed");
                    break;

        case LV_EVENT_LONG_PRESSED:
            m_mqttClient.publish(SECRETS::MqttTopicButtonsPressed, (s + "_long_pressed").c_str());
        break;

        case LV_EVENT_LONG_PRESSED_REPEAT:
            m_mqttClient.publish(SECRETS::MqttTopicButtonsPressed, (s + "_long_pressed_repeat").c_str());
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

#endif //TOUCH_CONTROLS_INIT_H
