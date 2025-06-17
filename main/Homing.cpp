#include "Homing.h"
#include "config.h"
#include "pins.h"
#include "MotorControl.h"
#include "InputManager.h"

// Globals
float        spindlePositionMm = 0.0f;
volatile uint32_t frameCWCount  = 0;
volatile uint32_t frameCCWCount = 0;

// Calculate mm per microstep
constexpr float MM_PER_STEP = REV_TRAVEL_MM / (STEPS_PER_REV * MICROSTEPS);

/**
 * @brief Convert mm to nearest microsteps.
 */
uint32_t mmToSteps(float mm) {
  return static_cast<uint32_t>(mm / MM_PER_STEP + 0.5f);
}

/**
 * @brief Compute delay [µs] between microsteps for target speed.
 */
uint32_t speedToDelayUs(float speed_mm_s) {
  // steps/sec = speed_mm_s / MM_PER_STEP
  // period_us  = 1e6 / steps/sec
  return static_cast<uint32_t>(1e6 * MM_PER_STEP / speed_mm_s + 0.5f);
}

/**
 * @brief Helper: step a given distance [mm] at speed [mm/s].
 */
static void stepDistance(float mm, float speed_mm_s) {
  uint32_t steps  = mmToSteps(mm);
  uint32_t delayUs= speedToDelayUs(speed_mm_s);
  motorA.enable();
  for (uint32_t i = 0; i < steps; ++i) {
    motorA.step();
    delayMicroseconds(delayUs);
  }
  motorA.disable();
}

/**
 * @brief Helper: step until endstopA is hit, at speed [mm/s].
 */
static void stepUntilEndstop(float speed_mm_s) {
  uint32_t delayUs = speedToDelayUs(speed_mm_s);
  motorA.enable();
  while (!endstopA.rawState()) {
    motorA.step();
    delayMicroseconds(delayUs);
  }
  motorA.disable();
}

void homeSpindle() {
  // 1) Initial check
  
  bool alreadyPressed = endstopA.readState();
  // Serial.print("Variant: ");
  // Serial.println(alreadyPressed ? "1 (pressed)" : "2 (not)");

  if (alreadyPressed) {
    // Variant 1: already touching
    motorA.setDirection(false);     // CW
    stepDistance(4.0f, 2.0f);       // CW ~4 mm @ 2 mm/s
  } else {
    // Variant 2: not touching
    motorA.setDirection(true);      // CCW
    stepUntilEndstop(2.0f);         // CCW until hit @ 2 mm/s
    motorA.setDirection(false);     // CW
    stepDistance(4.0f, 2.0f);       // CW ~4 mm @ 2 mm/s
  }
  // *** comon ***
  motorA.setDirection(true);     // CCW
  stepUntilEndstop(1.0f);         // CCW until hit @ 1 mm/s

  // 2) Soft-offset 1 mm CW → position zero
  motorA.setDirection(false);     // CW
  stepDistance(1.0f, 1.0f);
  spindlePositionMm = 0.0f;
}

/** Accessors for position & counters **/
float    getSpindlePosition()      { return spindlePositionMm; }
uint32_t getFrameCWCount()        { return frameCWCount; }
uint32_t getFrameCCWCount()       { return frameCCWCount; }
void     resetFrameCWCount()      { frameCWCount = 0; }
void     resetFrameCCWCount()     { frameCCWCount = 0; }