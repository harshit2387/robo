#include <Adafruit_SSD1306.h>
#include <FluxGarage_RoboEyes.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

RoboEyes<Adafruit_SSD1306> roboEyes(display);

String command = "";
bool eyesOpen = false;
bool sleeping = false;

unsigned long lastFaceTime = 0;  // last time a face was detected
const unsigned long sleepDelay = 10000; // 10 seconds

void setup() {
  Serial.begin(9600);

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;);
  }

  roboEyes.begin(SCREEN_WIDTH, SCREEN_HEIGHT, 100);
  roboEyes.setAutoblinker(OFF);
  roboEyes.setIdleMode(OFF);

  Serial.println("RoboEyes ready...");
}

void loop() {
  if (eyesOpen && !sleeping) {
    roboEyes.update();  // only update when eyes are open & awake
  }

  while (Serial.available()) {
    char c = Serial.read();
    if (c == '\n') {
      command.trim();

      if (command == "OPEN") {
        eyesOpen = true;
        sleeping = false;
        roboEyes.setMood(DEFAULT);
        display.clearDisplay();
        Serial.println("Eyes: OPEN");
        lastFaceTime = millis(); // reset timer
      }
      else if (command == "CLOSE") {
        eyesOpen = false;
        sleeping = false;
        display.clearDisplay();
        display.display();
        Serial.println("Eyes: CLOSED");
      }
      else if (eyesOpen) {
        if (command == "HAPPY") {
          roboEyes.setMood(HAPPY);
          roboEyes.setPosition(DEFAULT);
          Serial.println("Mood: HAPPY");
          lastFaceTime = millis(); // reset timer
        }
        else if (command == "SAD") {
          roboEyes.setMood(TIRED);  // TIRED looks sad
          roboEyes.setPosition(S);  // look down
          Serial.println("Mood: SAD");
          lastFaceTime = millis(); // reset timer
        }
      }
      command = "";
    } else {
      command += c;
    }
  }

  // --- Sleep logic ---
  if (eyesOpen && !sleeping && (millis() - lastFaceTime > sleepDelay)) {
    goToSleep();
  }
}

void goToSleep() {
  Serial.println("Eyes: SLEEPING...");

  // Animate eyelids closing (reduce height gradually)
  for (int h = 36; h >= 0; h -= 4) {
    roboEyes.setHeight(h, h);
    roboEyes.update();
    delay(100);
  }

  display.clearDisplay();
  display.display();

  sleeping = true;
  eyesOpen = false;
  Serial.println("Eyes are asleep.");
}

