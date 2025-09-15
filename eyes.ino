//***********************************************************************************************
//  Example: RoboEyes + Touch Sensor Integration
//
//  Hardware: 
//   - Arduino Nano R3 (or compatible)
//   - I2C OLED display (SSD1306 / SSD1309, 128x64)
//   - Touch sensor connected to pin 2
//
//  Library: FluxGarage RoboEyes + Adafruit SSD1306
//
//***********************************************************************************************

#include <Adafruit_SSD1306.h>
#include <FluxGarage_RoboEyes.h>

#define SCREEN_WIDTH 128  // OLED display width
#define SCREEN_HEIGHT 64  // OLED display height
#define OLED_RESET    -1  // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// RoboEyes instance
RoboEyes<Adafruit_SSD1306> roboEyes(display);

// Touch sensor pin
#define TOUCH_PIN 2

void setup() {
  Serial.begin(9600);
  pinMode(TOUCH_PIN, INPUT);

  // Initialize OLED
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3C typical
    Serial.println(F("SSD1306 allocation failed"));
    for (;;);
  }

  // Initialize RoboEyes
  roboEyes.begin(SCREEN_WIDTH, SCREEN_HEIGHT, 100);

  // Automated behaviours
  roboEyes.setAutoblinker(ON, 3, 2);
  roboEyes.setIdleMode(ON, 2, 2);

  // Default mood
  roboEyes.setMood(DEFAULT);
}

void loop() {
  // Read touch sensor
  int touchState = digitalRead(TOUCH_PIN);

  if (touchState == HIGH) {
    // If touched → joy mode
    roboEyes.setMood(HAPPY);
    Serial.println("Touch detected → HAPPY eyes");
  } else {
    // No touch → normal
    roboEyes.setMood(DEFAULT);
  }

  // Update eye animations
  roboEyes.update();
}
