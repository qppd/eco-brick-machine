/*
 * RELAY_CONF.cpp
 * 2-channel relay control implementation for EcoBrickMachine.
 */

#include "RELAY_CONF.h"

static const uint8_t RELAY_PINS[RELAY_COUNT] = { RELAY1_PIN, RELAY2_PIN };
static bool relayState[RELAY_COUNT];

void RelayConf_init(void) {
  for (uint8_t i = 0; i < RELAY_COUNT; i++) {
    pinMode(RELAY_PINS[i], OUTPUT);
    digitalWrite(RELAY_PINS[i], RELAY_OFF);
    relayState[i] = false;
  }
}

void RelayConf_on(uint8_t relay) {
  if (relay >= RELAY_COUNT) return;

  digitalWrite(RELAY_PINS[relay], RELAY_ON);
  relayState[relay] = true;
}

void RelayConf_off(uint8_t relay) {
  if (relay >= RELAY_COUNT) return;

  digitalWrite(RELAY_PINS[relay], RELAY_OFF);
  relayState[relay] = false;
}

void RelayConf_toggle(uint8_t relay) {
  if (relay >= RELAY_COUNT) return;

  if (relayState[relay]) {
    RelayConf_off(relay);
  } else {
    RelayConf_on(relay);
  }
}

bool RelayConf_isOn(uint8_t relay) {
  return (relay < RELAY_COUNT) ? relayState[relay] : false;
}

void RelayConf_allOn(void) {
  for (uint8_t i = 0; i < RELAY_COUNT; i++) {
    RelayConf_on(i);
  }
}

void RelayConf_allOff(void) {
  for (uint8_t i = 0; i < RELAY_COUNT; i++) {
    RelayConf_off(i);
  }
}
