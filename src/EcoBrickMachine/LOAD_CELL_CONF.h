/*
 * LOAD_CELL_CONF.h
 * HX711 load cell interface for EcoBrickMachine.
 *
 * Detects when a bottle has been filled with sufficient plastic flakes.
 * Used to gate the transition from FEED to PRESS_EXTEND.
 *
 * Wiring (ESP32 38-pin):
 *   HX711_DOUT  -> GPIO 34 (ADC2, safe)
 *   HX711_SCK   -> GPIO 35 (ADC2, safe)
 *   HX711_VCC   -> 5V (or 3.3V, most HX711 boards accept both)
 *   HX711_GND   -> GND
 *
 * Calibration:
 *   1. Call LoadCellConf_calibrate() at startup (no load on cell)
 *   2. Place a known weight, call LoadCellConf_setScale(knownWeightGrams)
 *   3. Use LoadCellConf_getGrams() to read current weight
 */

#ifndef LOAD_CELL_CONF_H
#define LOAD_CELL_CONF_H

#include <Arduino.h>

// --- Pin configuration ---
#define LOAD_CELL_DOUT_PIN  34
#define LOAD_CELL_SCK_PIN   35

// --- Timing ---
#define LOAD_CELL_POLL_MS   100        // minimum ms between readings
#define LOAD_CELL_AVG_SAMPLES 5        // moving average window

// --- Calibration defaults ---
#define LOAD_CELL_DEFAULT_SCALE  100.0f  // grams per ADC unit (tune via calibration)
#define LOAD_CELL_EMPTY_OFFSET   0.0f   // tare weight in grams

// --- Fill threshold ---
#define FILL_WEIGHT_THRESHOLD_G  120.0f // adjust based on trial (empty bottle ~30-40g, target filled ~150-180g)

class LoadCellConf {
public:
  void begin(void);
  void calibrateOffset(void);       // call with empty cell to set tare
  void setScale(float gramsPerUnit); // call after placing known weight

  // Get current weight in grams (with averaging)
  float getGrams(void);

  // Check if bottle meets fill threshold
  bool isFilled(void);

  // Print calibration data to Serial (for setup)
  void printCalibration(void);

private:
  uint32_t lastReadMs = 0;
  float sum = 0.0f;
  uint8_t sampleCount = 0;
  long rawValue = 0;
  float scale_ = LOAD_CELL_DEFAULT_SCALE;
  float offset_ = LOAD_CELL_EMPTY_OFFSET;
};

#endif // LOAD_CELL_CONF_H
