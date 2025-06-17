// main/MotorControl.cpp
#include "MotorControl.h"

// Konstruktor: zapis pinów i konfiguracja OUTPUT
StepperDriver::StepperDriver(uint8_t dirPin, uint8_t stepPin, uint8_t enPin)
  : _dirPin(dirPin), _stepPin(stepPin), _enPin(enPin)
{
  pinMode(_dirPin, OUTPUT);
  pinMode(_stepPin, OUTPUT);
  pinMode(_enPin, OUTPUT);
  disable();  // domyślnie silnik wyłączony
}

void StepperDriver::setDirection(bool cw) {
  // HIGH = CW, LOW = CCW
  digitalWrite(_dirPin, cw ? HIGH : LOW);
}

void StepperDriver::step() {
  // Pełny krok: krótkie pulsy na STEP
  digitalWrite(_stepPin, HIGH);
  delayMicroseconds(31);        // czas trwania sygnału (do optymalizacji)
  digitalWrite(_stepPin, LOW);
  delayMicroseconds(31);
}

void StepperDriver::enable() {
  // ENABLE jest aktywny w stanie LOW
  digitalWrite(_enPin, LOW);
}

void StepperDriver::disable() {
  // Wyłączenie silnika
  digitalWrite(_enPin, HIGH);
}
