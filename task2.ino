#include <Wire.h>
#include "AudioTools.h"
#include "BluetoothA2DPSink.h"
#include <Adafruit_SSD1306.h>
#include <FluxGarage_RoboEyes.h>

// OLED Display Settings
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
RoboEyes<Adafruit_SSD1306> roboEyes(display);

// Bluetooth Audio Settings
I2SStream i2s;
BluetoothA2DPSink a2dp_sink(i2s);

// Event Timing
unsigned long eventTimer;
bool event1wasPlayed = false;
bool event2wasPlayed = false;
bool event3wasPlayed = false;
bool animationStarted = false;

void setup() {
  Serial.begin(115200);

  // Custom I2C pin mapping: SDA = GPIO21, SCL = GPIO19
  Wire.begin(21, 19);

  // Initialize OLED
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    while (true); // halt
  }

  // Initialize RoboEyes
  roboEyes.begin(SCREEN_WIDTH, SCREEN_HEIGHT, 100);
  roboEyes.setPosition(DEFAULT);
  roboEyes.close();

  // Start Bluetooth A2DP sink
  a2dp_sink.start("MyMusic");

  // Start animation timer
  eventTimer = millis();
}

void loop() {
  // Check Bluetooth connection
  if (a2dp_sink.is_connected()) {
    roboEyes.update();

    // Start animation only once when connected
    if (!animationStarted) {
      animationStarted = true;
      eventTimer = millis(); // reset timer
      event1wasPlayed = false;
      event2wasPlayed = false;
      event3wasPlayed = false;
    }

    // Animation Sequence
    if (millis() >= eventTimer + 2000 && !event1wasPlayed) {
      event1wasPlayed = true;
      roboEyes.open();
    }

    if (millis() >= eventTimer + 4000 && !event2wasPlayed) {
      event2wasPlayed = true;
      roboEyes.setMood(HAPPY);
      roboEyes.anim_laugh();
    }

    if (millis() >= eventTimer + 6000 && !event3wasPlayed) {
      event3wasPlayed = true;
      roboEyes.setMood(TIRED);
    }

    if (millis() >= eventTimer + 8000) {
      roboEyes.close();
      roboEyes.setMood(DEFAULT);
      eventTimer = millis();
      event1wasPlayed = false;
      event2wasPlayed = false;
      event3wasPlayed = false;
    }

  } else {
    // If Bluetooth is disconnected, reset animation
    animationStarted = false;
    roboEyes.close();
    roboEyes.setMood(DEFAULT);
  }
}
