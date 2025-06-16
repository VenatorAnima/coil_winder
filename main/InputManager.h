// InputManager.h
#ifndef INPUTMANAGER_H
#define INPUTMANAGER_H

#include <Arduino.h>
#include "config.h"
#include "pins.h"

/**
 * @brief Class for debounced digital inputs (buttons, endstops).
 *
 * Provides stable readings with configurable debounce time and support for
 * normally open (NO) or normally closed (NC) switches.
 */
class DebouncedInput {
public:
    /**
     * @brief Construct a new Debounced Input object
     *
     * @param pin Digital pin number connected to the switch
     * @param normallyOpen true if switch is NO, false if NC
     * @param debounceDelay debounce interval in ms
     */
    DebouncedInput(uint8_t pin, bool normallyOpen, uint16_t debounceDelay = 50);

    /**
     * @brief Initialize the input pin mode and state
     *
     * Call in setup() to configure INPUT_PULLUP and initial stable state.
     */
    void begin();

    /**
     * @brief Read and update the debounced state
     *
     * Should be called frequently (e.g., in loop() or state machine) to sample the input and
     * update the stable state.
     * @return true if the switch is considered "activated" (pressed/triggered)
     */
    bool readState();

private:
    uint8_t _pin;               ///< Arduino pin number for the input
    bool _normOpen;             ///< true=NO, false=NC behavior
    uint16_t _debounceDelay;    ///< debounce interval in milliseconds

    bool _lastStableState;      ///< last debounced state
    bool _lastReading;          ///< last raw reading from digitalRead()
    unsigned long _lastDebounceTime; ///< last time raw reading changed

    /**
     * @brief Convert raw logic level to logical state based on NO/NC
     *
     * @param raw raw digitalRead() value (HIGH/LOW)
     * @return true if activated
     */
    bool computeState(bool raw);
};

// Externally visible instances for STOP button and endstop A
extern DebouncedInput stopButton;
extern DebouncedInput endstopA;

#endif // INPUTMANAGER_H