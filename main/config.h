// main/config.h
#ifndef CONFIG_H
#define CONFIG_H

// --- MECHANICAL CONSTANTS ---
// Skok trapezowej śruby lead-screw (mm na obrót)
constexpr float SCREW_LEAD_MM = 2.0f;
// Średnica śruby (mm)
constexpr float SCREW_DIAMETER_MM = 8.0f;
// Długość śruby (mm)
constexpr float SCREW_LENGTH_MM = 200.0f;

// Liczba kroków na obrót silnika (pełny krok)
constexpr int STEPS_PER_REV = 200;
// Mikrokrok (np. 16 => 1/16 mikrokrok)
constexpr int MICROSTEPS = 16;

// Domyślne prędkości obrotowe
constexpr int DEFAULT_RPM = 60;        // domyślne RPM w trybie nawijania
// TODO: uzupełnić maksymalne RPM wg. ograniczeń mechanicznych urządzenia
constexpr int MAX_RPM = 100;

// Konfiguracja krańcówki silnika A
// true = NO (normalnie otwarta), false = NC (normalnie zamknięta)
constexpr bool ENDSTOP_NORMALLY_OPEN = true;

// Konfiguracja dla przycisku STOP_BUTTON
// true = NO (normalnie otwarta), false = NC (normalnie zamknięta)
constexpr bool STOP_BUTTON_NORMALLY_OPEN = true;
#endif // CONFIG_H
