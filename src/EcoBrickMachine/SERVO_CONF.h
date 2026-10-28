/*
 * SERVO_CONF.h
 * Single servo control for rotating platform (bottle drop mechanism).
 *
 * The load cell platform rotates like a clock hand to drop filled bottles
 * onto the conveyor. One servo controls the rotation.
 *
 * Wiring (ESP32 38-pin):
 *   Servo signal → GPIO 25 (PWM capable)
 *   Servo VCC → 5V (external supply recommended)
 *   Servo GND → GND (common ground)
 *
 * Operation:
 *   HOME position (0°): Platform under shredder for filling
 *   DROP position (120°): Platform rotated to drop bottle on conveyor
 */

#ifndef SERVO_CONF_H
#define SERVO_CONF_H

#include <Arduino.h>

// --- Pin configuration ---
#define SERVO_PIN  25

// --- Servo timing ---
#define SERVO_FREQ       50        // Hz (standard servo)
#define SERVO_MIN_PULSE  500       // us at 0 degrees
#define SERVO_MAX_PULSE  2500      // us at 180 degrees
#define SERVO_RANGE_US   (SERVO_MAX_PULSE - SERVO_MIN_PULSE)

// --- Positions (degrees) ---
#define SERVO_HOME_ANGLE   0      // Under shredder for filling
#define SERVO_DROP_ANGLE   120    // Over conveyor to drop bottle

class ServoConf {
public:
  void begin(void);
  
  // Set servo angle (0-180 degrees)
  void setAngle(float angle);
  
  // Move to home position (filling position)
  void goHome(void);
  
  // Rotate to drop position
  void dropBottle(void);
  
  // Get current position
  float getCurrentAngle(void);

private:
  float currentAngle = SERVO_HOME_ANGLE;
  void setPulseWidth(uint16_t pulseUs);
};

#endif // SERVO_CONF_H
