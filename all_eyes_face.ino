#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <FluxGarage_RoboEyes.h>

// OLED display settings
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// RoboEyes object
RoboEyes<Adafruit_SSD1306> eyes(display);

// Automatic mood switching
unsigned long moodTimer = 0;
byte currentMood = 0; // 0=Default, 1=Tired, 2=Angry, 3=Happy

void setup() {
  Serial.begin(9600);
  
  // Initialize display
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("SSD1306 allocation failed");
    for(;;);
  }

  display.clearDisplay();
  display.display();

  // Initialize RoboEyes
  eyes.begin(SCREEN_WIDTH, SCREEN_HEIGHT, 50); // 50 fps
  eyes.setCuriosity(true);   // outer eyes grow when moving left/right
  eyes.setIdleMode(true, 2, 2); // eyes move randomly (idle)
  eyes.setAutoblinker(true, 3, 2); // automatic blinking

  moodTimer = millis();
  eyes.setMood(currentMood); // start with default
}

void loop() {
  // Update RoboEyes animation
  eyes.update();

  // Automatic mood switching every 10 seconds
  if (millis() - moodTimer >= 10000) {
    currentMood = (currentMood + 1) % 4; // cycle through 0..3
    eyes.setMood(currentMood);
    moodTimer = millis();
  }
}
