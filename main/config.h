// main/config.h
#ifndef CONFIG_H
#define CONFIG_H

// --- MECHANICAL CONSTANTS ---
// Lead of trapezoidal screw [mm/rev]
constexpr float SCREW_LEAD_MM = 2.0f;
// Screw diameter [mm]
constexpr float SCREW_DIAMETER_MM = 8.0f;
// Screw length [mm]
constexpr float SCREW_LENGTH_MM = 200.0f;

// Steps per revolution (full step)
constexpr int STEPS_PER_REV = 200;
// Microstepping (e.g., 32 = 1/32 microstep)
constexpr int MICROSTEPS = 32;

// Default and maximum RPM
constexpr int DEFAULT_RPM          = 60;   // default winding speed
constexpr int MAX_RPM              = 150;  // measured safe maximum

// Endstop type for axis A: true = NO, false = NC
constexpr bool ENDSTOP_NORMALLY_OPEN     = true;
// Emergency STOP button type: true = NO, false = NC
constexpr bool STOP_BUTTON_NORMALLY_OPEN = true;

#endif // CONFIG_H
