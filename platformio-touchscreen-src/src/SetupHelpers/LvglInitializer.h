#ifndef LVGLINITIALIZER_H
#define LVGLINITIALIZER_H


#include "Lvgl/touch_controls_init.h"           // Initialize LVGL buttons, etc...

class LvglInitializer
{
public:
    static void InitLvglToTouchscreen()
    {
        Serial.println("Initialize LVGL");
        lvgl_port_init(panel->getLcd(), panel->getTouch());

        Serial.println("Create UI");
        /* Lock the mutex due to the LVGL APIs are not thread-safe */
        lvgl_port_lock(-1);

        lvglInitComponents();

        /* Release the mutex */
        lvgl_port_unlock();

        /*Output prompt information to the console, you can also use printf() to print directly*/
        LV_LOG_USER("LVGL initialization completed!");
    }
};


#endif //LVGLINITIALIZER_H
