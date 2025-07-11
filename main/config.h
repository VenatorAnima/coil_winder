// main/config.h
#ifndef CONFIG_H
#define CONFIG_H

// --- MECHANICAL CONSTANTS ---
// --- SPINDLE ---
// Lead of trapezoidal screw [mm per start]
constexpr float SCREW_LEAD_MM                   = 2.0f;    // mm per revolution per thread start
// Number of thread starts on the screw
constexpr int   SCREW_STARTS                    = 4;       // 4-start thread → 8 mm/rev total
// Total linear travel per one full revolution [mm]
constexpr float REV_TRAVEL_MM                   = SCREW_LEAD_MM * SCREW_STARTS;  // 8 mm/rev
// Maximum safe length of the spindle's carriage movement [mm]
constexpr float SCREW_LENGTH_MM                 = 100.0f;
// Steps per revolution (full-step)
constexpr int   STEPS_PER_REV_SPINDLE           = 200;
// Microstepping (e.g., 32 = 1/32 microstep)
constexpr int   MICROSTEPS_SPINDLE              = 32;
// Max rpm for spindle motor [RPM]
const int       MAX_RPM_SPINDLE                 = 180;
// Endstop type for axis A: true = NO, false = NC
constexpr bool  ENDSTOP_NORMALLY_OPEN           = true;

// --- FRAME ROTATION ---
// Steps per revolution (full-step)
constexpr int   STEPS_PER_REV_FRAME_ROTATION    = 200;
// Microstepping (e.g., 32 = 1/32 microstep)
constexpr int   MICROSTEPS_FRAME_ROTATION       = 32;
// Max rpm for spindle motor [RPM]
const int       MAX_RPM_FRAME_ROTATION          = 180;


// Default and maximum RPM
constexpr int DEFAULT_RPM          = 60;   // default winding speed

// --- GENERAL ---
// Emergency STOP button type: true = NO, false = NC
constexpr bool STOP_BUTTON_NORMALLY_OPEN = true;

#endif // CONFIG_H
