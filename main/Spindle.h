// main/Spindle.h
#ifndef MOTOR_CONTROL_V2_H
#define MOTOR_CONTROL_V2_H

#include <Arduino.h>
#include <stdint.h>
#include <math.h>
#include "pins.h"
#include "config.h"
#include "InputManager.h"

//Timer
#define PIOX        PIOC
#define PIO         PIO_PC25
#define TC          TC2
#define TC_CH       0
#define ID_TC_CH    ID_TC6

extern uint32_t currentPositionSpindle;
extern uint32_t targetPositionSpindle;

extern volatile bool spindleWork;

// --- CONSTANTS ---
// The minimum period that is safe for maintaining proper operation [us]
constexpr uint32_t MIN_PERIOD = (1 / ((MAX_RPM_SPINDLE * STEPS_PER_REV_SPINDLE * MICROSTEPS_SPINDLE) / (60 * 1000000.0)));
// Pulse length time set to half the minimum signal period [us]
constexpr uint32_t HIGH_PULSE_TIME = MIN_PERIOD / 2;
// Maximum speed of the spindle carriage [mm/s]
constexpr float MAX_SPEED = REV_TRAVEL_MM * ((1000000.0f / MIN_PERIOD) / (STEPS_PER_REV_SPINDLE * MICROSTEPS_SPINDLE));
// Minimum speed of the spindle carriage [mm/s]


/**
 * @brief Wraps raw DIR/ENABLE for spindle.
 */
class Spindle {
public:
  /**
   * @param dirPin      DIR
   * @param stepPin     STEP (Pin D5/PC25/TIOA6)
   * @param enPin       ENABLE (actvie LOW)
   */
  Spindle(uint8_t dirPin, uint8_t stepPin, uint8_t enPin);

  // false = right, true = left (do weryfikacji!?)
  void setDirection(bool cw);

  /// Generate one microstep pulse (blocking) (do usuniecia po obsludze przerwania)
  void step();

  /// ENABLE = LOW → motor on
  void enable();

  /// ENABLE = HIGH → motor off
  void disable();

  /// ENABLE PULSE ON STEP
  void run();

  void brake();

  /// @brief Ustaw cel w krokach
  /// @param target pozycja w krokach
  void setTarget(int32_t target);

  /// @brief Ustaw predkosc wrzeciona w mm/s
  /// @param speed Predkosc w mm/s
  /// @return Prawda jesli udalo sie ustwic zadana predkosc, falsz jesli sie nie udalo
  bool setSpeed(float speed);

  /// @brief Ustaw pozycje wzgledem pozycji 0
  /// @param position Pozycja wrzeciona wzgledem pozycji 0 w mm
  void setAbsolutePosition(float position);

  /// @brief Ustaw pozycje wzgledem obecnej pozycji
  /// @param position Wartosc o ile przesunac wzgledem obecnej pozycji w mm
  void setRelativePosition(float position);

  /// @brief Ustawienie pozycji 0 przy wykorzystaniu krancowki
  void home();

private:
  uint8_t _dirPin;
  uint8_t _stepPin;
  uint8_t _enPin;

  uint32_t _CLK_DIVIDER;
  uint32_t _clkFreq;
  uint32_t _RA_ticks;
  uint32_t _RC_ticks;

  int32_t _maxPosition;

  uint32_t _KFreq;

  void configureTimer();

  float calculatePeriod(float speed);

  void setPeriod(float period);

  /// @brief Zwroc liczbe do rejestru zegara RA/RC
  /// @param time punkt do jakiego w czasie ma byc ustawiony zegar [us]
  /// @return zwraca jaka powinna byc wartosc rejestru RA/RC
  uint32_t calculateTicks(float time);

  /// @brief Oblicza pozycje z mm na pozycje w pulsach
  /// @param position dlugosc/pozycja [mm]
  /// @return dlugosc/pozycje w pulsach
  uint32_t calculateTickPosition(float position);

  float calculatePosition(uint32_t ticks);
};

void TC6_Handler();

#endif // MOTOR_CONTROL_V2_H