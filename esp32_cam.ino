#include "esp_camera.h"
#include <WiFi.h>

// === BOARD CONFIG: AI-Thinker ESP32-CAM ===
#define PWDN_GPIO_NUM     32
#define RESET_GPIO_NUM    -1
#define XCLK_GPIO_NUM      0
#define SIOD_GPIO_NUM     26
#define SIOC_GPIO_NUM     27
#define Y9_GPIO_NUM       35
#define Y8_GPIO_NUM       34
#define Y7_GPIO_NUM       39
#define Y6_GPIO_NUM       36
#define Y5_GPIO_NUM       21
#define Y4_GPIO_NUM       19
#define Y3_GPIO_NUM       18
#define Y2_GPIO_NUM       5
#define VSYNC_GPIO_NUM    25
#define HREF_GPIO_NUM     23
#define PCLK_GPIO_NUM     22

// === WIFI ===
const char* ssid = "AirFiber-5G";
const char* password = "12345678@";

// HTTP server
#include "esp_http_server.h"
httpd_handle_t camera_httpd = NULL;

// --- FACE TRACKING SIMULATION ---
bool facePresent = false;
unsigned long lastFaceTime = 0;
const unsigned long closeDelay = 2000; // ms

// --- STREAM HANDLER ---
esp_err_t stream_handler(httpd_req_t *req){
  camera_fb_t * fb = NULL;
  esp_err_t res = ESP_OK;
  char part_buf[64];
  
  static const char* _STREAM_CONTENT_TYPE = "multipart/x-mixed-replace;boundary=frame";
  static const char* _STREAM_BOUNDARY = "\r\n--frame\r\n";
  static const char* _STREAM_PART = "Content-Type: image/jpeg\r\nContent-Length: %u\r\n\r\n";

  res = httpd_resp_set_type(req, _STREAM_CONTENT_TYPE);
  if(res != ESP_OK) return res;

  while(true){
    fb = esp_camera_fb_get();
    if(!fb){
      Serial.println("Camera capture failed");
      res = ESP_FAIL;
      break;
    }

    if(httpd_resp_send_chunk(req, _STREAM_BOUNDARY, strlen(_STREAM_BOUNDARY)) != ESP_OK){
      esp_camera_fb_return(fb); res = ESP_FAIL; break;
    }

    int hlen = sprintf(part_buf, _STREAM_PART, (int)fb->len);
    if(httpd_resp_send_chunk(req, part_buf, hlen) != ESP_OK){
      esp_camera_fb_return(fb); res = ESP_FAIL; break;
    }

    if(httpd_resp_send_chunk(req, (const char*)fb->buf, fb->len) != ESP_OK){
      esp_camera_fb_return(fb); res = ESP_FAIL; break;
    }

    esp_camera_fb_return(fb);

    // --- SIMPLE FACE SIGNAL SIMULATION ---
    if(!facePresent){
      Serial.println("OPEN"); // Signal RoboEyes
      facePresent = true;
      lastFaceTime = millis();
    } else {
      if(millis() - lastFaceTime > closeDelay){
        Serial.println("CLOSE"); // Signal RoboEyes
        facePresent = false;
      }
    }

    vTaskDelay(10 / portTICK_PERIOD_MS);
  }
  return res;
}

// --- START CAMERA SERVER ---
void startCameraServer(){
  httpd_config_t config = HTTPD_DEFAULT_CONFIG();
  config.server_port = 80;
  if(httpd_start(&camera_httpd, &config) == ESP_OK){
    httpd_uri_t stream_uri = {
      .uri = "/stream",
      .method = HTTP_GET,
      .handler = stream_handler,
      .user_ctx = NULL
    };
    httpd_register_uri_handler(camera_httpd, &stream_uri);
    Serial.println("Stream ready at http://<ESP32-IP>/stream");
  }
}

// --- SETUP ---
void setup() {
  Serial.begin(115200);
  Serial.println("ESP32-CAM starting...");

  // Camera config
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
  config.pixel_format = PIXFORMAT_JPEG;

  if(psramFound()){
    config.frame_size = FRAMESIZE_VGA; // Small enough to avoid stack issues
    config.jpeg_quality = 10;
    config.fb_count = 1;
    config.fb_location = CAMERA_FB_IN_PSRAM;
    config.grab_mode = CAMERA_GRAB_LATEST;
  } else {
    config.frame_size = FRAMESIZE_QVGA;
    config.jpeg_quality = 12;
    config.fb_count = 1;
    config.fb_location = CAMERA_FB_IN_DRAM;
    config.grab_mode = CAMERA_GRAB_WHEN_EMPTY;
  }

  if(esp_camera_init(&config) != ESP_OK){
    Serial.println("Camera init failed");
    return;
  }

  // Connect WiFi
  WiFi.begin(ssid, password);
  WiFi.setSleep(false);
  Serial.print("Connecting WiFi");
  while(WiFi.status() != WL_CONNECTED){
    delay(500); Serial.print(".");
  }
  Serial.println("\nWiFi connected: ");
  Serial.println(WiFi.localIP());

  // Start streaming server
  startCameraServer();
}

// --- LOOP ---
void loop() {
  delay(10000); // All work handled in stream task
}

