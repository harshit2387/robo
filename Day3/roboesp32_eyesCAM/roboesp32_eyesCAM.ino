#include <Adafruit_SSD1306.h>
#include <FluxGarage_RoboEyes.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1

#define DEFAULT   0
#define HAPPY     1
#define SAD       2
#define ANGRY     3
#define SURPRISED 4

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
RoboEyes<Adafruit_SSD1306> roboEyes(display);

String command = "";

void setup() {
  Serial.begin(9600);
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);

  roboEyes.begin(SCREEN_WIDTH, SCREEN_HEIGHT, 100);
  roboEyes.setAutoblinker(ON, 3, 2);
  roboEyes.setIdleMode(ON, 2, 2);
  roboEyes.setMood(DEFAULT);
}

void loop() {
  while (Serial.available()) {
    char c = Serial.read();
    if (c == '\n') {
      processCommand(command);
      command = "";
    } else {
      command += c;
    }
  }
  roboEyes.update();
}

void processCommand(String cmd) {
  cmd.trim();
  if (cmd == "happy") roboEyes.setMood(HAPPY);
  else if (cmd == "sad") roboEyes.setMood(SAD);
  else if (cmd == "angry") roboEyes.setMood(ANGRY);
  else if (cmd == "surprised") roboEyes.setMood(SURPRISED);
  else roboEyes.setMood(DEFAULT);
}
