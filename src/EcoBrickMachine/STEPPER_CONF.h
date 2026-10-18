/*
 * STEPPER_CONF.h
 * TB6600 stepper driver control (PUL / DIR / ENA) for EcoBrickMachine.
 *
 * Wiring (Arduino -> TB6600 module):
 *   STEPPER_STEP_PIN -> PUL-   (PUL+ to +5V)
 *   STEPPER_DIR_PIN  -> DIR-   (DIR+ to +5V)
 *   STEPPER_EN_PIN   -> ENA-   (ENA+ to +5V)
 *
 * Set the TB6600 DIP switches for microstepping and current, and keep
 * STEPPER_STEPS_PER_REV in sync with the switch setting.
 */

#ifndef STEPPER_CONF_H
#define STEPPER_CONF_H

#include <Arduino.h>

// --- Pin configuration ---
#define STEPPER_STEP_PIN  3
#define STEPPER_DIR_PIN   4
#define STEPPER_EN_PIN    5

// --- TB6600 enable polarity (energizes the coils) ---
// Many TB6600 boards free the motor when ENA is HIGH; flip if yours differs.
#define STEPPER_EN_ENERGIZE  LOW
#define STEPPER_EN_FREE      HIGH

// --- Pulse timing ---
#define STEPPER_PULSE_US     5      // TB6600 min. pulse width ~1.5-2 us

// --- Steps per revolution (must match TB6600 DIP switches) ---
// 200 = full step, 400 = 1/2, 800 = 1/4, 1600 = 1/8, 3200 = 1/16
#define STEPPER_STEPS_PER_REV 1600UL

// --- Default speed: 1000 us per step = 1000 steps/s ---
#define STEPPER_DEFAULT_STEP_US 1000UL

void StepperConf_init(void);
void StepperConf_enable(void);
void StepperConf_disable(void);
void StepperConf_setDirection(bool clockwise);
void StepperConf_setStepInterval(uint32_t intervalUs);
void StepperConf_setSpeedRpm(float rpm);
uint32_t StepperConf_getStepInterval(void);

// Blocking step helper (uses the current direction and speed).
void StepperConf_step(void);
void StepperConf_runSteps(uint32_t steps);

// Non-blocking API: call StepperConf_update() every loop() iteration.
void StepperConf_start(uint32_t steps);   // start a move, current direction
bool StepperConf_update(void);            // true while the move is running
bool StepperConf_isBusy(void);
void StepperConf_stop(void);              // abort the current move

#endif  // STEPPER_CONF_H
