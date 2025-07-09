// main/FrameRotation.h
#ifndef FRAME_ROTATION_H
#define FRAME_ROTATION_H

#include <Arduino.h>
#include <stdint.h>
#include <math.h>
#include "pins.h"
#include "config.h"
#include "InputManager.h"

//Timer PWM
#define PIOX            PIOC
#define PIO             PIO_PC24
// #define TC          TC2
#define PWM_CH          7
#define ID_PWM_PERIPH   ID_PWM

extern int32_t currentPositionFrameRotation;
extern int32_t targetPositionFrameRotation;

extern volatile bool frameRotationWork;

// --- CONSTANTS ---
// The minimum period that is safe for maintaining proper operation [us]
constexpr uint32_t MIN_PERIOD_FRAME_ROTATION = (1 / ((MAX_RPM_FRAME_ROTATION * STEPS_PER_REV_FRAME_ROTATION * MICROSTEPS_FRAME_ROTATION) / (60 * 1000000.0)));
// Pulse length time set to half the minimum signal period [us]
constexpr uint32_t HIGH_PULSE_TIME_FRAME_ROTATION = MIN_PERIOD_FRAME_ROTATION / 2;
// Maximum speed of the spindle carriage [obr/min]
constexpr float MAX_SPEED_FRAME_ROTATION = ((1000000.0f / MIN_PERIOD_FRAME_ROTATION) / (STEPS_PER_REV_FRAME_ROTATION * MICROSTEPS_FRAME_ROTATION)) * 60;
// Minimum speed of the spindle carriage [obr/min]
constexpr float MIN_SPEED_FRAME_ROTATION = 10;

/**
 * @brief Wraps raw DIR/ENABLE for frame rotation.
 */
class FrameRotation {
public:
    /**
    * @param dirPin      DIR
    * @param stepPin     STEP (Pin D5/PC25/TIOA6)
    * @param enPin       ENABLE (actvie LOW)
    */
    FrameRotation(uint8_t dirPin, uint8_t stepPin, uint8_t enPin);

    // false = CW, true = CCW (do weryfikacji!?)  
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

    /// @brief Ustaw predkosc obrotowa w obr/min
    /// @param speed Predkosc w obr/min
    /// @return Prawda jesli udalo sie ustwic zadana predkosc, falsz jesli sie nie udalo
    bool setSpeed(float speed);

    /// @brief Ustaw pozycje wzgledem pozycji 0
    /// @param position Pozycja wzgledem pozycji 0 obr
    void setAbsolutePosition(float position);

    /// @brief Ustaw pozycje wzgledem obecnej pozycji
    /// @param position Wartosc o ile przesunac wzgledem obecnej pozycji w obr
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

  int32_t _maxPosition; // To nie potrzebne

  uint32_t _KFreq;

  void configureTimerPWM();

  float calculatePeriod(float speed);

  void setPeriod(float period);

  /// @brief Zwroc liczbe do rejestru zegara RA/RC
  /// @param time punkt do jakiego w czasie ma byc ustawiony zegar [us]
  /// @return zwraca jaka powinna byc wartosc rejestru RA/RC
  uint32_t calculateTicks(float time);

  /// @brief Oblicza pozycje z obr na pozycje w pulsach
  /// @param position dlugosc/pozycja [obr]
  /// @return dlugosc/pozycje w pulsach
  int32_t calculateTickPosition(float position);

  float calculatePosition(int32_t ticks);
};

void PWM_Handler();

#endif // FRAME_ROTATION_H