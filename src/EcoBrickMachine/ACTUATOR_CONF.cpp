/*
 * ACTUATOR_CONF.cpp
 * L293D linear actuator driver implementation for EcoBrickMachine.
 */

#include "ACTUATOR_CONF.h"

// 0 = stopped, 1 = extending, 2 = retracting
static uint8_t actState = 0;

void ActuatorConf_begin(void) {
  pinMode(ACT_EN_PIN, OUTPUT);
  digitalWrite(ACT_EN_PIN, LOW);

  ledcSetup(ACT_LEDC_CH_IN1, ACT_PWM_FREQ_HZ, ACT_PWM_BITS);
  ledcAttachPin(ACT_IN1_PIN, ACT_LEDC_CH_IN1);
  ledcWrite(ACT_LEDC_CH_IN1, 0);

  ledcSetup(ACT_LEDC_CH_IN2, ACT_PWM_FREQ_HZ, ACT_PWM_BITS);
  ledcAttachPin(ACT_IN2_PIN, ACT_LEDC_CH_IN2);
  ledcWrite(ACT_LEDC_CH_IN2, 0);

  actState = 0;
}

void ActuatorConf_extend(void) {
  digitalWrite(ACT_EN_PIN, HIGH);
#if ACT_INVERT_DIRECTION
  ledcWrite(ACT_LEDC_CH_IN1, 0);
  ledcWrite(ACT_LEDC_CH_IN2, ACT_PWM_DUTY);
  actState = 2;
#else
  ledcWrite(ACT_LEDC_CH_IN1, ACT_PWM_DUTY);
  ledcWrite(ACT_LEDC_CH_IN2, 0);
  actState = 1;
#endif
}

void ActuatorConf_retract(void) {
  digitalWrite(ACT_EN_PIN, HIGH);
#if ACT_INVERT_DIRECTION
  ledcWrite(ACT_LEDC_CH_IN1, ACT_PWM_DUTY);
  ledcWrite(ACT_LEDC_CH_IN2, 0);
  actState = 1;
#else
  ledcWrite(ACT_LEDC_CH_IN1, 0);
  ledcWrite(ACT_LEDC_CH_IN2, ACT_PWM_DUTY);
  actState = 2;
#endif
}

void ActuatorConf_stop(void) {
  ledcWrite(ACT_LEDC_CH_IN1, 0);
  ledcWrite(ACT_LEDC_CH_IN2, 0);
  digitalWrite(ACT_EN_PIN, LOW);
  actState = 0;
}

bool ActuatorConf_isExtending(void) {
  return actState == 1;
}

bool ActuatorConf_isRetracting(void) {
  return actState == 2;
}
