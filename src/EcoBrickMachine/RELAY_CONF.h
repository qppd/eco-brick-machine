/*
 * RELAY_CONF.h
 * 4 relay channels for EcoBrickMachine: 2 linear actuators x 2 channels.
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
 */

#ifndef RELAY_CONF_H
#define RELAY_CONF_H

#include <Arduino.h>

// --- Pin configuration ---
#define RELAY_PRESS_EXT_PIN  6
#define RELAY_PRESS_RET_PIN  7
#define RELAY_SEAL_EXT_PIN   8
#define RELAY_SEAL_RET_PIN   9

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
