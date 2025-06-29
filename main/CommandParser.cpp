// main/CommandParser.cpp
#include "CommandParser.h"

// -----------------------------------------------------------------------------
// Internal data
// -----------------------------------------------------------------------------

/// Holds current winding parameters (width, pitch, turns, etc).
// static CoilParams params;

/// Last error string when parsing failed.
static String    lastError;
bool CommandParserErrorAvailable = LOW;

static Spindle*         _spindle    = nullptr;
static FrameRotation*   _frame      = nullptr;

/// Type alias for a command handler function.
using CmdHandler = bool(*)(const String& args);

/// Forward declarations of individual handlers
static bool cmdHelp         (const String&);
static bool cmdShow         (const String&);
static bool cmdSet          (const String&);
static bool cmdRotate       (const String&);
static bool cmdTurn         (const String&);
static bool cmdHome         (const String&);
static bool cmdSpeed        (const String&);
static bool cmdMove         (const String&);

/// Table of ASCII commands and their handlers.
/// Add new lines here to easily extend later.
static const struct {
    const char*    name;
    CmdHandler     handler;
    const char*    usage;
} commandTable[] = {
    {"help",    cmdHelp,    "help"},
    {"show",    cmdShow,    "show"},
    {"set",     cmdSet,     "set <param> <value>"},
    {"rotate",  cmdRotate,  "rotate <deg>"},
    {"turn",    cmdTurn,    "turn <revolutions>"},
    {"home",    cmdHome,    "home"},
    {"speed",   cmdSpeed,   "speed <frame/spindle> <value>"},
    {"move",    cmdMove,    "move 0.1"}
};

static constexpr size_t COMMAND_COUNT = sizeof(commandTable) / sizeof(commandTable[0]);

// -----------------------------------------------------------------------------
// Public API
// -----------------------------------------------------------------------------
void CommandParser_init(Spindle& sp, FrameRotation& fr) {
    _spindle = &sp;
    _frame = &fr;
}

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
                CommandParserErrorAvailable = HIGH;
            }
            return ok;
        }
    }

    lastError = "Unknown command";
    CommandParserErrorAvailable = HIGH;
    return false;
}

String CommandParser_lastError() {
    CommandParserErrorAvailable = LOW;
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
        CommandParserErrorAvailable = HIGH;
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
    int deg = args.toFloat();
    // Validate range [-360..360]
    if (deg < -360.0 || deg > 360.0) {
        lastError = "Angle out of range";
        CommandParserErrorAvailable = HIGH;
        return false;
    }
    _frame->setRelativePosition(deg / 360.0);
    _frame->run();

    Serial.println(F("ROTATE START"));
    return true;
}

static bool cmdTurn(const String& args) {
    int rev = args.toInt();
    // if (rev < 0) {
    //     lastError = "Revolutions must be ≥0";
    //     CommandParserErrorAvailable = HIGH;
    //     return false;
    // }

    _frame->setRelativePosition(rev);
    _frame->run();

    Serial.println(F("TURN START"));
    return true;
}

static bool cmdHome(const String&) {
    _spindle->home();
    Serial.println(F("HOMING OK"));
    return true;
}

static bool cmdSpeed(const String& args) {
    // Expect "<param> <value>"
    int sp = args.indexOf(' ');
    if (sp < 0) {
        lastError = "Bad syntax";
        CommandParserErrorAvailable = HIGH;
        return false;
    }
    String key   = args.substring(0, sp);
    String value = args.substring(sp + 1);

    float speed = value.toFloat();

    if (key == "spindle")
    {
        if (!_spindle->setSpeed(speed)) {
            lastError = "Speed too high or too low";
            CommandParserErrorAvailable = HIGH;
            return false;
        }
    }
    else if (key == "frame")
    {
        if (!_frame->setSpeed(speed)) {
            lastError = "Speed too high or too low";
            CommandParserErrorAvailable = HIGH;
            return false;
        }
    }
    else 
    {
        lastError = "Bad syntax";
        CommandParserErrorAvailable = HIGH;
        return false;
    }
    
    Serial.println(F("SPEED SET"));
    return true;
}

static bool cmdMove(const String& args) {
    float distance = args.toFloat();
    // if (rev < 0) {
    //     lastError = "Revolutions must be ≥0";
    //     CommandParserErrorAvailable = HIGH;
    //     return false;
    // }

    _spindle->setRelativePosition(distance);
    _spindle->run();

    Serial.println(F("MOVE START"));
    return true;
}