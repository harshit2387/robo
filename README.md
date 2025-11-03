----------------------------------------------------------------------------------------------------------------------------------------------------------
Simple I2S Example (A2DP Sink) on ESP32

This example demonstrates how to set up a Bluetooth A2DP Sink on an ESP32 using default I2S pins.

Example Code
#include "AudioTools.h"
#include "BluetoothA2DPSink.h"

I2SStream i2s;
BluetoothA2DPSink a2dp_sink(i2s);

void setup() {
    // Start Bluetooth device with the name "MyMusic"
    a2dp_sink.start("MyMusic");
}

void loop() {
    // Nothing required here
}


When uploaded, this creates a new Bluetooth device named “MyMusic”.
The audio stream is sent over I2S to an external DAC using the default ESP32 pins.

Default I2S Pin Mapping
- BCLK: GPIO14
- LRCLK: GPIO15
- DIN: GPIO22

Make sure to connect these pins correctly to your DAC for proper audio output.

ESP32 Board Settings

Board: ESP32-WROOM-DA

Partition Scheme:
Go to Tools → Partition Scheme and select:
OTA (2MB APP / 2MB SPIFFS)

Installing AudioTools Library

The project requires the AudioTools
 library.

Manual Installation

Visit the GitHub repository:
👉 pschatzmann/arduino-audio-tools

Click the green Code button → Download ZIP

In Arduino IDE:

Go to Sketch → Include Library → Add .ZIP Library

Select the downloaded ZIP file

The library is now available and can be included with:

#include <AudioTools.h>


✅ With this setup, your ESP32 acts as a Bluetooth audio receiver and streams sound via I2S to an external DAC.
------------------------------------------------------------------------------------------------------------------

![WhatsApp Image 2025-11-03 at 9 41 47 PM](https://github.com/user-attachments/assets/73c4bcdb-6645-4d5c-a7a5-b0257be5d673)

in this i have used inmp114 and oled and esp32 dev module and MAX98357A
Partition Scheme:
Go to Tools → Partition Scheme and select:
OTA (2MB APP / 2MB SPIFFS)
-----------------------------------------------------------------------------




