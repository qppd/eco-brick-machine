/*
 * BOOM_GATE_CONF.h
 * Limit switch boom gate control for EcoBrickMachine.
 *
 * Controls a servo-operated boom barrier that stops bottles on the conveyor.
 * The limit switch at the end of the boom detects when a bottle arrives.
 *
 * Hardware:
 *   - Servo (SG90/MG90S): GPIO 25 (LEDC PWM)
 *   - Limit switch: GPIO 23 (active LOW with internal pull-up)
 *
 * Operation:
 *   - ARM_UP (0°): Bottles can pass freely
 *   - ARM_DOWN (90°): Blocks conveyor, limit switch triggers
 *   - Bottle hits boom → stopped at exact position under press
 *
 * Wiring:
 *   Servo:
 *     Signal → GPIO 25
 *     VCC   → 5V external (2A+)
 *     GND   → GND (common)
 *   
 *   Limit Switch:
 *     Out   → GPIO 23
 *     VCC   → 5V
 *     GND   → GND
 */

#ifndef BOOM_GATE_CONF_H
#define BOOM_GATE_CONF_H

#include <Arduino.h>

// --- Pin configuration ---
#define BOOM_SERVO_PIN     25
#define LIMIT_SWITCH_PIN   23

// --- Servo positions (degrees) ---
#define BOOM_ARM_UP_ANGLE    0   // Raised, bottles can pass
#define BOOM_ARM_DOWN_ANGLE  90  // Lowered, blocks conveyor

// --- Timing ---
#define BOOM_MOVE_DELAY_MS   500UL   // Wait time for servo movement
#define LIMIT_SWITCH_DEBOUNCE_MS  50UL  // Debounce time for switch

class BoomGateConf {
public:
  void begin(void);
  
  // Arm control
  void armUp(void);           // Raise boom, allow bottles to pass
  void armDown(void);         // Lower boom, block conveyor
  void toggle(void);          // Switch between up and down
  
  // Status
  bool isArmUp(void);
  bool isArmDown(void);
  bool isBottleDetected(void);  // Returns true if limit switch triggered
  
  // Manual test
  void testCycle(void);       // Up → Down → Up for testing

private:
  bool armUpState = true;
  bool lastSwitchState = HIGH;
  uint32_t lastSwitchDebounceMs = 0;
  
  void setServoAngle(float angle);
};

#endif // BOOM_GATE_CONF_H
