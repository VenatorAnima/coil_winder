// InputManager.cpp
#include "InputManager.h"

// Constructor: store pin, type, and debounce settings
DebouncedInput::DebouncedInput(uint8_t pin, bool normallyOpen, uint16_t debounceDelay)
    : _pin(pin),
      _normOpen(normallyOpen),
      _debounceDelay(debounceDelay),
      _lastStableState(false),
      _lastReading(true),
      _lastDebounceTime(0) {}

/**
 * @brief Initialize pin and read initial state
 *
 * Configure the pin as INPUT_PULLUP and sample initial reading.
 * Called in setup().
 */
void DebouncedInput::begin() {
    pinMode(_pin, INPUT_PULLUP);
    _lastReading = digitalRead(_pin);
    _lastStableState = computeState(_lastReading);
    _lastDebounceTime = millis();
}

/**
 * @brief Read pin, apply debounce logic, and return stable state
 *
 * Checks for changes in raw input; if stable longer than debounceDelay,
 * updates _lastStableState.
 *
 * @return true if switch activated, false otherwise
 */
bool DebouncedInput::readState() {
    bool reading = digitalRead(_pin);
    if (reading != _lastReading) {
        // raw reading changed: reset debounce timer
        _lastDebounceTime = millis();
        _lastReading = reading;
    }
    if ((millis() - _lastDebounceTime) >= _debounceDelay) {
        // enough time passed: compute new stable state
        bool newState = computeState(reading);
        if (newState != _lastStableState) {
            // state changed after debounce
            _lastStableState = newState;
        }
    }
    return _lastStableState;
}

/**
 * @brief Map raw reading (HIGH/LOW) to logical state based on switch type
 *
 * For NO: LOW (closed) => activated; for NC: HIGH (open) => activated.
 */
bool DebouncedInput::computeState(bool raw) {
    return _normOpen ? !raw : raw;
}

// Instantiate global objects using config constants
DebouncedInput stopButton(STOP_BUTTON_PIN, STOP_BUTTON_NORMALLY_OPEN);
DebouncedInput endstopA(ENDSTOP_PIN, ENDSTOP_NORMALLY_OPEN);
