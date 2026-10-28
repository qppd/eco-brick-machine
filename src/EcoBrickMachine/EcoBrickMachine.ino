/*
 * EcoBrickMachine.ino
 *
 * EcoBrickMachine controller: 1x TB6600 stepper + 2 linear actuators
 * (press + seal carrier) + SSR for 220 V soldering iron + SSR for AC shredder.
 *
 * Hardware:
 *   - Press actuator: 2CH 5V relay board with optocoupler (extend/retract)
 *   - Seal actuator: 2CH 5V relay board with optocoupler (extend/retract)
 *   - Soldering iron: 1CH SSR module (stays ON once machine started)
 *   - Shredder motor: 1CH SSR module (turns ON with START button)
 *   - START button: tactile, GPIO 21, internal pull-up, active LOW
 *   - STOP button: tactile, GPIO 22, internal pull-up, active LOW
 *
 * Operation:
 *   Press START -> iron SSR ON, shredder SSR ON, auto cycle begins
 *   Press STOP  -> cycle pauses, iron and shredder remain ON
 *   Press START again -> cycle resumes
 *
 * Serial commands (115200 baud, manual mode only when paused):
 *   f / b    stepper forward / backward
 *   z        stop stepper
 *   p / P    press actuator extend / retract
 *   l / L    seal actuator extend / retract
 *   0        stop all actuators
 *   ?        print status
 */

#include <Arduino.h>
#include "STEPPER_CONF.h"
#include "RELAY_CONF.h"

// --- Button pins (tactile, active LOW with internal pull-up) ---
#define PIN_START_BUTTON  21
#define PIN_STOP_BUTTON   22

// --- Button debounce ---
#define BUTTON_DEBOUNCE_MS  200UL

// --- Feed / stepper settings ---
#define FEED_STEPS      1600UL   // steps per feed stroke (1 rev @ 1/8 ustep)

// --- Press (compression) timings ---
#define PRESS_EXTEND_MS   1500UL
#define PRESS_HOLD_MS      500UL
#define PRESS_RETRACT_MS  1500UL

// --- Seal (induction sealing) timings ---
#define SEAL_EXTEND_MS    1500UL
#define SEAL_HOLD_MS      3000UL   // iron ON time: melt PET neck to fuse seal
#define SEAL_RETRACT_MS   1500UL

// --- Cycle pause ---
#define CYCLE_PAUSE_MS    1000UL

// --- Machine state ---
enum CycleState : uint8_t {
  STATE_IDLE,          // waiting for START button
  STATE_RUNNING,       // auto cycle running, iron+shredder ON
  STATE_PAUSED         // cycle paused, iron+shredder still ON
};

static CycleState state        = STATE_IDLE;
static uint32_t stateStartMs   = 0;
static bool ironOn             = false;
static bool shredderOn         = false;

// --- Button state ---
static bool lastStartState = HIGH;
static bool lastStopState  = HIGH;
static uint32_t lastStartDebounceMs = 0;
static uint32_t lastStopDebounceMs  = 0;
static bool startPressed = false;
static bool stopPressed  = false;

static bool elapsed(uint32_t ms) {
  return (millis() - stateStartMs) >= ms;
}

// --- Button reading with debounce ----------------------------------------

static bool readButton(uint8_t pin, bool &lastState, uint32_t &lastDebounceMs, bool &pressed) {
  bool current = digitalRead(pin);

  if (current != lastState) {
    // state changed - reset debounce timer
    lastDebounceMs = millis();
    lastState = current;
    pressed = false;
    return false;
  }

  // same state for long enough - consider it valid
  if ((millis() - lastDebounceMs) >= BUTTON_DEBOUNCE_MS) {
    if (!pressed && current == LOW) {
      pressed = true;
    }
  }

  return pressed;
}

static void handleButtons(void) {
  readButton(PIN_START_BUTTON, lastStartState, lastStartDebounceMs, startPressed);
  readButton(PIN_STOP_BUTTON,  lastStopState,  lastStopDebounceMs,  stopPressed);
}

static void enterState(CycleState next) {
  state = next;
  stateStartMs = millis();

  switch (next) {
    case STATE_IDLE:
      RelayConf_allOff();
      ironOn = false;
      shredderOn = false;
      break;

    case STATE_RUNNING:
      // Iron and shredder stay ON for entire run cycle
      if (!ironOn) {
        RelayConf_ironOn();
        ironOn = true;
      }
      if (!shredderOn) {
        RelayConf_shredderOn();
        shredderOn = true;
      }
      // Fall through to start first cycle step
      [[fallthrough]];

    case STATE_PAUSED:
      // Do nothing extra - iron and shredder remain on
      break;
  }

  // Sub-state for the auto cycle (only entered when RUNNING)
  if (next == STATE_RUNNING || next == STATE_PAUSED) {
    // If transitioning to RUNNING from IDLE, start the cycle
    if (next == STATE_RUNNING && stateStartMs == millis()) {
      // Fresh start - begin from feed
      StepperConf_setDirection(true);
      StepperConf_start(FEED_STEPS);
      RelayConf_pressExtend();
      stateStartMs = millis();
    }
  }
}

// --- Serial command handling --------------------------------------------

static void printStatus(void) {
  Serial.print(F("state="));
  switch (state) {
    case STATE_IDLE:   Serial.print(F("idle")); break;
    case STATE_RUNNING:Serial.print(F("running")); break;
    case STATE_PAUSED: Serial.print(F("paused")); break;
  }

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

  Serial.print(F(" iron="));
  Serial.print(ironOn ? F("on") : F("off"));

  Serial.print(F(" shredder="));
  Serial.println(shredderOn ? F("on") : F("off"));

  Serial.println();
}

