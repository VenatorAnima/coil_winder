// main/pins.h
#ifndef PINS_H
#define PINS_H

#include <Arduino.h>

// --- PROTOCOL SELECTION PIN ---
// Wysoki stan  -> JSON, niski stan -> ASCII
constexpr uint8_t PROTOCOL_SELECT_PIN = 22;

// --- EMERGENCY STOP ---
constexpr uint8_t STOP_BUTTON_PIN   = 24;

// --- ENDSTOP (krańcówka) for axis A ---
constexpr uint8_t ENDSTOP_PIN        = 26; 

// --- MOTOR A (wrzeciono) DRV8825 pins ---
// Kierunek, krok, enable
constexpr uint8_t A_DIR_PIN   = 4;
constexpr uint8_t A_STEP_PIN  = 5;
constexpr uint8_t A_ENABLE_PIN= 12;

// --- MOTOR B (obrót karkasa) DRV8825 pins ---
constexpr uint8_t B_DIR_PIN   = 7; //7
constexpr uint8_t B_STEP_PIN  = 6; //6
constexpr uint8_t B_ENABLE_PIN= 8; //8

#endif // PINS_H
