----------------------------------------------------------------------------------------------------------------------------------------------------------
i used  Simple I2S Example (A2DS Sink) using default Pins
Here is the simplest example which just uses the proper default settings:

#include "AudioTools.h"
#include "BluetoothA2DPSink.h"

I2SStream i2s;
BluetoothA2DPSink a2dp_sink(i2s);

void setup() {
    a2dp_sink.start("MyMusic");
}

void loop() {
}
This creates a new Bluetooth device with the name “MyMusic” and the output will be sent to the following default I2S pins which need to be connected to an external DAC:
- I2S Audio: Commonly used pins for I2S:
- BCLK: GPIO27 or GPIO14
- LRCLK: GPIO25 or GPIO15
- DIN: GPIO22 or GPIO26
esp32 - WROOM - DA  with
Change Partition Scheme
Go to Tools → Partition Scheme and select:
OTA  i have selected (2MB APP/ 2MB SPIFFS)
and use lib
✅ Manual Installation of AudioTools
📦 Step-by-Step:
- Go to the GitHub repository:
👉 https://github.com/pschatzmann/arduino-audio-tools
- Click the green “Code” button → Download ZIP
- In Arduino IDE:
- Go to Sketch → Include Library → Add .ZIP Library
- Select the downloaded ZIP file
- It will now be available as #include <AudioTools.h> in your sketches.
-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
