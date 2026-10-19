/*
 * RELAY_CONF.cpp
 * 5-channel relay control implementation for EcoBrickMachine.
 * Channels 0–3 drive two linear actuators (press + seal).
 * Channel 4 drives an SSR for the 220 V soldering iron.
 */

#include "RELAY_CONF.h"

static const uint8_t RELAY_PINS[RELAY_COUNT] = {
  RELAY_PRESS_EXT_PIN,
  RELAY_PRESS_RET_PIN,
  RELAY_SEAL_EXT_PIN,
  RELAY_SEAL_RET_PIN,
  RELAY_SEAL_IRON_PIN
};

static bool relayState[RELAY_COUNT];

// Channel that must not be on together with the given channel.
static uint8_t relayPair(uint8_t relay) {
  switch (relay) {
    case RELAY_PRESS_EXT: return RELAY_PRESS_RET;
    case RELAY_PRESS_RET: return RELAY_PRESS_EXT;
    case RELAY_SEAL_EXT:  return RELAY_SEAL_RET;
    case RELAY_SEAL_RET:  return RELAY_SEAL_EXT;
    default:              return relay;
  }
}

void RelayConf_init(void) {
  for (uint8_t i = 0; i < RELAY_COUNT; i++) {
    pinMode(RELAY_PINS[i], OUTPUT);
    digitalWrite(RELAY_PINS[i], RELAY_OFF);
    relayState[i] = false;
  }
}

void RelayConf_on(uint8_t relay) {
  if (relay >= RELAY_COUNT) return;

  // Interlock: releasing the opposite channel first avoids polarity short.
  uint8_t other = relayPair(relay);
  if (other != relay) {
    digitalWrite(RELAY_PINS[other], RELAY_OFF);
    relayState[other] = false;
  }

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

void RelayConf_allOff(void) {
  for (uint8_t i = 0; i < RELAY_COUNT; i++) {
    RelayConf_off(i);
  }
}

// --- Actuator helpers ----------------------------------------------------

static void actuatorSet(uint8_t extChannel, uint8_t retChannel, int8_t dir) {
  if (dir > 0) {          // extend
    RelayConf_off(retChannel);
    RelayConf_on(extChannel);
  } else if (dir < 0) {   // retract
    RelayConf_off(extChannel);
    RelayConf_on(retChannel);
  } else {                // stop / hold position
    RelayConf_off(extChannel);
    RelayConf_off(retChannel);
  }
}

void RelayConf_pressExtend(void) {
  actuatorSet(RELAY_PRESS_EXT, RELAY_PRESS_RET, 1);
}

void RelayConf_pressRetract(void) {
  actuatorSet(RELAY_PRESS_EXT, RELAY_PRESS_RET, -1);
}

void RelayConf_pressStop(void) {
  actuatorSet(RELAY_PRESS_EXT, RELAY_PRESS_RET, 0);
}

void RelayConf_sealExtend(void) {
  actuatorSet(RELAY_SEAL_EXT, RELAY_SEAL_RET, 1);
}

void RelayConf_sealRetract(void) {
  actuatorSet(RELAY_SEAL_EXT, RELAY_SEAL_RET, -1);
}

void RelayConf_sealStop(void) {
  actuatorSet(RELAY_SEAL_EXT, RELAY_SEAL_RET, 0);
}

// --- Soldering iron (SSR) control ----------------------------------------

void RelayConf_ironOn(void) {
  // SSR is active HIGH (verify with your SSR datasheet).
  digitalWrite(RELAY_SEAL_IRON_PIN, HIGH);
  relayState[RELAY_SEAL_IRON] = true;
}

void RelayConf_ironOff(void) {
  digitalWrite(RELAY_SEAL_IRON_PIN, LOW);
  relayState[RELAY_SEAL_IRON] = false;
}

bool RelayConf_ironIsOn(void) {
  return relayState[RELAY_SEAL_IRON];
}
