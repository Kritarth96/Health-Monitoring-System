#define ECG_PIN 36
#define LO_PLUS 34
#define LO_MINUS 35

unsigned long lastECGSampleTime = 0;
const int ecgSampleRateHz = 360;
const int ecgSampleInterval = 1000000 / ecgSampleRateHz;

void readECG() {
  unsigned long now = micros();

  if (now - lastECGSampleTime >= ecgSampleInterval) {
    lastECGSampleTime = now;

    if (digitalRead(LO_PLUS) == 1 || digitalRead(LO_MINUS) == 1) {
      Serial.println("ECG:512");
    } else {
      int ecgRaw = analogRead(ECG_PIN);
      int ecgScaled = map(ecgRaw, 0, 4095, 0, 1023);
      Serial.print("ECG:");
      Serial.println(ecgScaled);
    }
  }
}
