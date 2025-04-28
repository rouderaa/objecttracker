#include <Arduino.h>
#include "esp_camera.h"

#define CAMERA_MODEL_AI_THINKER // Has PSRAM
#include "camera_pins.h"

extern void setupLedFlash(int pin);
extern void startCameraServer();

// Function that does the actual detecting of the red object. Returns true if detection.
extern bool detect(
    uint8_t *buf, int buf_len,
    int red_level, int green_level, int blue_level,
    int &left, int &top, int &right, int &bottom);

static void setup_camera()
{
    Serial.println("Initializing camera");

    camera_config_t config;
    config.ledc_channel = LEDC_CHANNEL_0;
    config.ledc_timer = LEDC_TIMER_0;
    config.pin_d0 = Y2_GPIO_NUM;
    config.pin_d1 = Y3_GPIO_NUM;
    config.pin_d2 = Y4_GPIO_NUM;
    config.pin_d3 = Y5_GPIO_NUM;
    config.pin_d4 = Y6_GPIO_NUM;
    config.pin_d5 = Y7_GPIO_NUM;
    config.pin_d6 = Y8_GPIO_NUM;
    config.pin_d7 = Y9_GPIO_NUM;
    config.pin_xclk = XCLK_GPIO_NUM;
    config.pin_pclk = PCLK_GPIO_NUM;
    config.pin_vsync = VSYNC_GPIO_NUM;
    config.pin_href = HREF_GPIO_NUM;
    config.pin_sccb_sda = SIOD_GPIO_NUM;
    config.pin_sccb_scl = SIOC_GPIO_NUM;
    config.pin_pwdn = PWDN_GPIO_NUM;
    config.pin_reset = RESET_GPIO_NUM;
    config.xclk_freq_hz = 20000000;

    //    config.frame_size = FRAMESIZE_UXGA;
    //    config.frame_size = FRAMESIZE_HQVGA;
    config.frame_size = FRAMESIZE_QQVGA;
    config.pixel_format = PIXFORMAT_JPEG;
    //    config.pixel_format = PIXFORMAT_YUV422;
    // config.pixel_format = PIXFORMAT_RGB565;
    config.grab_mode = CAMERA_GRAB_WHEN_EMPTY;
    // config.grab_mode = CAMERA_GRAB_LATEST;

    // config.fb_location = CAMERA_FB_IN_PSRAM;
    config.fb_location = CAMERA_FB_IN_DRAM;

    config.jpeg_quality = 63; // low jpeg quality
    config.fb_count = 1;
    config.frame_size = FRAMESIZE_QQVGA;
    config.jpeg_quality = 10;
    config.fb_count = 2;
    config.grab_mode = CAMERA_GRAB_LATEST;
    config.fb_location = CAMERA_FB_IN_DRAM;

    // camera init
    esp_err_t err = esp_camera_init(&config);
    if (err != ESP_OK)
    {
        Serial.printf("Camera init failed with error 0x%x", err);
        return;
    }

    sensor_t *s = esp_camera_sensor_get();

    // Do whitebalacing in order to get valid colors
    s->set_whitebal(s, true);
    delay(3000);               // Give whitebalance a chance to work
    s->set_whitebal(s, false); // Disable awb for better color detection

    s->set_saturation(s, 2); // Saturation to max for better colors
    s->set_quality(s, 63);   // Low quality, no need for a lot of pixels
    Serial.printf("Camera initialized\r\n");
}

// Main esp32cam initialisation with webserver task for camera
// configuration.
void esp32cam_setup()
{
    Serial.println("esp32cam_setup");
    setup_camera();

    // For LED flash pwm
    ledcSetup(0, 5000, 8);
    ledcAttachPin(4, 0);

    startCameraServer();
}
