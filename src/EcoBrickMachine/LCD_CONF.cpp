/*
 * LCD_CONF.cpp
 * I2C 16x2 LCD implementation for EcoBrickMachine.
 * LiquidCrystal_I2C (Frank de Brabander) on the shared I2C bus.
 */

#include "LCD_CONF.h"
#include <LiquidCrystal_I2C.h>

static LiquidCrystal_I2C lcd(LCD_I2C_ADDR, LCD_COLS, LCD_ROWS);
static char lastLine1[LCD_COLS + 1] = {0};
static char lastLine2[LCD_COLS + 1] = {0};

// Copy into a fixed 16-char buffer, space-padded (clears stale characters)
static void padLine(char* dst, const char* src) {
  uint8_t i = 0;
  if (src != NULL) {
    while (src[i] != '\0' && i < LCD_COLS) {
      dst[i] = src[i];
      i++;
    }
  }
  while (i < LCD_COLS) dst[i++] = ' ';
  dst[LCD_COLS] = '\0';
}

void LcdConf_begin(void) {
  lcd.init();          // initialize the display
  lcd.backlight();     // backlight on

  char b1[LCD_COLS + 1], b2[LCD_COLS + 1];
  padLine(b1, "EcoBrick Machine");
  padLine(b2, "starting...");
  lcd.setCursor(0, 0);
  lcd.print(b1);
  lcd.setCursor(0, 1);
  lcd.print(b2);
  strcpy(lastLine1, b1);
  strcpy(lastLine2, b2);
}

void LcdConf_show(const char* line1, const char* line2) {
  char b1[LCD_COLS + 1], b2[LCD_COLS + 1];
  padLine(b1, line1);
  padLine(b2, line2);

  // Only touch the I2C bus when something actually changed
  if (strcmp(b1, lastLine1) != 0) {
    lcd.setCursor(0, 0);
    lcd.print(b1);
    strcpy(lastLine1, b1);
  }
  if (strcmp(b2, lastLine2) != 0) {
    lcd.setCursor(0, 1);
    lcd.print(b2);
    strcpy(lastLine2, b2);
  }
}
