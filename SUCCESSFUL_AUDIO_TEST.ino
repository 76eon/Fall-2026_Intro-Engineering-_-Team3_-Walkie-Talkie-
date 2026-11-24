#include <Arduino.h>
#include <ESP_I2S.h>

I2SClass I2S;

void setup() {
  Serial.begin(115200);

  // Pins: SCK/BCK = 26, WS = 22, DOUT = -1 (unused), DIN/SD = 25
  I2S.setPins(26, 22, -1, 25);

  if (!I2S.begin(I2S_MODE_STD, 16000, I2S_DATA_BIT_WIDTH_32BIT, I2S_SLOT_MODE_STEREO)) {
    Serial.println("Failed to initialize I2S!");
    while (1);
  }
}

void loop() {
  int32_t audio_buffer[512]; // 256 stereo pairs (~16 ms of audio)
  size_t bytes_read = I2S.readBytes((char *)audio_buffer, sizeof(audio_buffer));
  int samples_read = bytes_read / sizeof(int32_t);

  if (samples_read < 2) return;

  int32_t min_val = 32767;
  int32_t max_val = -32768;

  // Read only the Left channel (even indices) and measure the peak-to-peak swing
  for (int i = 0; i < samples_read; i += 2) {
    int32_t sample = audio_buffer[i] >> 16;
    if (sample < min_val) min_val = sample;
    if (sample > max_val) max_val = sample;
  }

  int32_t volume = max_val - min_val;
  Serial.print("Mic:");
  Serial.println(volume);

  // Increase this number (e.g., to 60 or 100) if you want it even slower
  delay(80);
}