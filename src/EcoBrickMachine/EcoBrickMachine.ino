/*
 * EcoBrickMachine.ino
 *
 * EcoBrickMachine controller: 1x TB6600 stepper + 2 linear actuators
 * (press + induction sealer), each driven by 2 relay channels.
 *
 * Auto cycle (default):
 *   1. FEED           : stepper feeds the bottle into the pressing station
 *   2. PRESS EXTEND   : press actuator extends, compressing the plastic
 *   3. PRESS HOLD     : actuator stopped, compression held
 *   4. PRESS RETRACT  : press actuator returns
 *   5. SEAL EXTEND    : sealing actuator moves the sealer onto the bottle
 *   6. SEAL HOLD      : induction heating applied, then stopped
 *   7. SEAL RETRACT   : sealing actuator returns
 *   8. RETURN         : stepper returns to the start position
 *   9. PAUSE          : everything off, then repeat
 *
 * Serial commands (115200 baud):
 *   a        toggle auto cycle
 *   f / b    stepper forward / backward (manual mode)
 *   z        stop stepper
 *   p / P    press actuator extend / retract
 *   l / L    seal actuator extend / retract
 *   0        stop all actuators
 *   ?        print status
 */

#include <Arduino.h>
#include "STEPPER_CONF.h"
#include "RELAY_CONF.h"

// --- Feed / stepper settings ---
#define FEED_STEPS      1600UL   // steps per feed stroke (1 rev @ 1/8 µstep)

// --- Press (compression) timings ---
#define PRESS_EXTEND_MS   1500UL
#define PRESS_HOLD_MS      500UL
#define PRESS_RETRACT_MS  1500UL

// --- Seal (induction sealing) timings ---
#define SEAL_EXTEND_MS    1500UL
#define SEAL_HOLD_MS      3000UL   // heating time on the bottle opening
#define SEAL_RETRACT_MS   1500UL

// --- Cycle pause ---
#define CYCLE_PAUSE_MS    1000UL

// --- Machine state ---
enum CycleState : uint8_t {
  STATE_FEED,
  STATE_PRESS_EXTEND,
  STATE_PRESS_HOLD,
  STATE_PRESS_RETRACT,
  STATE_SEAL_EXTEND,
  STATE_SEAL_HOLD,
  STATE_SEAL_RETRACT,
  STATE_RETURN,
  STATE_PAUSE
};

static CycleState state      = STATE_PAUSE;
static uint32_t stateStartMs = 0;
static bool autoMode         = true;

static bool elapsed(uint32_t ms) {
  return (millis() - stateStartMs) >= ms;
}

static void enterState(CycleState next) {
  state = next;
  stateStartMs = millis();

  switch (next) {
    case STATE_FEED:
      StepperConf_setDirection(true);               // feed into the press
      StepperConf_start(FEED_STEPS);
      break;

    case STATE_PRESS_EXTEND:
      RelayConf_pressExtend();
      break;

    case STATE_PRESS_HOLD:
      RelayConf_pressStop();                        // hold position
      break;

    case STATE_PRESS_RETRACT:
      RelayConf_pressRetract();
      break;

    case STATE_SEAL_EXTEND:
      RelayConf_pressStop();                        // belt-and-braces
      RelayConf_sealExtend();
      break;

    case STATE_SEAL_HOLD:
      RelayConf_sealStop();
      break;

    case STATE_SEAL_RETRACT:
      RelayConf_sealRetract();
      break;

    case STATE_RETURN:
      RelayConf_sealStop();
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

  Serial.print(F(" press="));
  if (RelayConf_isOn(RELAY_PRESS_EXT))      Serial.print(F("extend"));
  else if (RelayConf_isOn(RELAY_PRESS_RET)) Serial.print(F("retract"));
  else                                      Serial.print(F("stop"));

  Serial.print(F(" seal="));
  if (RelayConf_isOn(RELAY_SEAL_EXT))       Serial.print(F("extend"));
  else if (RelayConf_isOn(RELAY_SEAL_RET))  Serial.print(F("retract"));
  else                                      Serial.print(F("stop"));

  Serial.println();
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

      case 'z':
        StepperConf_stop();
        break;

      case 'p':                                    // press manual controls
      case 'P':
      case 'l':                                    // seal manual controls
      case 'L':
      case '0':
        if (autoMode) break;
        if (cmd == 'p') RelayConf_pressExtend();
        if (cmd == 'P') RelayConf_pressRetract();
        if (cmd == 'l') RelayConf_sealExtend();
        if (cmd == 'L') RelayConf_sealRetract();
        if (cmd == '0') RelayConf_allOff();
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
      if (!StepperConf_isBusy()) {                  // bottle in position
        enterState(STATE_PRESS_EXTEND);
      }
      break;

    case STATE_PRESS_EXTEND:
      if (elapsed(PRESS_EXTEND_MS)) {
        enterState(STATE_PRESS_HOLD);
      }
      break;

    case STATE_PRESS_HOLD:
      if (elapsed(PRESS_HOLD_MS)) {
        enterState(STATE_PRESS_RETRACT);
      }
      break;

    case STATE_PRESS_RETRACT:
      if (elapsed(PRESS_RETRACT_MS)) {
        enterState(STATE_SEAL_EXTEND);
      }
      break;

    case STATE_SEAL_EXTEND:
      if (elapsed(SEAL_EXTEND_MS)) {
        enterState(STATE_SEAL_HOLD);
      }
      break;

    case STATE_SEAL_HOLD:
      if (elapsed(SEAL_HOLD_MS)) {
        enterState(STATE_SEAL_RETRACT);
      }
      break;

    case STATE_SEAL_RETRACT:
      if (elapsed(SEAL_RETRACT_MS)) {
        enterState(STATE_RETURN);
      }
      break;

    case STATE_RETURN:
      if (!StepperConf_isBusy()) {
        enterState(STATE_PAUSE);
      }
      break;

    case STATE_PAUSE:
      if (elapsed(CYCLE_PAUSE_MS)) {
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