static void handleSerial(void) {
  while (Serial.available() > 0) {
    char cmd = Serial.read();

    switch (cmd) {
      case 'f':
      case 'b':
        if (state != STATE_PAUSED) break;           // manual only when paused
        StepperConf_setDirection(cmd == 'f');
        StepperConf_start(FEED_STEPS);
        break;

      case 'z':
        StepperConf_stop();
        break;

      case 'p':
      case 'P':
      case 'l':
      case 'L':
      case '0':
        if (state != STATE_PAUSED) break;
        if (cmd == 'p') RelayConf_pressExtend();
        if (cmd == 'P') RelayConf_pressRetract();
        if (cmd == 'l') RelayConf_sealExtend();
        if (cmd == 'L') RelayConf_sealRetract();
        if (cmd == '0') {
          RelayConf_pressStop();
          RelayConf_sealStop();
          StepperConf_stop();
        }
        break;

      case '?':
        printStatus();
        break;

      default:
        break;
    }
  }
}

// --- Automatic cycle -----------------------------------------------------

static void runCycle(void) {
  // State machine for the production cycle
  static uint8_t cycleStep = 0;
  static bool cycleRunning = false;

  // Handle button input
  if (startPressed) {
    startPressed = false;
    if (state == STATE_IDLE || state == STATE_PAUSED) {
      enterState(STATE_RUNNING);
      cycleStep = 0;
      cycleRunning = true;
      Serial.println(F("START pressed - cycle running"));
    }
    return;
  }

  if (stopPressed) {
    stopPressed = false;
    if (state == STATE_RUNNING) {
      StepperConf_stop();
      RelayConf_pressStop();
      RelayConf_sealStop();
      enterState(STATE_PAUSED);
      Serial.println(F("STOP pressed - cycle paused (iron+shredder remain ON)"));
    }
    return;
  }

  // Only run cycle logic when in RUNNING state
  if (state != STATE_RUNNING) return;

  // Cycle steps:
  // 0 = FEED (stepper moves bottle into position)
  // 1 = PRESS EXTEND
  // 2 = PRESS HOLD
  // 3 = PRESS RETRACT
  // 4 = SEAL EXTEND
  // 5 = SEAL HOLD (iron already ON)
  // 6 = SEAL RETRACT
  // 7 = RETURN (stepper back to origin)
  // 8 = brief pause then loop

  switch (cycleStep) {
    case 0: // FEED - stepper moves bottle under press
      if (!StepperConf_isBusy()) {
        cycleStep = 1;
        stateStartMs = millis();
        Serial.println(F("step: press extend"));
      }
      break;

    case 1: // PRESS EXTEND
      if (elapsed(PRESS_EXTEND_MS)) {
        cycleStep = 2;
        RelayConf_pressStop();
        stateStartMs = millis();
        Serial.println(F("step: press hold"));
      }
      break;

    case 2: // PRESS HOLD
      if (elapsed(PRESS_HOLD_MS)) {
        cycleStep = 3;
        RelayConf_pressRetract();
        stateStartMs = millis();
        Serial.println(F("step: press retract"));
      }
      break;

    case 3: // PRESS RETRACT
      if (elapsed(PRESS_RETRACT_MS)) {
        cycleStep = 4;
        RelayConf_sealExtend();
        stateStartMs = millis();
        Serial.println(F("step: seal extend"));
      }
      break;

    case 4: // SEAL EXTEND
      if (elapsed(SEAL_EXTEND_MS)) {
        cycleStep = 5;
        RelayConf_sealStop();
        stateStartMs = millis();
        Serial.println(F("step: seal hold (iron ON)"));
      }
      break;

    case 5: // SEAL HOLD - iron stays ON
      if (elapsed(SEAL_HOLD_MS)) {
        cycleStep = 6;
        RelayConf_sealRetract();
        stateStartMs = millis();
        Serial.println(F("step: seal retract"));
      }
      break;

    case 6: // SEAL RETRACT
      if (elapsed(SEAL_RETRACT_MS)) {
        cycleStep = 7;
        StepperConf_setDirection(false);
        StepperConf_start(FEED_STEPS);
        stateStartMs = millis();
        Serial.println(F("step: return"));
      }
      break;

    case 7: // RETURN - stepper back to origin
      if (!StepperConf_isBusy()) {
        cycleStep = 8;
        RelayConf_sealStop();
        stateStartMs = millis();
        Serial.println(F("step: pause"));
      }
      break;

    case 8: // PAUSE between cycles
      if (elapsed(CYCLE_PAUSE_MS)) {
        cycleStep = 0;
        StepperConf_setDirection(true);
        StepperConf_start(FEED_STEPS);
        RelayConf_pressExtend();
        stateStartMs = millis();
        Serial.println(F("step: feed (next cycle)"));
      }
      break;
  }
}

// --- Arduino entry points ------------------------------------------------

void setup() {
  Serial.begin(115200);

  // Configure button pins with internal pull-ups
  pinMode(PIN_START_BUTTON, INPUT_PULLUP);
  pinMode(PIN_STOP_BUTTON,  INPUT_PULLUP);

  RelayConf_init();
  StepperConf_init();
  StepperConf_setSpeedRpm(60.0f);                   // feed speed

  // Ensure all relays are off and SSRs are off at boot
  RelayConf_allOff();

  Serial.println(F("EcoBrickMachine ready."));
  Serial.println(F("Press START button to begin cycle."));
  Serial.println(F("Send '?' for status, '0' to stop actuators."));
  enterState(STATE_IDLE);
}

void loop() {
  handleButtons();       // read buttons with debounce
  handleSerial();        // handle serial commands

  if (state == STATE_RUNNING) {
    runCycle();          // advance production cycle
  }

  StepperConf_update();  // non-blocking stepping
}
