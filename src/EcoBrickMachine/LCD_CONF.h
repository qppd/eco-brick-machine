/*
 * LCD_CONF.h
 * I2C 16x2 LCD status display for EcoBrickMachine.
 *
 * Uses the popular LiquidCrystal_I2C library (Frank de Brabander).
 * Shares the I2C bus with the PCA9685 servo driver:
 *   LCD SDA -> GPIO 21, LCD SCL -> GPIO 22
 *   LCD address: 0x27 (typical PCF8574 backpack; change LCD_I2C_ADDR
 *   if yours reports 0x3F).
 *
 * LcdConf_show() only writes to the display when the content changes,
 * so the I2C bus stays quiet during steady-state machine operation.
 */

#ifndef LCD_CONF_H
#define LCD_CONF_H

#include <Arduino.h>

// --- I2C / geometry ---
#define LCD_I2C_ADDR   0x27
#define LCD_COLS       16
#define LCD_ROWS       2

void LcdConf_begin(void);                                 // init + splash
void LcdConf_show(const char* line1, const char* line2);  // change-detected

#endif // LCD_CONF_H
