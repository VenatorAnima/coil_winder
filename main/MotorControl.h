// main/MotorControl.h
#ifndef MOTOR_CONTROL_H
#define MOTOR_CONTROL_H

#include <Arduino.h>

/**
 * @brief Wraps raw DIR/STEP/ENABLE for one stepper in full-step mode.
 */
class StepperDriver {
public:
  /**
   * @param dirPin   DIR
   * @param stepPin  STEP
   * @param enPin    ENABLE (actvie LOW)
   */
  StepperDriver(uint8_t dirPin, uint8_t stepPin, uint8_t enPin);

  // false = CW/right, true = CCW/left
  void setDirection(bool cw);

  /// Generate one microstep pulse (blocking)
  void step();

  /// ENABLE = LOW → motor on
  void enable();

  /// ENABLE = HIGH → motor off
  void disable();

private:
  uint8_t _dirPin;
  uint8_t _stepPin;
  uint8_t _enPin;
};

#endif // MOTOR_CONTROL_H
