/*
 * SERVO_CONF.cpp
 * Single servo control implementation for rotating platform.
 * Uses ESP32 LEDC (hardware PWM) for smooth servo control.
 */

#include "SERVO_CONF.h"

// LEDC configuration for servo
#define LEDC_TIMER          LEDC_TIMER_0
#define LEDC_CHANNEL        LEDC_CHANNEL_0
#define LEDC_MODE           LEDC_LOW_SPEED_MODE
#define LEDC_TIMER_BIT      LEDC_TIMER_13_BIT  // 13-bit resolution for precision

void ServoConf::begin(void) {
  // Configure LEDC timer
  ledcSetup(LEDC_CHANNEL, SERVO_FREQ, LEDC_TIMER_BIT);
  ledcAttachPin(SERVO_PIN, LEDC_CHANNEL);
  
  // Start at home position
  goHome();
}

void ServoConf::setPulseWidth(uint16_t pulseUs) {
  // Convert microseconds to LEDC duty cycle
  // duty = (pulseUs / period) * 2^bit_resolution
  // period = 1/freq = 20000us for 50Hz
  float duty = (float)pulseUs / 20000.0f * (float)(1 << LEDC_TIMER_BIT);
  ledcWrite(LEDC_CHANNEL, (int)duty);
}

void ServoConf::setAngle(float angle) {
  // Clamp angle
  if (angle < 0.0f) angle = 0.0f;
  if (angle > 180.0f) angle = 180.0f;
  
  // Convert angle to pulse width
  uint16_t pulseUs = SERVO_MIN_PULSE + (uint16_t)((angle / 180.0f) * SERVO_RANGE_US);
  setPulseWidth(pulseUs);
  currentAngle = angle;
}

void ServoConf::goHome(void) {
  setAngle(SERVO_HOME_ANGLE);
}

void ServoConf::dropBottle(void) {
  setAngle(SERVO_DROP_ANGLE);
}

float ServoConf::getCurrentAngle(void) {
  return currentAngle;
}
