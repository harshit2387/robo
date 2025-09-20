#include "esp_camera.h"
#include <WiFi.h>
#include <WebServer.h>

const char* ssid = "AirFiber-5G";
const char* password = "12345678@";

WebServer server(80);

WiFiClient streamClient;
bool streaming = false;

void handle_root() {
  server.send(200, "text/html",
    "<html><body><h1>ESP32-CAM Stream</h1><img src=\"/stream\"/></body></html>");
}

void handle_stream() {
  streamClient = server.client();
  streaming = true;

  String response = "HTTP/1.1 200 OK\r\n";
  response += "Content-Type: multipart/x-mixed-replace; boundary=frame\r\n\r\n";
  streamClient.print(response);
}

void setup() {
  Serial.begin(115200);

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
  config.jpeg_quality = 10;
  config.fb_count = 2;

  if (esp_camera_init(&config) != ESP_OK) {
    Serial.println("Camera init failed");
    return;
  }

  sensor_t *s = esp_camera_sensor_get();
  s->set_vflip(s, 1);
  s->set_hmirror(s, 0);

  WiFi.begin(ssid, password);
  Serial.print("Connecting");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500); Serial.print(".");
  }
  Serial.print("\nStream ready at: http://");
  Serial.println(WiFi.localIP());

  server.on("/", handle_root);
  server.on("/stream", HTTP_GET, handle_stream);
  server.begin();
}

void loop() {
  server.handleClient();

  if (streaming && streamClient.connected()) {
    camera_fb_t *fb = esp_camera_fb_get();
    if (fb) {
      streamClient.printf("--frame\r\nContent-Type: image/jpeg\r\nContent-Length: %u\r\n\r\n", fb->len);
      streamClient.write(fb->buf, fb->len);
      streamClient.print("\r\n");
      esp_camera_fb_return(fb);
    }
    delay(50); // control FPS
  } else {
    streaming = false;
  }
}