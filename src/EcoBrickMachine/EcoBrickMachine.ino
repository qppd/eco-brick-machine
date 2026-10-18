/*
 * EcoBrickMachine.ino
 *
 * EcoBrickMachine controller: 1x TB6600 stepper + 2 relays.
 *
 * Auto cycle (default):
 *   1. FEED   : relay1 ON  -> feeder/conveyor runs, stepper drives forward
 *   2. PRESS  : relay1 OFF, relay2 ON for PRESS_MS (press/compactor)
 *   3. RETURN : relay2 OFF, stepper returns to the start position
 *   4. PAUSE  : everything off for CYCLE_PAUSE_MS, then repeat
 *
 * Serial commands (115200 baud) while auto mode is paused:
 *   a  toggle auto cycle
 *   f  stepper forward  b  stepper backward  s  stepper stop
 *   1  toggle relay1    2  toggle relay2     x  all relays off
 *   ?  print status
 */

#include <Arduino.h>
#include "STEPPER_CONF.h"
#include "RELAY_CONF.h"

// --- Cycle settings ---
#define FEED_STEPS      1600UL   // steps per feed stroke (1 rev @ 1/8 µstep)
#define PRESS_MS        1500UL   // relay2 on time
#define CYCLE_PAUSE_MS  1000UL   // pause between cycles

// --- Machine state ---
enum CycleState : uint8_t {
  STATE_FEED,    // relay1 on + stepper forward
  STATE_PRESS,   // relay2 on, timed
  STATE_RETURN,  // stepper back to origin
  STATE_PAUSE    // everything off, timed
};

static CycleState state     = STATE_FEED;
static uint32_t stateStartMs = 0;
static bool autoMode         = true;

static void enterState(CycleState next) {
  state = next;
  stateStartMs = millis();

  switch (next) {
    case STATE_FEED:
      RelayConf_on(RELAY_1);                        // feeder/conveyor runs
      StepperConf_setDirection(true);               // forward
      StepperConf_start(FEED_STEPS);
      break;

    case STATE_PRESS:
      RelayConf_off(RELAY_1);
      RelayConf_on(RELAY_2);                        // press/compactor
      break;

    case STATE_RETURN:
      RelayConf_off(RELAY_2);
      StepperConf_setDirection(false);              // back to origin
      StepperConf_start(FEED_STEPS);
      break;

    case STATE_PAUSE:
      RelayConf_allOff();
      break;
  }
}

// --- Serial command handling --------------------------------------------

static void printStatus(void) {
  Serial.print(F("auto="));
  Serial.print(autoMode ? F("on") : F("off"));
  Serial.print(F(" state="));
  Serial.print(state);
  Serial.print(F(" stepperBusy="));
  Serial.print(StepperConf_isBusy() ? F("yes") : F("no"));
  Serial.print(F(" relay1="));
  Serial.print(RelayConf_isOn(RELAY_1) ? F("on") : F("off"));
  Serial.print(F(" relay2="));
  Serial.println(RelayConf_isOn(RELAY_2) ? F("on") : F("off"));
}

static void handleSerial(void) {
  while (Serial.available() > 0) {
    char cmd = Serial.read();

    switch (cmd) {
      case 'a':
        autoMode = !autoMode;
        if (autoMode) {
          enterState(STATE_FEED);                    // resume the cycle
        } else {
          RelayConf_allOff();                        // safe pause
          StepperConf_stop();
        }
        Serial.println(autoMode ? F("auto ON") : F("auto OFF"));
        break;

      case 'f':
      case 'b':
        if (autoMode) break;                       // manual only when paused
        StepperConf_setDirection(cmd == 'f');
        StepperConf_start(FEED_STEPS);
        break;

      case 's':
        StepperConf_stop();
        break;

      case '1':
        if (!autoMode) RelayConf_toggle(RELAY_1);
        break;

      case '2':
        if (!autoMode) RelayConf_toggle(RELAY_2);
        break;

      case 'x':
        RelayConf_allOff();
        break;

      case '?':
        printStatus();
        break;

      default:
        break;                                      // ignore whitespace etc.
    }
  }
}

// --- Automatic cycle -----------------------------------------------------

static void runCycle(void) {
  switch (state) {
    case STATE_FEED:
      // done when the forward stroke has finished
      if (!StepperConf_isBusy()) {
        enterState(STATE_PRESS);
      }
      break;

    case STATE_PRESS:
      if ((millis() - stateStartMs) >= PRESS_MS) {
        enterState(STATE_RETURN);
      }
      break;

    case STATE_RETURN:
      if (!StepperConf_isBusy()) {
        enterState(STATE_PAUSE);
      }
      break;

    case STATE_PAUSE:
      if ((millis() - stateStartMs) >= CYCLE_PAUSE_MS) {
        enterState(STATE_FEED);
      }
      break;
  }
}

// --- Arduino entry points ------------------------------------------------

void setup() {
  Serial.begin(115200);

  RelayConf_init();
  StepperConf_init();
  StepperConf_setSpeedRpm(60.0f);                   // feed speed

  Serial.println(F("EcoBrickMachine ready. Send '?' for status."));
  enterState(STATE_PAUSE);        // boot with everything off; first feed after pause
}

void loop() {
  handleSerial();

  if (autoMode) {
    runCycle();
  }

  StepperConf_update();                               // non-blocking stepping
}
