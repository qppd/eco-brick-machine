/*
 * SENSOR_CONF.h
 * Station and homing sensors for EcoBrickMachine:
 *
 *   - IR sensor 1: bottle detection at the PRESSING station (GPIO 5)
 *   - IR sensor 2: bottle detection at the SEALING station (GPIO 17)
 *   - Home limit switch: conveyor home position (GPIO 16)
 *
 * The IR sensors are digital obstacle-avoidance style modules with a DO
 * output. The home limit switch is a mechanical switch wired to GND with
 * the ESP32 internal pull-up.
 *
 * Wiring (ESP32 38-pin):
 *   IR sensor 1 DO -> GPIO 5   (strapping pin: output must not hold it
 *                               LOW at power-on)
 *   IR sensor 2 DO -> GPIO 17  (free on WROOM modules without PSRAM)
 *   Home limit SW  -> GPIO 16  (free on WROOM modules without PSRAM,
 *                               internal pull-up, active LOW)
 *   IR sensor VCC  -> 5V (or 3.3V, per module), GND -> GND (common)
 *
 * All inputs are debounced (SENSOR_DEBOUNCE_MS) before their state is
 * reported, so a bottle rolling past an IR beam cannot cause a false
 * trigger.
 */

#ifndef SENSOR_CONF_H
#define SENSOR_CONF_H

#include <Arduino.h>

// --- Pin configuration (ESP32 38-pin) ---
#define IR_PRESS_PIN        5    // IR sensor 1: pressing station
#define IR_SEAL_PIN         17   // IR sensor 2: sealing station
#define HOME_LIMIT_PIN      16   // conveyor home limit switch

// --- Trigger levels ---
// Most IR obstacle modules drive DO LOW when the beam is broken (bottle present).
#define IR_ACTIVE_LEVEL         LOW
// Limit switch closes to GND when the conveyor is at home.
#define HOME_LIMIT_ACTIVE_LEVEL LOW

// --- Timing ---
#define SENSOR_DEBOUNCE_MS  50UL

class SensorConf {
public:
  void begin(void);

  bool pressStationDetected(void);   // true = bottle at pressing station
  bool sealStationDetected(void);    // true = bottle at sealing station
  bool conveyorHomed(void);          // true = conveyor at home position

  // Debounced raw state, useful for diagnostics
  bool pressStationRaw(void);
  bool sealStationRaw(void);
  bool homeSwitchRaw(void);

private:
  struct DebouncedInput {
    uint8_t  pin;
    bool     lastLevel;
    bool     stableState;
    uint32_t lastChangeMs;

    void begin(uint8_t p);
    bool update(void);   // returns debounced raw level
  };

  DebouncedInput irPress_;
  DebouncedInput irSeal_;
  DebouncedInput homeLimit_;
};

#endif // SENSOR_CONF_H
