/*
 * RELAY_CONF.h
 * Relay and SSR control for EcoBrickMachine:
 *   - Press actuator: 2CH 5V relay board with optocoupler (channels 0-1)
 *   - Seal actuator: 2CH 5V relay board with optocoupler (channels 2-3)
 *   - Soldering iron: 1CH 5V relay module (channel 4, active LOW)
 *   - Shredder motor: 1CH SSR module (channel 5, active HIGH)
 *
 * Board: ESP32 DevKit, 38-pin (ESP32-WROOM-32)
 *
 * Press actuator:
 *   RELAY_PRESS_EXT -> extend,  RELAY_PRESS_RET -> retract
 * Seal actuator:
 *   RELAY_SEAL_EXT  -> extend,  RELAY_SEAL_RET  -> retract
 * Soldering iron (220 V):
 *   RELAY_SEAL_IRON -> Relay output, LOW = iron ON (active LOW relay module)
 * Shredder motor (AC):
 *   RELAY_SHREDDER  -> SSR output, HIGH = motor ON (active HIGH SSR module)
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
#define RELAY_SEAL_IRON_PIN   4   // 1CH 5V relay module for soldering iron, active LOW
#define RELAY_SHREDDER_PIN   18   // 1CH SSR module for AC shredder motor, active HIGH

// --- Trigger level for relay boards (active LOW) ---
#define RELAY_ON    LOW
#define RELAY_OFF   HIGH

// --- Channel index ---
#define RELAY_PRESS_EXT  0
#define RELAY_PRESS_RET  1
#define RELAY_SEAL_EXT   2
#define RELAY_SEAL_RET   3
#define RELAY_SEAL_IRON  4
#define RELAY_SHREDDER   5

#define RELAY_COUNT  6

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

// --- Soldering iron (active LOW relay) ----------------------------------
void RelayConf_ironOn(void);
void RelayConf_ironOff(void);
bool RelayConf_ironIsOn(void);

// --- Shredder AC motor (active HIGH SSR) --------------------------------
void RelayConf_shredderOn(void);
void RelayConf_shredderOff(void);
bool RelayConf_shredderIsOn(void);

#endif  // RELAY_CONF_H
