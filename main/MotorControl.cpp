// main/MotorControl.cpp
#include "MotorControl.h"

// Store pins, configure OUTPUT, and disable motor by default
StepperDriver::StepperDriver(uint8_t dirPin, uint8_t stepPin, uint8_t enPin)
  : _dirPin(dirPin), _stepPin(stepPin), _enPin(enPin)
{
  pinMode(_dirPin, OUTPUT);
  pinMode(_stepPin, OUTPUT);
  pinMode(_enPin, OUTPUT);
  disable();  // domyślnie silnik wyłączony
}

void StepperDriver::setDirection(bool cw) {
  // HIGH = CCW, LOW = CW
  digitalWrite(_dirPin, cw ? HIGH : LOW);
}

void StepperDriver::step() {
  // Generate one pulse: 31 µs HIGH + 31 µs LOW → ~62 µs period
  digitalWrite(_stepPin, HIGH);
  delayMicroseconds(26);
  digitalWrite(_stepPin, LOW);
  delayMicroseconds(26);
}

void StepperDriver::enable() {
  digitalWrite(_enPin, LOW);
}

void StepperDriver::disable() {
  digitalWrite(_enPin, HIGH);
}
