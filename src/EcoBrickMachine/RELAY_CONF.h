/*
 * RELAY_CONF.h
 * 5 relay channels for EcoBrickMachine:
 *   - 2 channels per press actuator (compression)
 *   - 2 channels per seal actuator (soldering iron carrier)
 *   - 1 channel for SSR driving the 220 V soldering iron
 *
 * Board: ESP32 DevKit, 38-pin (ESP32-WROOM-32)
 *
 * Press actuator:
 *   RELAY_PRESS_EXT -> extend,  RELAY_PRESS_RET -> retract
 * Seal actuator:
 *   RELAY_SEAL_EXT  -> extend,  RELAY_SEAL_RET  -> retract
 * Soldering iron (220 V):
 *   RELAY_SEAL_IRON -> SSR output, HIGH = iron ON
 *
 * Actuator wiring: each pair switches one supply lead so EXT + RET
 * together reverse polarity. They must NEVER be on simultaneously —
 * the interlock enforces that. All channels off = actuator stopped.
 *
 * SSR note: most solid-state relays for 220 V loads are active HIGH
 * (input HIGH = load powered). Verify with your SSR datasheet.
 *
 * Relay boards are typically active LOW (input LOW = coil energized).
 * ESP32 outputs 3.3V — use a board that triggers at that level or
 * add a level shifter.
 */

#ifndef RELAY_CONF_H
#define RELAY_CONF_H

#include <Arduino.h>

// --- Pin configuration (ESP32 38-pin) ---
#define RELAY_PRESS_EXT_PIN  32
#define RELAY_PRESS_RET_PIN  33
#define RELAY_SEAL_EXT_PIN   13
#define RELAY_SEAL_RET_PIN   14
#define RELAY_SEAL_IRON_PIN   4   // SSR gate for 220 V soldering iron

// --- Trigger level ---
#define RELAY_ON    LOW
#define RELAY_OFF   HIGH

// --- Channel index ---
#define RELAY_PRESS_EXT  0
#define RELAY_PRESS_RET  1
#define RELAY_SEAL_EXT   2
#define RELAY_SEAL_RET   3
#define RELAY_SEAL_IRON  4

#define RELAY_COUNT  5

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

// --- Soldering iron (SSR) -----------------------------------------------
void RelayConf_ironOn(void);
void RelayConf_ironOff(void);
bool RelayConf_ironIsOn(void);

#endif  // RELAY_CONF_H
