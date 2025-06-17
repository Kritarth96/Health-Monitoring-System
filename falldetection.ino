#include <MPU6500_WE.h>
#include <Wire.h>
#define FALL_DETECTION_COOLDOWN 2000 
unsigned long lastFallTime = 0;
bool fallDetected=true;


#define I2C_ADDRESS 0x68
MPU6500_WE mpu6500(&Wire, I2C_ADDRESS);

// Define constants for fall detection thresholds
#define ACCEL_FALL_THRESHOLD 15.0  // Example value (m/s^2)
#define GYRO_THRESHOLD 300.0      // Example value (degrees per second)

void setupMPU6500() {
  Wire.begin();
  
  if (!mpu6500.init()) {
    Serial.println("MPU6500 not found. Check wiring!");
    while (1);  // Halt if the sensor is not found
  }

  mpu6500.setAccRange(MPU6500_ACC_RANGE_4G);  // Set accelerometer range
  mpu6500.setGyrRange(MPU6500_GYRO_RANGE_250);  // Set gyroscope range
  
  mpu6500.setSampleRateDivider(5);  // Optional: reduce data rate
}

void checkFall() {
  xyzFloat acc = mpu6500.getAccRawValues();
  xyzFloat gyr = mpu6500.getGyrRawValues();

  float ax = (acc.x / 8192.0) * 9.81;
  float ay = (acc.y / 8192.0) * 9.81;
  float az = (acc.z / 8192.0) * 9.81;

  float accMagnitude = sqrt(ax * ax + ay * ay + az * az);
  unsigned long currentTime = millis();

  if ((accMagnitude > ACCEL_FALL_THRESHOLD ||
       abs(gyr.x) > GYRO_THRESHOLD ||
       abs(gyr.y) > GYRO_THRESHOLD ||
       abs(gyr.z) > GYRO_THRESHOLD) &&
       (currentTime - lastFallTime > FALL_DETECTION_COOLDOWN)) {
    
    fallDetected = true;
    Serial.println("⚠ Fall Detected!");
    lastFallTime = currentTime;
  } else {
    fallDetected = false;
    Serial.println("No fall detected");
  }
}

