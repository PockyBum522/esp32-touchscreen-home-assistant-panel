#include <Arduino.h>
#include "lvglComponentsInitializer.h"
#include <cstdint>
#include <extra/widgets/chart/lv_chart.h>
#include <src/widgets/lv_label.h>

static void profile_create(lv_obj_t * parent);

static lv_obj_t * tv;
static lv_style_t style_text_muted;
static lv_style_t style_title;
static lv_style_t style_icon;
static lv_style_t style_bullet;

static const lv_font_t * font_large;
static const lv_font_t * font_normal;


// Mine
static lv_obj_t * garageButton;

void make_background_dark()
{
    /*Change the active screen's background color*/
    lv_obj_set_style_bg_color(lv_scr_act(), lv_color_hex(0x111111), LV_PART_MAIN);
}

void static event_garage_door_button(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);
    //lv_obj_t * label = lv_event_get_user_data(e);

    switch(code)
    {
        case LV_EVENT_PRESSED:
            Serial.println("The last button event:\nLV_EVENT_PRESSED");
        break;

        case LV_EVENT_CLICKED:
            Serial.println("The last button event:\nLV_EVENT_CLICKED");
        break;

        case LV_EVENT_LONG_PRESSED:
            Serial.println("The last button event:\nLV_EVENT_LONG_PRESSED");
        break;

        case LV_EVENT_LONG_PRESSED_REPEAT:
            Serial.println("The last button event:\nLV_EVENT_LONG_PRESSED_REPEAT");
        break;

        default:
            break;
    }
}

void make_garage_door_open_button()
{
    garageButton = lv_btn_create(lv_scr_act());
    lv_obj_set_size(garageButton, 140, 140);
    lv_obj_center(garageButton);

    lv_obj_t * btn_label = lv_label_create(garageButton);
    lv_label_set_text(btn_label, "Garage Door");
    lv_obj_center(btn_label);

    lv_obj_add_event_cb(garageButton, event_garage_door_button, LV_EVENT_ALL, NULL);
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

    make_garage_door_open_button();

    /*Create an object with the new style_pr*/
    lv_obj_add_style(garageButton, &style_def, 0);
    lv_obj_add_style(garageButton, &style_pr, LV_STATE_PRESSED);

    lv_obj_center(garageButton);
}

void LvglComponentsInitializer::InitComponents()
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

}