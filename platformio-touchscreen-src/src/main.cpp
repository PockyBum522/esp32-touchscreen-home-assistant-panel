#include <Arduino.h>
#include <Secrets/Secrets.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <ESP32Time.h>
#include <ElegantOTA.h>
#include <ESP_Panel_Library.h>
#include <lvgl.h>
#include <cstdint>
#include <src/widgets/lv_label.h>

#include "Logging/Logger.h"
#include "lvgl_port_v8.h"


// You must plug/unplug the USB-C cable physically if you are seeing i2c transaction failed/i2c read errors
//
// Once you get a boot where you do not see those errors and the only console output is the IDLE loop
//
// Then you can upload new code/or reset the board as much as you want and you shouldn't see i2c errors until you
// physically unplug the board again.


bool m_debugSerialOn = true;

String m_versionNumber = "v01";
String m_applicationName = "Den Touchscreen";

constexpr uint8_t IO_EXPANDER_TOUCH_PANEL_RESET = 1;
constexpr uint8_t IO_EXPANDER_LCD_BACKLIGHT = 2;
constexpr uint8_t IO_EXPANDER_LCD_RESET = 3;
constexpr uint8_t IO_EXPANDER_SD_CS = 4;
constexpr uint8_t IO_EXPANDER_USB_SELECT = 5;

// Dependency setup
auto logger = *new Logger(Information, &m_debugSerialOn);
WebServer server(80);
WiFiClient espClient;
PubSubClient mqttClient(espClient);
ESP32Time rtc(0);

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


    auto *panel = new ESP_Panel();

    delay(150);
    panel->init();
    delay(150);

#if LVGL_PORT_AVOID_TEAR

    // When avoid tearing function is enabled, configure the RGB bus according to the LVGL configuration
    auto *rgb_bus = static_cast<ESP_PanelBus_RGB *>(panel->getLcd()->getBus());
    delay(150);
    rgb_bus->configRgbFrameBufferNumber(LVGL_PORT_DISP_BUFFER_NUM);
    delay(150);
    rgb_bus->configRgbBounceBufferSize(LVGL_PORT_RGB_BOUNCE_BUFFER_SIZE);
    delay(150);

#endif

    panel->begin();
    delay(150);

    if (m_debugSerialOn)
        Serial.println("Initialize LVGL");

    lvgl_port_init(panel->getLcd(), panel->getTouch());
    delay(150);

    lv_init();

    if (m_debugSerialOn)
        Serial.println("Create UI");

    /* Lock the mutex due to the LVGL APIs are not thread-safe */
    lvgl_port_lock(-1);

    // Set up my LVGL screen
    lvglInitComponents();

    /* Release the mutex */
    lvgl_port_unlock();

    if (m_debugSerialOn)
        Serial.println(m_versionNumber + " end");

    /*Output prompt information to the console, you can also use printf() to print directly*/
    LV_LOG_USER("LVGL initialization completed!");

    connectWifi();

    server.on("/", []() {
        server.send(200, "text/plain", "Hi! This is " + m_applicationName);
    });

    ElegantOTA.begin(&server);    // Start ElegantOTA
    server.begin();

    connectMqtt();
}

void loop()
{
    // if (m_debugSerialOn)
    //     Serial.println("IDLE loop");

    server.handleClient();

    ElegantOTA.loop();

    mqttClient.loop();

    // Restart after 5 minutes to reset vertical screen shifting issue
    if (rtc.getLocalEpoch() > 300)
    {
        rtc.setTime(0);
        ESP.restart();
    }

    // Delay that helps (slightly) with vertical screen shifting issue
    delay(200);
}


void connectWifi()
{
    if (m_debugSerialOn)
    {
        Serial.print("Connecting to wifi");
        //Serial.println(ssid);
    }

    WiFi.begin(SECRETS::WifiSsid, SECRETS::WifiPsk);

    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);

        if (m_debugSerialOn)
        {
            Serial.print(".");
        }
    }

    if (m_debugSerialOn)
    {
        Serial.println("");
        Serial.println("WiFi connected.");
        Serial.println("IP address: ");
        Serial.println(WiFi.localIP());
    }
}

