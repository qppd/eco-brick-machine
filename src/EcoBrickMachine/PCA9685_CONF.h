/*
 * PCA9685_CONF.h
 * PCA9685 16-channel 12-bit PWM servo driver interface for EcoBrickMachine.
 *
 * Controls the robotic arm for pick-and-place operation:
 *   Channel 0: Base rotation (swings arm between stations)
 *   Channel 1: Shoulder/elbow (picks up and lowers)
 *   Channel 2: Wrist/gripper (opens/closes to grab/release bottle)
 *
 * Wiring (ESP32 38-pin):
 *   PCA9685 VCC  -> 5V (external servo supply recommended)
 *   PCA9685 GND  -> GND (common ground with ESP32)
 *   PCA9685 SDA  -> GPIO 16 (I2C)
 *   PCA9685 SCL  -> GPIO 17 (I2C)
 *   PCA9685 OE   -> GND (enable outputs)
 *   PCA9685 ADDR -> GND (address 0x40)
 *
 * Servo specs (standard 5V RC servo):
 *   PWM frequency: 50 Hz (period = 20000 us)
 *   Pulse range: 500 us (0°) to 2500 us (180°)
 *   12-bit resolution: 4096 steps
 */

#ifndef PCA9685_CONF_H
#define PCA9685_CONF_H

#include <Arduino.h>
#include <Wire.h>

// --- PCA9685 I2C address ---
#define PCA9685_ADDR          0x40
#define PCA9685_MODE1         0x00
#define PCA9685_MODE2         0x01
#define PCA9685_PRESCALE      0xFE
#define PCA9685_LEDOUT_L      0x0A
#define PCA9685_LEDOUT_R      0x0B

// --- PWM registers for each channel (ON/L + ON/H + OFF/L + OFF/H) ---
#define PCA9685_PWM0_L        0x06
#define PCA9685_STEP          4

// --- Servo configuration ---
#define SERVO_FREQ            50        // Hz
#define SERVO_MIN_PULSE_US    500       // us at 0 degrees
#define SERVO_MAX_PULSE_US    2500      // us at 180 degrees
#define SERVO_RANGE_US        (SERVO_MAX_PULSE_US - SERVO_MIN_PULSE_US)
#define SERVO_12BIT           4096

// --- Channel assignments ---
#define CHASSIS_BASE          0    // Base rotation
#define CHASSIS_SHOULDER      1    // Shoulder/elbow
#define CHASSIS_GRIPPER       2    // Wrist/gripper

// --- Servo positions (degrees) ---
// Home position
#define POS_HOME_BASE         90
#define POS_HOME_SHOULDER     90
#define POS_HOME_GRIPPER      90

// Pick position (at shredding station)
#define POS_PICK_BASE         45
#define POS_PICK_SHOULDER     30
#define POS_PICK_GRIPPER      70    // open

// Place position (at pressing station)
#define POS_PLACE_BASE        135
#define POS_PLACE_SHOULDER    30
#define POS_PLACE_GRIPPER     70    // open

class PCA9685Conf {
public:
  void begin(void);
  void setPWMFreq(float freq);
  void setServoPulse(uint8_t channel, uint16_t pulseUs);
  void setServoAngle(uint8_t channel, float angleDeg);
  
  // Convenience: set all servos to home position
  void goHome(void);
  
  // Pick and place sequence
  void pickBottle(void);
  void placeBottle(void);
  
  // Gripper control
  void gripperOpen(void);
  void gripperClose(void);
  
  // Debug: print I2C address scan
  void scanI2C(void);

private:
  void writeReg(uint8_t reg, uint8_t val);
  uint8_t readReg(uint8_t reg);
  void setPWM(uint8_t channel, uint16_t on, uint16_t off);
};

#endif // PCA9685_CONF_H
