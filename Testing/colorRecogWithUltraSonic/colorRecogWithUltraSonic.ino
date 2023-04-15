#include "esp_camera.h"

#define TRIG 2
#define ECHO 3
// Timer interval (in milliseconds)
const long interval = 5000;

unsigned long previousMillis = 0;

// Function to take a picture and get the RGB value of the center pixel
void captureAndSend() {
  camera_fb_t * fb = NULL;
  esp_err_t res = ESP_OK;
  
  // Take a picture
  fb = esp_camera_fb_get();
  
  if (!fb) {
    Serial.println("Camera capture failed");
    return;
  }

  // Get the RGB value of the center pixel
  size_t fbWidth = fb->width;
  size_t fbHeight = fb->height;
  uint8_t *fbBuf = fb->buf;
  size_t pixelIndex = (fbWidth * (fbHeight / 2) + (fbWidth / 2)) * 3;
  uint8_t red = fbBuf[pixelIndex];
  uint8_t green = fbBuf[pixelIndex + 1];
  uint8_t blue = fbBuf[pixelIndex + 2];

  // Print the RGB value to the serial monitor
  Serial.print("R: ");
  Serial.print(red);
  Serial.print(" G: ");
  Serial.print(green);
  Serial.print(" B: ");
  Serial.println(blue);

  // Free the memory used by the frame buffer
  esp_camera_fb_return(fb);
}

void setup() {
  Serial.begin(115200);
  // Initialize the Ultrasonic_sensor
  pinMode(TRIG, OUTPUT);
  pinMode(ECHO, INPUT);
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
  config.pixel_format = PIXFORMAT_JPEG;
  config.frame_size = FRAMESIZE_QVGA;
  config.jpeg_quality = 12;
  config.fb_count = 1;
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Camera init failed with error 0x%x", err);
    return;
  }
}

void loop() {
  digitalWrite(TRIG, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG, HIGH);
  delayMicroseconds(5);
  digitalWrite(TRIG, LOW);

  float t = pulseIn(ECHO, HIGH);
  float distance = t*0.017015;
  Serial.print("Distance: ");
  Serial.println(distance);
  if (distance <= 50) {
    captureAndSend();
    delay(500);
  }
  delay(500);
}