void connectMqtt()
{

    mqttClient.setServer(SECRETS::MqttBroker, SECRETS::MqttPort);
    mqttClient.setCallback(mqttCallback);

    while (!mqttClient.connected())
    {
        server.handleClient();
        ElegantOTA.loop();

        String client_id = "esp32-client-";
        client_id += String(WiFi.macAddress());

        if (m_debugSerialOn)
            Serial.printf("The client %s connects to the public MQTT broker\n", client_id.c_str());

        if (mqttClient.connect(client_id.c_str(), SECRETS::MqttUsername, SECRETS::MqttPassword))
        {
            if (m_debugSerialOn)
                Serial.println("Public EMQX MQTT broker connected");
        }
        else
        {
            if (m_debugSerialOn)
            {
                Serial.print("failed with state ");
                Serial.print(mqttClient.state());
            }

            delay(100);
        }
    }

    String subscribeMessage = "Subscribing to: ";
    mqttClient.publish(SECRETS::MqttTopicGeneralStatus, subscribeMessage.c_str());

    String helloMessage = m_applicationName + " ESP32 up! ";
    helloMessage += m_versionNumber;
    helloMessage += " @ ";
    helloMessage += String(WiFi.localIP().toString());

    mqttClient.publish(SECRETS::MqttTopicGeneralStatus, helloMessage.c_str());
    mqttClient.publish(SECRETS::MqttTopicDeviceStatus, helloMessage.c_str());
}

String getIncomingPayloadAsString(uint8_t *payload, unsigned int payloadLength)
{
    String payloadStr = "";

    // Convert payload to string
    for (int i = 0; i < payloadLength; i++)
    {
        payloadStr += ((char)payload[i]);
    }

    return payloadStr;
}

String getIncomingTopicAsString(char *topic)
{
    String topicStr = "";

    // Convert topic to string
    for (int i = 0; i < strlen(topic); i++)
    {
        topicStr += ((char)topic[i]);
    }

    return topicStr;
}

void mqttCallback(char *topic, byte *payload, unsigned int length)
{
    if (m_debugSerialOn)
    {
        Serial.print("Message arrived in topic: ");
        Serial.println(topic);

        Serial.print("Message:");
    }

    String topicStr = getIncomingTopicAsString(topic);
    String payloadStr = getIncomingPayloadAsString(payload, length);

    String newMessage = "New message on: '";
    newMessage += topicStr;
    newMessage += "' with payload of: '";
    newMessage += payloadStr;
    newMessage += "'";

    mqttClient.publish(SECRETS::MqttTopicGeneralStatus,  newMessage.c_str());
    mqttClient.publish(SECRETS::MqttTopicDeviceStatus,  newMessage.c_str());

    if (topicStr == SECRETS::MqttTopicGeneralStatus)
    {
        if (payloadStr == "info")
        {
            String incomingMessage = payloadStr;
            incomingMessage += " <= New";

            mqttClient.publish(SECRETS::MqttTopicDeviceStatus,  incomingMessage.c_str());
        }
    }
}


void make_background_dark()
{
    /*Change the active screen's background color*/
    lv_obj_set_style_bg_color(lv_scr_act(), lv_color_hex(0x111111), LV_PART_MAIN);
}

void static event_button(lv_event_t * e)
{
    return; // FOR TESTING SO MQTT EVENTS ARE NOT SENT

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
            mqttClient.publish(SECRETS::MqttTopicButtonsPressed,  (s + "_pressed").c_str());
        break;

        case LV_EVENT_CLICKED:
            // Don't think we need this as pressed always fires
                //mqttClient.publish(SECRETS::MqttTopicButtonsPressed, "den_lights_button_pressed");
                    break;

        case LV_EVENT_LONG_PRESSED:
            mqttClient.publish(SECRETS::MqttTopicButtonsPressed, (s + "_long_pressed").c_str());
        break;

        case LV_EVENT_LONG_PRESSED_REPEAT:
            mqttClient.publish(SECRETS::MqttTopicButtonsPressed, (s + "_long_pressed_repeat").c_str());
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