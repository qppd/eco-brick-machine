/*
 * BOOM_GATE_CONF.cpp
 * Limit switch boom gate control implementation.
 */

#include "BOOM_GATE_CONF.h"
#include "SERVO_CONF.h"  // For servo control

extern ServoConf servo;  // Global servo instance

void BoomGateConf::begin(void) {
  pinMode(BOOM_SERVO_PIN, OUTPUT);
  pinMode(LIMIT_SWITCH_PIN, INPUT_PULLUP);
  
  // Start with arm UP (bottles can pass)
  armUp();
}

void BoomGateConf::setServoAngle(float angle) {
  servo.setAngle(angle);
}

void BoomGateConf::armUp(void) {
  setServoAngle(BOOM_ARM_UP_ANGLE);
  armUpState = true;
  delay(BOOM_MOVE_DELAY_MS);
}

void BoomGateConf::armDown(void) {
  setServoAngle(BOOM_ARM_DOWN_ANGLE);
  armUpState = false;
  delay(BOOM_MOVE_DELAY_MS);
}

void BoomGateConf::toggle(void) {
  if (armUpState) {
    armDown();
  } else {
    armUp();
  }
}

bool BoomGateConf::isArmUp(void) {
  return armUpState;
}

bool BoomGateConf::isArmDown(void) {
  return !armUpState;
}

bool BoomGateConf::isBottleDetected(void) {
  bool current = digitalRead(LIMIT_SWITCH_PIN);
  
  // Debounce
  if (current != lastSwitchState) {
    lastSwitchDebounceMs = millis();
    lastSwitchState = current;
    return false;
  }
  
  if ((millis() - lastSwitchDebounceMs) >= LIMIT_SWITCH_DEBOUNCE_MS) {
    // Active LOW: switch pressed = LOW
    return (current == LOW);
  }
  
  return false;
}

void BoomGateConf::testCycle(void) {
  Serial.println(F("Testing boom gate..."));
  Serial.println(F("Arm UP..."));
  armUp();
  delay(500);
  
  Serial.println(F("Arm DOWN..."));
  armDown();
  delay(500);
  
  Serial.println(F("Arm UP..."));
  armUp();
  delay(500);
  
  Serial.println(F("Test complete."));
}
