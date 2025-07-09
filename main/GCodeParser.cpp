// GCodeParser.cpp
#include "GCodeParser.h"

/**
 * Constructor: store references and set defaults.
 */
GCodeParser::GCodeParser(Spindle* spindle, FrameRotation* frame)
    : spindle_(spindle), frame_(frame), absoluteMode_(true),
      currentX_(0.0f), currentA_(0.0f), feedRateA_(0.0f) {}

/**
 * Initialize parser state at startup.
 */
void GCodeParser::init() {
    absoluteMode_ = true;   // Default to absolute (G90)
    currentX_ = 0.0f;       // Reset X position
    currentA_ = 0.0f;       // Reset A rotation count
    feedRateA_ = 0.0f;      // No feed rate yet
}

/**
 * Parse a G-code line and dispatch to specific handlers.
 * Called from the main loop when a full line is received.
 */
void GCodeParser::parseLine(const String& line) {
    String cmd = line;
    cmd.trim();  // Remove leading/trailing whitespace

    // Coordinate mode commands
    if (cmd.startsWith("G90")) { setCoordinateMode(true); return; }
    if (cmd.startsWith("G91")) { setCoordinateMode(false); return; }

    // Homing command
    if (cmd.startsWith("G28")) { handleHoming(); return; }

    // Motor enable/disable commands
    if (cmd.startsWith("M3")) { handleMotorEnable(true); return; }
    if (cmd.startsWith("M5")) { handleMotorEnable(false); return; }

    while (spindleWork || frameRotationWork)
    {
        /* czekaj, aż skonczy poprzedni ruch */
    }

    // Linear moves G0/G1
    if (cmd.startsWith("G0") || cmd.startsWith("G1")) {
        bool hasX = false, hasA = false, modeG1 = false;
        float xVal = 0.0f, aVal = 0.0f;
        if (cmd.startsWith("G1")) {
            modeG1 = true;
        }
        // Scan for parameters X, A, F
        for (int i = 0; i < cmd.length(); ++i) {
            char c = cmd.charAt(i);
            if (c == 'X') {
                hasX = true;
                xVal = cmd.substring(i+1).toFloat(); // Value after X
            } else if (c == 'A') {
                hasA = true;
                aVal = cmd.substring(i+1).toFloat();
            } else if (c == 'F') {
                // F only affects A axis feed rate
                feedRateA_ = cmd.substring(i+1).toFloat();
            }
        }
        handleLinearMove(hasX, xVal, hasA, aVal, modeG1);
    }
}

/**
 * Execute a linear (G0/G1) move on X and/or A axes.
 * Converts coordinate values based on the current mode.
 */
void GCodeParser::handleLinearMove(bool hasX, float xValue, bool hasA, float aValue, bool modeG1) {
    float targetX = currentX_;
    float targetA = currentA_;
    float time = 0.0f;
    float speedSpindle = 5.0f;
    float speedFrame = feedRateA_;
    float timeSpindle = 0.0f;
    float timeFrame = 0.0f;

    if (!modeG1) {
        if (hasX && hasA) {
            timeSpindle = abs(xValue) / MAX_SPEED_SPINDLE;
            timeFrame = abs(aValue) / MAX_SPEED_FRAME_ROTATION * 60;
            if (timeFrame >= timeSpindle) {
                speedSpindle = abs(xValue) / timeFrame;
                speedFrame = MAX_SPEED_FRAME_ROTATION;
            } else {
                speedFrame = abs(aValue) / timeSpindle * 60;
                speedSpindle = MAX_SPEED_SPINDLE;
            }
            targetX = absoluteMode_ ? xValue : currentX_ + xValue;
            targetA = absoluteMode_ ? aValue : currentA_ + aValue;
        } else if (hasX) {
            speedSpindle = MAX_SPEED_SPINDLE;
            targetX = absoluteMode_ ? xValue : currentX_ + xValue;
        } else if (hasA) {
            speedFrame = MAX_SPEED_FRAME_ROTATION;
            targetA = absoluteMode_ ? aValue : currentA_ + aValue;
        }
    } else {
        if (hasX && hasA) {
            timeSpindle = abs(xValue) / MAX_SPEED_SPINDLE;
            timeFrame = abs(aValue) / feedRateA_ * 60;
            if (timeFrame >= timeSpindle) {
                speedSpindle = abs(xValue) / timeFrame;
                speedFrame = feedRateA_;
            } else {
                speedFrame = abs(aValue) / timeSpindle * 60;
                speedSpindle = MAX_SPEED_SPINDLE;
            }
            targetX = absoluteMode_ ? xValue : currentX_ + xValue;
            targetA = absoluteMode_ ? aValue : currentA_ + aValue;
        } else if (hasX) {
            speedSpindle = 5.0f;
            targetX = absoluteMode_ ? xValue : currentX_ + xValue;
        } else if (hasA) {
            speedFrame = feedRateA_;
            targetA = absoluteMode_ ? aValue : currentA_ + aValue;
        }
    }

    // Compute new target positions
    // if (hasX) {
        // Absolute vs. relative computation
    //     targetX = absoluteMode_ ? xValue : currentX_ + xValue;
    // }
    // if (hasA) {
    //     time = abs(aValue) / feedRateA_ * 60;
    //     speedSpindle = abs(xValue) / time;
    //     targetA = absoluteMode_ ? aValue : currentA_ + aValue;
    // }
    

    // Set rotational speed (feedRateA_ is in rotations per minute)
    frame_->setSpeed(speedFrame);
    // Rotate to the computed A target (in rotations)
    frame_->setAbsolutePosition(targetA);

    // Set spindle speed [mm/s];
    spindle_->setSpeed(speedSpindle);
    // Move linear axis to the computed X target (in mm)
    spindle_->setAbsolutePosition(targetX);

    // Run
    frame_->run();
    spindle_->run();

    // Update current state
    currentX_ = targetX;
    currentA_ = targetA;
}

/**
 * Enable or disable both motors (M3/M5).
 */
void GCodeParser::handleMotorEnable(bool on) {
    if (on) {
        spindle_->enable();  // Turn on linear motor driver
        frame_->enable();    // Turn on rotational motor driver
    } else {
        spindle_->disable(); // Turn off linear motor driver
        frame_->disable();   // Turn off rotational motor driver
    }
}

/**
 * Switch between absolute (G90) and relative (G91) positioning.
 */
void GCodeParser::setCoordinateMode(bool absolute) {
    absoluteMode_ = absolute;
}

/**
 * Home both axes to their reference switches (G28).
 * After homing, reset internal position counters to zero.
 */
void GCodeParser::handleHoming() {
    spindle_->home();      // Move linear axis to home switch
    frame_->home();        // Move rotational axis to home switch
    currentX_ = 0.0f;      // Reset logical position
    currentA_ = 0.0f;
}
