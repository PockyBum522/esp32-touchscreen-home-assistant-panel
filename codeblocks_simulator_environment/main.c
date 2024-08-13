
/**
 * @file main
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include <stdlib.h>
#include <unistd.h>

#include "lvgl/lvgl.h"
#include "lvgl/demos/lv_demos.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

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

static void event_garage_door_button(lv_event_t * e)
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

make_garage_door_open_button()
{
    lv_obj_t * btn = lv_button_create(lv_screen_active());
    lv_obj_set_size(btn, 100, 100);
    lv_obj_center(btn);

    lv_obj_t * btn_label = lv_label_create(btn);
    lv_label_set_text(btn_label, "Garage Door");
    lv_obj_center(btn_label);

    // lv_obj_t * info_label = lv_label_create(lv_screen_active());
    // lv_label_set_text(info_label, "The last button event:\nNone");

    lv_obj_add_event_cb(btn, event_garage_door_button, LV_EVENT_ALL, NULL);
}

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR szCmdLine, int nCmdShow)
{
    /*Initialize LVGL*/
    lv_init();

    /*Initialize the HAL for LVGL*/
    lv_display_t * display = lv_windows_create_display("LVGL Simulator", 800, 480, 100, FALSE, FALSE);
    lv_windows_acquire_pointer_indev(display);

    /*Output prompt information to the console, you can also use printf() to print directly*/
    LV_LOG_USER("LVGL initialization completed!");

    make_background_dark();
    make_hello_label();
    make_garage_door_open_button();

    while(1) {
        /* Periodically call the lv_task handler.
         * It could be done in a timer interrupt or an OS task too.*/
        lv_task_handler();
        usleep(5000);       /*Just to let the system breath*/
    }
    return 0;
}
