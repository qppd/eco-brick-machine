/*
 * PCA9685_CONF.cpp
 * PCA9685 16-channel 12-bit PWM servo driver implementation.
 */

#include "PCA9685_CONF.h"

void PCA9685Conf::begin(void) {
  Wire.begin(16, 17);  // SDA=GPIO16, SCL=GPIO17
  delay(10);
  
  // Reset to default
  writeReg(PCA9685_MODE1, 0x00);
  delay(10);
  
  // Set frequency
  setPWMFreq((float)SERVO_FREQ);
  
  // Enable all LEDs (grouped mode)
  writeReg(PCA9685_LEDOUT_L, 0x10);
  writeReg(PCA9685_LEDOUT_R, 0x10);
  
  // Initialize servos to home position
  goHome();
}

void PCA9685Conf::setPWMFreq(float freq) {
  // Calculate prescaler value
  // Prescale = clock / (4096 * freq) - 1
  // ESP32 internal clock = 50MHz
  float prescale = (50000000.0f / (4096.0f * freq)) - 1.0f;
  uint8_t oldMode = readReg(PCA9685_MODE1);
  writeReg(PCA9685_MODE1, (uint8_t)(oldMode & 0x7F) | 0x10);  // Sleep
  writeReg(PCA9685_PRESCALE, (uint8_t)prescale);
  writeReg(PCA9685_MODE1, oldMode);
  delay(5);
}

void PCA9685Conf::setServoPulse(uint8_t channel, uint16_t pulseUs) {
  // Convert microseconds to 12-bit PWM value
  // At 50Hz, period = 20000us, so: value = (pulseUs / 20000) * 4096
  uint16_t pwmValue = (uint16_t)((float)pulseUs / 20000.0f * (float)SERVO_12BIT);
  // Clamp to valid range
  if (pwmValue > SERVO_12BIT) pwmValue = SERVO_12BIT;
  
  setPWM(channel, 0, pwmValue);
}

void PCA9685Conf::setServoAngle(uint8_t channel, float angleDeg) {
  // Clamp angle
  if (angleDeg < 0.0f) angleDeg = 0.0f;
  if (angleDeg > 180.0f) angleDeg = 180.0f;
  
  // Convert degrees to pulse width
  uint16_t pulseUs = SERVO_MIN_PULSE_US + (uint16_t)((angleDeg / 180.0f) * SERVO_RANGE_US);
  setServoPulse(channel, pulseUs);
}

void PCA9685Conf::goHome(void) {
  setServoAngle(CHASSIS_BASE, POS_HOME_BASE);
  setServoAngle(CHASSIS_SHOULDER, POS_HOME_SHOULDER);
  setServoAngle(CHASSIS_GRIPPER, POS_HOME_GRIPPER);
  delay(500);  // Wait for servos to reach position
}

void PCA9685Conf::pickBottle(void) {
  // Move to pick position
  setServoAngle(CHASSIS_BASE, POS_PICK_BASE);
  delay(300);
  setServoAngle(CHASSIS_SHOULDER, POS_PICK_SHOULDER);
  delay(300);
  gripperOpen();
  delay(200);
  // Close gripper to grab bottle
  gripperClose();
  delay(300);
  // Lift bottle
  setServoAngle(CHASSIS_SHOULDER, POS_HOME_SHOULDER);
  delay(400);
  delay(500);
}

void PCA9685Conf::placeBottle(void) {
  // Move to place position
  setServoAngle(CHASSIS_BASE, POS_PLACE_BASE);
  delay(300);
  setServoAngle(CHASSIS_SHOULDER, POS_PLACE_SHOULDER);
  delay(300);
  // Open gripper to release bottle
  gripperOpen();
  delay(300);
  // Return to home
  goHome();
}

void PCA9685Conf::gripperOpen(void) {
  setServoAngle(CHASSIS_GRIPPER, 70);  // Open position
  delay(200);
}

void PCA9685Conf::gripperClose(void) {
  setServoAngle(CHASSIS_GRIPPER, 110);  // Closed position
  delay(200);
}

void PCA9685Conf::scanI2C(void) {
  Serial.println(F("Scanning I2C bus..."));
  for (uint8_t addr = 1; addr < 127; addr++) {
    Wire.beginTransmission(addr);
    if (Wire.endTransmission() == 0) {
      Serial.printf("Found device at address: 0x%02X\n", addr);
    }
  }
}

void PCA9685Conf::writeReg(uint8_t reg, uint8_t val) {
  Wire.beginTransmission(PCA9685_ADDR);
  Wire.write(reg);
  Wire.write(val);
  Wire.endTransmission();
}

uint8_t PCA9685Conf::readReg(uint8_t reg) {
  Wire.beginTransmission(PCA9685_ADDR);
  Wire.write(reg);
  Wire.endTransmission(false);  // Repeat start
  Wire.requestFrom(PCA9685_ADDR, (uint8_t)1);
  return Wire.read();
}

void PCA9685Conf::setPWM(uint8_t channel, uint16_t on, uint16_t off) {
  // Calculate register addresses
  uint8_t regStart = PCA9685_PWM0_L + (channel * PCA9685_STEP);
  
  // Write ON time (low byte, high byte)
  Wire.beginTransmission(PCA9685_ADDR);
  Wire.write(regStart);
  Wire.write(on & 0xFF);
  Wire.write((on >> 8) & 0xFF);
  // Write OFF time (low byte, high byte)
  Wire.write(off & 0xFF);
  Wire.write((off >> 8) & 0xFF);
  Wire.endTransmission();
}
