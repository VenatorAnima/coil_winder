#ifndef HOMING_H
#define HOMING_H

#include <Arduino.h>
#include "MotorControl.h"

/**
 * @brief Current spindle (axis A) position [mm] since last home.
 */
extern float       spindlePositionMm;

/**
 * @brief Frame (axis B) rotation counters: CW and CCW.
 */
extern volatile uint32_t frameCWCount;
extern volatile uint32_t frameCCWCount;

/**
 * @brief Perform blocking homing of axis A with soft-offset.
 *
 * - If endstop already pressed:  
 *     1. Move CW (right) ~2 mm @ 2 mm/s  
 *     2. Move CCW (left) @ 1 mm/s until endstop  
 * - Else:  
 *     1. Move CCW @ 2 mm/s until endstop  
 *     2. Move CW ~2 mm @ 2 mm/s  
 *     3. Move CCW @ 1 mm/s until endstop  
 * - In both cases:  
 *     • Offset CW 1 mm → set `spindlePositionMm = 0.0f`.  
 */
void      homeSpindle();

/**
 * @brief Convert linear distance [mm] to microsteps.
 */
uint32_t  mmToSteps(float mm);

/**
 * @brief Compute inter-step delay [µs] for given speed [mm/s].
 */
uint32_t  speedToDelayUs(float speed_mm_s);

/**
 * @brief Read spindle position [mm].
 */
float     getSpindlePosition();

/** 
 * @brief Read & reset frame rotation counters.
 */
uint32_t  getFrameCWCount();
uint32_t  getFrameCCWCount();
void      resetFrameCWCount();
void      resetFrameCCWCount();

// Externally visible instances
extern StepperDriver motorA;

#endif // HOMING_H