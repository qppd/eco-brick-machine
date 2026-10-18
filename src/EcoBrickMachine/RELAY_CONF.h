/*
 * RELAY_CONF.h
 * 4 relay channels for EcoBrickMachine: 2 linear actuators x 2 channels.
 *
 * Board: ESP32 DevKit, 38-pin (ESP32-WROOM-32)
 *
 * Press actuator (compression station):
 *   RELAY_PRESS_EXT -> extend,  RELAY_PRESS_RET -> retract
 * Sealing actuator (induction sealing station):
 *   RELAY_SEAL_EXT  -> extend,  RELAY_SEAL_RET  -> retract
 *
 * Wiring: each channel switches one supply lead of the actuator, so
 * EXT + RET together reverse the polarity (that is why EXT and RET of the
 * same actuator must NEVER be energized at the same time - the interlock
 * below enforces that). All channels off = actuator stopped / holding.
 *
 * Most relay boards are active LOW (input LOW = coil energized).
 * ESP32 pins output 3.3V, so use a relay board that switches reliably at
 * 3.3V logic (many 5V boards do; otherwise add a level shifter).
 */

#ifndef RELAY_CONF_H
#define RELAY_CONF_H

#include <Arduino.h>

// --- Pin configuration (ESP32 38-pin) ---
#define RELAY_PRESS_EXT_PIN  32
#define RELAY_PRESS_RET_PIN  33
#define RELAY_SEAL_EXT_PIN   13
#define RELAY_SEAL_RET_PIN   14

// --- Trigger level ---
#define RELAY_ON    LOW
#define RELAY_OFF   HIGH

// --- Channel index ---
#define RELAY_PRESS_EXT  0
#define RELAY_PRESS_RET  1
#define RELAY_SEAL_EXT   2
#define RELAY_SEAL_RET   3

#define RELAY_COUNT  4

void RelayConf_init(void);
void RelayConf_on(uint8_t relay);
void RelayConf_off(uint8_t relay);
void RelayConf_toggle(uint8_t relay);
bool RelayConf_isOn(uint8_t relay);
void RelayConf_allOff(void);   // safe state: every channel off

// --- Actuator-level helpers (interlocked: never EXT + RET together) -----
void RelayConf_pressExtend(void);
void RelayConf_pressRetract(void);
void RelayConf_pressStop(void);
void RelayConf_sealExtend(void);
void RelayConf_sealRetract(void);
void RelayConf_sealStop(void);

#endif  // RELAY_CONF_H
