#include "esp_camera.h"

#define PIXEL_PIN 5
#define PIXEL_COUNT 1
// Timer interval (in milliseconds)
const long interval = 5000;

struct Color {
  const char* name;
  uint8_t r;
  uint8_t g;
  uint8_t b;
};

// List of colors to match against
Color colors[] = {
  {"red", 255, 0, 0},
  {"green", 0, 255, 0},
  {"blue", 0, 0, 255},
  {"yellow", 255, 255, 0},
  {"purple", 128, 0, 128},
  {"orange", 255, 165, 0},
  {"pink", 255, 192, 203},
  {"brown", 165, 42, 42},
  {"gray", 128, 128, 128}
};

// Function to take a picture and get the average RGB value of a 50x50 square in the center
void captureAndSend() {
  pinMode(4, OUTPUT);
  //digitalWrite(4, HIGH);
  // Take a picture
  camera_fb_t * fb = NULL;
  fb = esp_camera_fb_get();
  if(!fb) {
    Serial.println("Camera capture failed");
    return;
  }
  delay(100);
  digitalWrite(4, LOW);

  // Get the RGB values of the pixels in the center square
  size_t fbWidth = fb->width;
  size_t fbHeight = fb->height;
  uint8_t *fbBuf = fb->buf;
  int xStart = fbWidth / 2 - 25;
  int yStart = fbHeight / 2 - 25;
  uint32_t red = 0;
  uint32_t green = 0;
  uint32_t blue = 0;
  for (int y = yStart; y < yStart + 50; y++) {
    for (int x = xStart; x < xStart + 50; x++) {
      int pixelIndex = (y * fbWidth + x) * 3;
      red += fbBuf[pixelIndex];
      green += fbBuf[pixelIndex + 1];
      blue += fbBuf[pixelIndex + 2];
    }
  }
  uint32_t pixelCount = 50 * 50;
  uint8_t avgRed = red / pixelCount;
  uint8_t avgGreen = green / pixelCount;
  uint8_t avgBlue = blue / pixelCount;

  // Find the closest matching color
  uint32_t minDistance = UINT32_MAX;
  const char* closestColor = "";
  for (int i = 0; i < sizeof(colors) / sizeof(colors[0]); i++) {
    uint8_t dr = abs(colors[i].r - avgRed);
    uint8_t dg = abs(colors[i].g - avgGreen);
    uint8_t db = abs(colors[i].b - avgBlue);
    uint32_t distance = dr * dr + dg * dg + db * db;
    if (distance < minDistance) {
      minDistance = distance;
      closestColor = colors[i].name;
    }
  }

  // Print the name of the closest matching color to the serial monitor
  Serial.println(closestColor);
  Serial.println(avgRed);
  Serial.println(avgGreen);
  Serial.println(avgBlue);
  // Free the memory used by the frame buffer
  esp_camera_fb_return(fb);
  fb = NULL;
}

void setup() {
  Serial.begin(115200);

  // Initialize the camera
  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = 5;
  config.pin_d1 = 18;
  config.pin_d2 = 19;
  config.pin_d3 = 21;
  config.pin_d4 = 36;
  config.pin_d5 = 39;
  config.pin_d6 = 34;
  config.pin_d7 = 35;
  config.pin_xclk = 0;
  config.pin_pclk = 22;
  config.pin_vsync = 25;
  config.pin_href = 23;
  config.pin_sscb_sda = 26;
  config.pin_sscb_scl = 27;
  config.pin_pwdn = 32;
  config.pin_reset = -1;
  config.xclk_freq_hz = 20000000;
  config.pixel_format = PIXFORMAT_RGB888;
  config.frame_size = FRAMESIZE_QQVGA;
  config.jpeg_quality = 12;
  config.fb_count = 1;
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Camera init failed with error 0x%x", err);
    return;
  }
}

void loop() {
  delay(1000);
    // Take a picture and get the RGB value of the center pixel
  captureAndSend();
}
