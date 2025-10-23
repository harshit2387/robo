#include <Arduino.h>
#include "AudioTools.h"
#include "BluetoothA2DPSink.h"

#define I2S_BCLK_SPK 14
#define I2S_LRCLK_SPK 15
#define I2S_DOUT_SPK 22

// I2S pins for microphone input
#define I2S_SCK_MIC 32
#define I2S_WS_MIC 25
#define I2S_SD_MIC 33

I2SStream i2sSpeaker;
I2SStream i2sMic;

BluetoothA2DPSink a2dp_sink(i2sSpeaker);

const int sampleCount = 16000;  // number of samples to record (approx 0.36s at 44.1kHz)
int16_t audioBuffer[sampleCount];  // buffer to hold recorded audio

void setup() {
  Serial.begin(115200);
  delay(1000);

  // Initialize I2S for mic input (RX mode)
  auto cfgMic = i2sMic.defaultConfig(RX_MODE);
  cfgMic.pin_bck = I2S_SCK_MIC;
  cfgMic.pin_ws = I2S_WS_MIC;
  cfgMic.pin_data = I2S_SD_MIC;
  cfgMic.sample_rate = 44100;
  cfgMic.bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT;
  i2sMic.begin(cfgMic);

  Serial.println("Recording audio from I2S mic...");
  
  // Read samples from mic into buffer
  int bytesToRead = sampleCount * sizeof(int16_t);
  int bytesRead = 0;
  while (bytesRead < bytesToRead) {
    int result = i2sMic.read((uint8_t*)audioBuffer + bytesRead, bytesToRead - bytesRead);
    if (result > 0) bytesRead += result;
  }

  i2sMic.end();  // stop mic reading

  Serial.println("Audio recording done, starting playback...");

  // Initialize I2S for speaker output (TX mode)
  auto cfgSpk = i2sSpeaker.defaultConfig();
  cfgSpk.pin_bck = I2S_BCLK_SPK;
  cfgSpk.pin_ws = I2S_LRCLK_SPK;
  cfgSpk.pin_data = I2S_DOUT_SPK;
  cfgSpk.sample_rate = 44100;
  cfgSpk.bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT;
  i2sSpeaker.begin(cfgSpk);
}

void loop() {
  // Playback the recorded audio buffer in a loop
  size_t totalBytes = sampleCount * sizeof(int16_t);
  
  size_t bytesWritten = 0;
  while (bytesWritten < totalBytes) {
    int written = i2sSpeaker.write((uint8_t*)audioBuffer + bytesWritten, totalBytes - bytesWritten);
    if (written > 0) bytesWritten += written;
  }

  delay(1000);  // pause before replay
}
