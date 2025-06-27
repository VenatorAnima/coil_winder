// main/CommandParser.cpp
#include "CommandParser.h"

// -----------------------------------------------------------------------------
// Internal data
// -----------------------------------------------------------------------------

/// Holds current winding parameters (width, pitch, turns, etc).
// static CoilParams params;

/// Last error string when parsing failed.
static String    lastError;

/// Type alias for a command handler function.
using CmdHandler = bool(*)(const String& args);

/// Forward declarations of individual handlers
static bool cmdHelp       (const String&);
static bool cmdShow       (const String&);
static bool cmdSet        (const String&);
static bool cmdRotate     (const String&);
static bool cmdTurn       (const String&);
static bool cmdHome       (const String&);

/// Table of ASCII commands and their handlers.
/// Add new lines here to easily extend later.
static const struct {
    const char*    name;
    CmdHandler     handler;
    const char*    usage;
} commandTable[] = {
    {"help",   cmdHelp,   "help"},
    {"show",   cmdShow,   "show"},
    {"set",    cmdSet,    "set <param> <value>"},
    {"rotate", cmdRotate, "rotate <deg>"},
    {"turn",   cmdTurn,   "turn <revolutions>"},
    {"home",   cmdHome,   "home"},
};

static constexpr size_t COMMAND_COUNT = sizeof(commandTable) / sizeof(commandTable[0]);

// -----------------------------------------------------------------------------
// Public API
// -----------------------------------------------------------------------------

void CommandParser_begin() {
    // Protocol pin tells us ASCII vs JSON; we leave JSON for Sprint 7.
    pinMode(PROTOCOL_SELECT_PIN, INPUT_PULLUP);

    // Load defaults from your CoilParams module.
//   params = CoilParams_default();

    Serial.println(F("PARSER READY"));
}

bool CommandParser_process() {
    if (! Serial.available()) 
        return false;

    String line = Serial.readStringUntil('\n');
    line.trim();
    if (line.length() == 0) 
        return false;

    // Split command name and args
    int sp = line.indexOf(' ');
    String cmd = (sp < 0 ? line : line.substring(0, sp));
    String args = (sp < 0 ? ""   : line.substring(sp + 1));

    // Lookup in our table
    for (size_t i = 0; i < COMMAND_COUNT; ++i) {
        if (cmd.equalsIgnoreCase(commandTable[i].name)) {
            bool ok = commandTable[i].handler(args);
            if (! ok && lastError.length() == 0) {
                lastError = String("Usage: ") + commandTable[i].usage;
            }
            return ok;
        }
    }

    lastError = "Unknown command";
    return false;
}

String CommandParser_lastError() {
    return lastError;
}

// -----------------------------------------------------------------------------
// Command implementations
// -----------------------------------------------------------------------------

static bool cmdHelp(const String&) {
    Serial.println(F("Available commands:"));
    for (size_t i = 0; i < COMMAND_COUNT; ++i) {
        Serial.print("  ");
        Serial.println(commandTable[i].usage);
    }
    return true;
}

static bool cmdShow(const String&) {
    // Delegates to your CoilParams::print()
    // params.print();
    return true;
}

static bool cmdSet(const String& args) {
    // Expect "<param> <value>"
    int sp = args.indexOf(' ');
    if (sp < 0) {
        lastError = "Bad syntax";
        return false;
    }
    String key   = args.substring(0, sp);
    String value = args.substring(sp + 1);

    // Try to set; returns false if name unknown or value invalid
    // if (! params.set(key, value)) {
    //     lastError = "Invalid param or value";
    //     return false;
    // }
    Serial.print("OK ");
    // Serial.println(key);
    return true;
}

static bool cmdRotate(const String& args) {
    int deg = args.toInt();
    // Validate range [0..360]
    if (deg < 0 || deg > 360) {
        lastError = "Angle out of range";
        return false;
    }

    // Compute the number of microsteps
    // uint32_t steps = (uint32_t)((float)deg / 360.0f * (STEPS_PER_REV * MICROSTEPS) + 0.5f);

    // Run motor B at 10 RPM (preset)
    // motorB.enable();
    // motorB.setDirection(true);  // CW
    // for (uint32_t i = 0; i < steps; ++i) {
        // motorB.step();
    // }
    // motorB.disable();

    Serial.println(F("ROTATE OK"));
    return true;
}

static bool cmdTurn(const String& args) {
    int rev = args.toInt();
    if (rev < 0) {
        lastError = "Revolutions must be ≥0";
        return false;
    }

    // uint32_t totalSteps = rev * (STEPS_PER_REV * MICROSTEPS);

    // Run motor B at 6 RPM
    // motorB.enable();
    // motorB.setDirection(true);  // CW
    // for (uint32_t i = 0; i < totalSteps; ++i) {
    //     motorB.step();
    // }
    // motorB.disable();

    Serial.println(F("TURN OK"));
    return true;
}

static bool cmdHome(const String&) {
    // homeSpindle();
    Serial.println(F("HOMING OK"));
    return true;
}
