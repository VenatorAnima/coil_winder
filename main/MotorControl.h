// main/MotorControl.h
#ifndef MOTOR_CONTROL_H
#define MOTOR_CONTROL_H

#include <Arduino.h>

/**
 * @brief Klasa obsługująca pojedynczy silnik krokowy (pełny krok)
 * 
 * Opakowuje bezpośrednie sterowanie pinami DIR/STEP/ENABLE
 * i zapewnia prosty interfejs move(), enable(), disable().
 */
class StepperDriver {
public:
  /**
   * @param dirPin   Numer pinu DIR
   * @param stepPin  Numer pinu STEP
   * @param enPin    Numer pinu ENABLE (aktywny LOW)
   */
  StepperDriver(uint8_t dirPin, uint8_t stepPin, uint8_t enPin);

  // Ustawia kierunek obrotu: false = CW/prawo, true = CCW/lewo
  void setDirection(bool cw);

  // Wykonuje jeden pełny krok (z blokowaniem czasu)
  void step();

  // Włącza silnik (ENABLE = LOW)
  void enable();

  // Wyłącza silnik (ENABLE = HIGH)
  void disable();

private:
  uint8_t _dirPin;
  uint8_t _stepPin;
  uint8_t _enPin;
};

#endif // MOTOR_CONTROL_H
