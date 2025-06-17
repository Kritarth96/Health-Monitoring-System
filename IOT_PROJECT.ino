#include <Wire.h>
#include "MAX30105.h"
#include "spo2_algorithm.h"
#include <WiFi.h>
#include <HTTPClient.h>

// MAX30105 setup
MAX30105 particleSensor;

uint32_t irBuffer[100];
uint32_t redBuffer[100];
int32_t bufferLength = 100;
int32_t spo2;
int8_t validSPO2;
int32_t heartRate;
int8_t validHeartRate;
extern bool fallDetected;

const int SDA_PIN = 21;
const int SCL_PIN = 22;
const int pulseLED = 4;
const int readLED = 2;

// Wi-Fi credentials
const char* ssid = "Nothing Phone (2a)_3600";
const char* password = "NothingPhone2A";

// Server endpoint
const String serverUrl = "http://192.168.59.13/send_data";
 // Full URL with HTTP and port

void setup() {
  Serial.begin(115200);
  delay(1000);

  // Initialize I2C and LED pins
  Wire.begin(SDA_PIN, SCL_PIN);
  pinMode(pulseLED, OUTPUT);
  pinMode(readLED, OUTPUT);

  if (!particleSensor.begin(Wire, I2C_SPEED_FAST)) {
    Serial.println(F("MAX30105 not found. Check wiring."));
    while (1);
  }

  Serial.println(F("Attach MAX30102 to finger. Press any key to start."));
  while (Serial.available() == 0);
  Serial.read();

  // Sensor setup
  byte ledBrightness = 60;
  byte sampleAverage = 4;
  byte ledMode = 2;
  byte sampleRate = 100;
  int pulseWidth = 411;
  int adcRange = 4096;
  particleSensor.setup(ledBrightness, sampleAverage, ledMode, sampleRate, pulseWidth, adcRange);

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");
}

void loop() {
  checkFall();
  // Step 1: Initial buffer fill (100 samples)
  for (byte i = 0; i < bufferLength; i++) {
    while (!particleSensor.available()) particleSensor.check();

    redBuffer[i] = particleSensor.getRed();
    irBuffer[i] = particleSensor.getIR();
    particleSensor.nextSample();
    delay(10); // 100 Hz = 10 ms/sample
  }

  maxim_heart_rate_and_oxygen_saturation(irBuffer, bufferLength, redBuffer, &spo2, &validSPO2, &heartRate, &validHeartRate);
  Serial.println("----- Initial HR/SpO2 Calculated -----");

  // Step 2: Continuous processing and sending data
  while (1) {
    // Shift the buffer
    for (byte i = 25; i < 100; i++) {
      redBuffer[i - 25] = redBuffer[i];
      irBuffer[i - 25] = irBuffer[i];
    }

    // Add new data to buffer
    for (byte i = 75; i < 100; i++) {
      while (!particleSensor.available()) particleSensor.check();

      redBuffer[i] = particleSensor.getRed();
      irBuffer[i] = particleSensor.getIR();
      particleSensor.nextSample();
      delay(10); // Maintain 100 Hz
    }

    // Calculate heart rate and SPO2
    maxim_heart_rate_and_oxygen_saturation(irBuffer, bufferLength, redBuffer, &spo2, &validSPO2, &heartRate, &validHeartRate);

    // --- Internal temperature ---
    float temperatureF = particleSensor.readTemperatureF();
    float c = temperatureF + 2.9;
    Serial.print("Body Temerature : ");
    Serial.print(c);
    Serial.println(" °F");

    if (c >= 99.0){
      Serial.println("Fever Detected!");

    }
    else{
      Serial.println("no fever");
    }

    

    // --- Create JSON payload ---
    String jsonPayload = "{";
    jsonPayload += "\"spo2\": " + String(spo2) + ",";
    jsonPayload += "\"heart_rate\": " + String(heartRate) + ",";
    jsonPayload += "\"temperature\": " + String(c) + ",";
    jsonPayload += "\"fall\": " + String(fallDetected ? "true" : "false") + ",";
    jsonPayload += "\"timestamp\": \"" + getTimestamp() + "\"";
    jsonPayload += "}";

    // Send data to Flask server
    sendDataToServer(jsonPayload);

    delay(1000);  // Delay between readings
  }
}

String getTimestamp() {
  // Get current timestamp as ISO 8601 format
  time_t now = time(NULL);
  struct tm* timeinfo = localtime(&now);
  char buffer[80];
  strftime(buffer, sizeof(buffer), "%Y-%m-%dT%H:%M:%S", timeinfo);
  return String(buffer);
}

void sendDataToServer(String payload) {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin(serverUrl);  // Specify the URL of your Flask server
    http.addHeader("Content-Type", "application/json");

    int httpResponseCode = http.POST(payload);

    if (httpResponseCode > 0) {
      Serial.print("Data sent, HTTP Response code: ");
      Serial.println(httpResponseCode);
    } else {
      Serial.print("Error in sending data. HTTP Response code: ");
      Serial.println(httpResponseCode);
    }

    http.end();  // Close connection
  } else {
    Serial.println("WiFi Disconnected, unable to send data");
  }
}
