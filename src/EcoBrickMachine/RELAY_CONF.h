/*
 * RELAY_CONF.h
 * 2-channel relay control for EcoBrickMachine.
 *
 *   RELAY1 -> e.g. material feeder / conveyor
 *   RELAY2 -> e.g. press / compactor
 *
 * Most relay boards are active LOW (input LOW = coil energized).
 */

#ifndef RELAY_CONF_H
#define RELAY_CONF_H

#include <Arduino.h>

// --- Pin configuration ---
#define RELAY1_PIN  6
#define RELAY2_PIN  7

// --- Trigger level ---
#define RELAY_ON    LOW
#define RELAY_OFF   HIGH

// --- Relay index ---
#define RELAY_1  0
#define RELAY_2  1

#define RELAY_COUNT  2

void RelayConf_init(void);
void RelayConf_on(uint8_t relay);
void RelayConf_off(uint8_t relay);
void RelayConf_toggle(uint8_t relay);
bool RelayConf_isOn(uint8_t relay);
void RelayConf_allOn(void);
void RelayConf_allOff(void);

#endif  // RELAY_CONF_H
