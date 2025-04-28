#include <Arduino.h>
#include <WiFi.h>
#include "credentials.h"

extern void esp32cam_setup();
extern bool detect(
    uint8_t *buf, int buf_len,
    int red_level, int green_level, int blue_level,
    int &left, int &top, int &right, int &bottom);

void setup()
{
    // Using pin 2 on the esp32-cam for serial debug output
    Serial.begin(115200, SERIAL_8N1, -1, 2);
    //    Serial.begin(115200);

    Serial.println("Wifi setup");
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
    }
    Serial.println("\r\nWiFi connected successfully!");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());

    esp32cam_setup();
}

static uint8_t *buf;
static int buf_len;
static int red_level = 170;  // above red level
static int green_level = 60; // below green level
static int blue_level = 80;  // below blue level
// output variables
int left, top, right, bottom;

void loop()
{
    // Let camera take picture and look for a red object 
    // by using the red_level, green_level and blue_level
    if (detect(
            buf, buf_len,
            red_level, green_level, blue_level,
            left, top, right, bottom)
    ) {
        Serial.printf("Object: (%d,%d)-(%d,%d)\r\n", left, top, right, bottom);
    }

    delay(1000);
}
