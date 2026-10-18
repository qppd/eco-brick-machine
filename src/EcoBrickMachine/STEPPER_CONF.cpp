/*
 * STEPPER_CONF.cpp
 * TB6600 stepper driver control implementation for EcoBrickMachine.
 */

#include "STEPPER_CONF.h"

static uint32_t stepIntervalUs  = STEPPER_DEFAULT_STEP_US;
static uint32_t stepsRemaining  = 0;
static uint32_t lastStepUs      = 0;

void StepperConf_init(void) {
  pinMode(STEPPER_STEP_PIN, OUTPUT);
  pinMode(STEPPER_DIR_PIN, OUTPUT);
  pinMode(STEPPER_EN_PIN, OUTPUT);

  digitalWrite(STEPPER_STEP_PIN, LOW);
  digitalWrite(STEPPER_DIR_PIN, HIGH);          // default direction: CW
  digitalWrite(STEPPER_EN_PIN, STEPPER_EN_FREE); // coils de-energized

  stepsRemaining = 0;
}

void StepperConf_enable(void) {
  digitalWrite(STEPPER_EN_PIN, STEPPER_EN_ENERGIZE);
}

void StepperConf_disable(void) {
  digitalWrite(STEPPER_EN_PIN, STEPPER_EN_FREE);
}

void StepperConf_setDirection(bool clockwise) {
  digitalWrite(STEPPER_DIR_PIN, clockwise ? HIGH : LOW);
}

void StepperConf_setStepInterval(uint32_t intervalUs) {
  // Keep at least the TB6600 minimum pulse width (+ margin) between pulses.
  if (intervalUs < (STEPPER_PULSE_US * 4)) {
    intervalUs = STEPPER_PULSE_US * 4;
  }
  stepIntervalUs = intervalUs;
}

uint32_t StepperConf_getStepInterval(void) {
  return stepIntervalUs;
}

void StepperConf_setSpeedRpm(float rpm) {
  if (rpm <= 0.0f) return;

  // interval [us] = 60e6 / (rpm * steps per revolution)
  float interval = 60000000.0f / (rpm * (float)STEPPER_STEPS_PER_REV);
  StepperConf_setStepInterval((uint32_t)interval);
}

void StepperConf_step(void) {
  digitalWrite(STEPPER_STEP_PIN, HIGH);
  delayMicroseconds(STEPPER_PULSE_US);
  digitalWrite(STEPPER_STEP_PIN, LOW);
}

// --- Blocking move -------------------------------------------------------

void StepperConf_runSteps(uint32_t steps) {
  StepperConf_start(steps);
  while (StepperConf_update()) {
    // busy wait; timing is handled by micros() inside update()
  }
}

// --- Non-blocking move ---------------------------------------------------

void StepperConf_start(uint32_t steps) {
  if (steps == 0) {
    stepsRemaining = 0;
    StepperConf_disable();
    return;
  }

  StepperConf_enable();
  stepsRemaining = steps;
  lastStepUs = micros();
}

bool StepperConf_update(void) {
  if (stepsRemaining == 0) {
    return false;
  }

  uint32_t now = micros();
  if ((uint32_t)(now - lastStepUs) >= stepIntervalUs) {
    StepperConf_step();
    lastStepUs = now;               // avoids burst-catching if we fall behind
    stepsRemaining--;

    if (stepsRemaining == 0) {
      StepperConf_disable();        // release the motor when the move ends
    }
  }

  return stepsRemaining > 0;
}

bool StepperConf_isBusy(void) {
  return stepsRemaining > 0;
}

void StepperConf_stop(void) {
  stepsRemaining = 0;
  StepperConf_disable();
}
