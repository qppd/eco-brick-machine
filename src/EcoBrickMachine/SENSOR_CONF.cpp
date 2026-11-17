/*
 * SENSOR_CONF.cpp
 * IR station sensors and conveyor home limit switch implementation.
 */

#include "SENSOR_CONF.h"

void SensorConf::DebouncedInput::begin(uint8_t p) {
  pin = p;
  pinMode(pin, INPUT_PULLUP);
  lastLevel    = digitalRead(pin);
  stableState  = lastLevel;
  lastChangeMs = millis();
}

bool SensorConf::DebouncedInput::update(void) {
  bool raw = digitalRead(pin);

  if (raw != lastLevel) {
    // Level changed — restart the debounce window, keep the old stable state
    lastLevel    = raw;
    lastChangeMs = millis();
    return stableState;
  }

  if ((millis() - lastChangeMs) >= SENSOR_DEBOUNCE_MS) {
    stableState = raw;
  }

  return stableState;
}

void SensorConf::begin(void) {
  irPress_.begin(IR_PRESS_PIN);
  irSeal_.begin(IR_SEAL_PIN);
  homeLimit_.begin(HOME_LIMIT_PIN);
}

bool SensorConf::pressStationDetected(void) {
  return irPress_.update() == IR_ACTIVE_LEVEL;
}

bool SensorConf::sealStationDetected(void) {
  return irSeal_.update() == IR_ACTIVE_LEVEL;
}

bool SensorConf::conveyorHomed(void) {
  return homeLimit_.update() == HOME_LIMIT_ACTIVE_LEVEL;
}

bool SensorConf::pressStationRaw(void) {
  return digitalRead(IR_PRESS_PIN) == IR_ACTIVE_LEVEL;
}

bool SensorConf::sealStationRaw(void) {
  return digitalRead(IR_SEAL_PIN) == IR_ACTIVE_LEVEL;
}

bool SensorConf::homeSwitchRaw(void) {
  return digitalRead(HOME_LIMIT_PIN) == HOME_LIMIT_ACTIVE_LEVEL;
}
