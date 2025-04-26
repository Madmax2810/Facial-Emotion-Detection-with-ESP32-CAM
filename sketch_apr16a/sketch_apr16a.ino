#include "esp_camera.h"
#include <WiFi.h>
#include <HTTPClient.h>

// Replace with your actual WiFi credentials
const char* ssid = "manas's S21 FE";
const char* password = "iusu5050";

// Replace with your server IP and port
const char* server_ip = "http://192.168.112.253";  // Server (laptop) IP
const int server_port = 8000;           // Server port
String server_url = "/upload"; // Flask server route for receiving images

WiFiClient client;
HTTPClient http;

void setup() {
  Serial.begin(115200);
  Serial.setDebugOutput(true);
  Serial.println("[LOG] Starting ESP32-CAM...");

  // Configure Camera
  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer   = LEDC_TIMER_0;
  config.pin_d0       = 5;
  config.pin_d1       = 18;
  config.pin_d2       = 19;
  config.pin_d3       = 21;
  config.pin_d4       = 36;
  config.pin_d5       = 39;
  config.pin_d6       = 34;
  config.pin_d7       = 35;
  config.pin_xclk     = 0;
  config.pin_pclk     = 22;
  config.pin_vsync    = 25;
  config.pin_href     = 23;
  config.pin_sccb_sda = 26;
  config.pin_sccb_scl = 27;
  config.pin_pwdn     = 32;
  config.pin_reset    = -1;
  config.xclk_freq_hz = 20000000;
  config.pixel_format = PIXFORMAT_JPEG;

  if (psramFound()) {
    config.frame_size = FRAMESIZE_VGA; // 640x480
    config.jpeg_quality = 10;
    config.fb_count = 2;
    Serial.println("[LOG] PSRAM found. Using high-quality settings.");
  } else {
    config.frame_size = FRAMESIZE_QVGA;
    config.jpeg_quality = 12;
    config.fb_count = 1;
    Serial.println("[LOG] PSRAM not found. Using fallback settings.");
  }

  // Initialize camera
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("[ERROR] Camera init failed with error 0x%x\n", err);
    return;
  }
  Serial.println("[LOG] Camera init successful.");

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  Serial.print("[LOG] Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\n[LOG] WiFi connected. IP: " + WiFi.localIP().toString());
}

void loop() {
  // Capture image
  camera_fb_t * fb = esp_camera_fb_get();
  if (!fb) {
      Serial.println("Camera capture failed");
      return;
  }

  Serial.printf("[LOG] Captured image: %d bytes\n", fb->len);

  // Set up HTTP request
  if (http.begin("http://192.168.112.253:8000/upload")) {
    http.addHeader("Content-Type", "application/octet-stream"); // Set content type for raw data
    
    // Send image data as POST request
    int httpResponseCode = http.POST(fb->buf, fb->len);
    
    if (httpResponseCode > 0) {
      Serial.printf("[LOG] Image sent, Response code: %d\n", httpResponseCode);
    } else {
      Serial.printf("[ERROR] HTTP POST failed, Response code: %d\n", httpResponseCode);
    }

    // End HTTP request
    http.end();
  } else {
    Serial.println("[ERROR] Unable to connect to the server.");
  }

  esp_camera_fb_return(fb);

  delay(50); // Adjust as needed for bandwidth
}
