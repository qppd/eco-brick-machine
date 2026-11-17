/*
 * ACTUATOR_CONF.h
 * Linear actuator driver (L293D) for EcoBrickMachine.
 *
 * ONE shared 1000 N, 50 mm linear actuator performs both the pressing
 * and the sealing operation. The L293D is wired with three signals:
 *
 *   L_EN (enable)  -> GPIO 33
 *   LPWM (IN1)     -> GPIO 32
 *   L_IS (IN2)     -> GPIO 25
 *
 * Standard L293D truth table used here:
 *   extend : EN HIGH, IN1 = PWM duty, IN2 LOW
 *   retract: EN HIGH, IN1 LOW,      IN2 = PWM duty
 *   stop   : EN LOW,  IN1 LOW,      IN2 LOW   (outputs floating)
 *
 * The PWM duty (ACT_PWM_DUTY, 0-255 at ACT_PWM_FREQ_HZ) sets the
 * actuator speed; it starts at full duty.
 *
 * Set ACT_INVERT_DIRECTION to 1 if extend/retract are physically swapped.
 *
 * Note: a bare L293D tops out around 600 mA per channel — make sure your
 * driver board / actuator supply matches the actuator's stall current.
 */

#ifndef ACTUATOR_CONF_H
#define ACTUATOR_CONF_H

#include <Arduino.h>

// --- Pin configuration (ESP32 38-pin) ---
#define ACT_EN_PIN    33   // L_EN  -> L293D enable
#define ACT_IN1_PIN   32   // LPWM  -> L293D IN1
#define ACT_IN2_PIN   25   // L_IS  -> L293D IN2

// --- LEDC PWM (IN1 / IN2 carry the PWM) ---
#define ACT_LEDC_CH_IN1   0
#define ACT_LEDC_CH_IN2   1
#define ACT_PWM_FREQ_HZ   20000   // ultrasonic, above audible range
#define ACT_PWM_BITS      8       // duty range 0-255
#define ACT_PWM_DUTY      255     // full speed

// Set to 1 if extend/retract are physically reversed
#define ACT_INVERT_DIRECTION  0

void ActuatorConf_begin(void);       // pins + LEDC, outputs off
void ActuatorConf_extend(void);      // press / move toward the bottle
void ActuatorConf_retract(void);     // return to home position
void ActuatorConf_stop(void);        // both inputs low, enable off

bool ActuatorConf_isExtending(void);
bool ActuatorConf_isRetracting(void);

#endif // ACTUATOR_CONF_H
