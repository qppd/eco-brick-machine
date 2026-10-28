/*
 * LOAD_CELL_CONF.cpp
 * HX711 load cell driver implementation for EcoBrickMachine.
 *
 * Uses bit-banged SPI to read the HX711 ADC.
 * Compatible with 1kg load cells (typically 2mV/V sensitivity).
 */

#include "LOAD_CELL_CONF.h"

void LoadCellConf::begin(void) {
  pinMode(LOAD_CELL_SCK_PIN, OUTPUT);
  pinMode(LOAD_CELL_DOUT_PIN, INPUT);

  digitalWrite(LOAD_CELL_SCK_PIN, LOW);

  // Wait for HX711 to power up and stabilize
  delay(100);
  calibrateOffset();
}

void LoadCellConf::calibrateOffset(void) {
  // Read multiple samples with no load to establish baseline
  sum = 0.0f;
  sampleCount = 0;
  for (uint8_t i = 0; i < LOAD_CELL_AVG_SAMPLES * 3; i++) {
    rawValue = 0;
    digitalWrite(LOAD_CELL_SCK_PIN, HIGH);
    delayMicroseconds(1);
    digitalWrite(LOAD_CELL_SCK_PIN, LOW);
    delayMicroseconds(1);

    // Read 24 bits
    long value = 0;
    for (int8_t b = 0; b < 24; b++) {
      digitalWrite(LOAD_CELL_SCK_PIN, HIGH);
      delayMicroseconds(1);
      if (digitalRead(LOAD_CELL_DOUT_PIN)) value |= (1L << (23 - b));
      digitalWrite(LOAD_CELL_SCK_PIN, LOW);
      delayMicroseconds(1);
    }

    // Sign extend for 24-bit signed number
    if (value & (1L << 23)) value |= 0xFF000000;

    sum += (float)value;
    sampleCount++;
  }

  // Store average as offset (tare)
  offset_ = sum / (float)sampleCount;
  // Convert to grams using scale
  offset_ = offset_ / scale_;
}

void LoadCellConf::setScale(float gramsPerUnit) {
  scale_ = gramsPerUnit;
}

float LoadCellConf::getGrams(void) {
  uint32_t now = millis();
  if (now - lastReadMs < LOAD_CELL_POLL_MS) return -1.0f; // too soon
  lastReadMs = now;

  // Read raw ADC value
  long reading = 0;
  digitalWrite(LOAD_CELL_SCK_PIN, HIGH);
  delayMicroseconds(1);
  digitalWrite(LOAD_CELL_SCK_PIN, LOW);
  delayMicroseconds(1);

  for (int8_t b = 0; b < 24; b++) {
    digitalWrite(LOAD_CELL_SCK_PIN, HIGH);
    delayMicroseconds(1);
    if (digitalRead(LOAD_CELL_DOUT_PIN)) reading |= (1L << (23 - b));
    digitalWrite(LOAD_CELL_SCK_PIN, LOW);
    delayMicroseconds(1);
  }
  if (reading & (1L << 23)) reading |= 0xFF000000;

  // Convert to grams
  float grams = ((float)reading - offset_) / scale_;

  // Apply moving average for stability
  sum = sum - (sum / (float)LOAD_CELL_AVG_SAMPLES) + grams;
  sampleCount++;

  return sum / (float)LOAD_CELL_AVG_SAMPLES;
}

bool LoadCellConf::isFilled(void) {
  float weight = getGrams();
  return (weight >= FILL_WEIGHT_THRESHOLD_G);
}

void LoadCellConf::printCalibration(void) {
  Serial.println(F("=== Load Cell Calibration ==="));
  Serial.print(F("Offset (raw): "));
  Serial.println(offset_);
  Serial.print(F("Scale: "));
  Serial.println(scale_, 4);
  Serial.print(F("Threshold: "));
  Serial.print(FILL_WEIGHT_THRESHOLD_G, 1);
  Serial.println(F("g"));
  Serial.println(F("Instructions:"));
  Serial.println(F("1. Ensure load cell is EMPTY (no bottle)"));
  Serial.println(F("2. Call calibrateOffset() to set tare"));
  Serial.println(F("3. Place known weight (e.g., 100g)"));
  Serial.println(F("4. Call setScale(100.0f) to calibrate"));
  Serial.println(F("============================"));
}
