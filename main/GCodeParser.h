// GCodeParser.h
#ifndef GCODE_PARSER_H
#define GCODE_PARSER_H

#include <Arduino.h>
#include "Spindle.h"
#include "FrameRotation.h"
#include <math.h>

/**
 * GCodeParser handles parsing and execution of basic G-code commands:
 *  - G0/G1 moves with X (linear) and A (rotational) parameters
 *  - F feed rate for the A axis
 *  - G90/G91 absolute and relative positioning
 *  - M3/M5 motor enable/disable
 *  - G28 homing
 */
class GCodeParser {
public:
    /**
     * Construct a GCodeParser with references to the two motor controllers.
     * @param spindle Pointer to the Spindle motor (linear axis).
     * @param frame Pointer to the FrameRotation motor (rotational axis).
     */
    GCodeParser(Spindle* spindle, FrameRotation* frame);

    /**
     * Initialize internal state. Call once in setup() to reset modes and positions.
     */
    void init();

    /**
     * Parse and execute a single line of G-code.
     * Called for each line of text received.
     * @param line The input G-code string (e.g. "G1 X10 A2 F60").
     */
    void parseLine(const String& line);

private:
    Spindle* spindle_;           // Controller for linear carriage movements
    FrameRotation* frame_;       // Controller for rotational winding movements

    bool absoluteMode_;          // True = absolute coordinates (G90), false = relative (G91)
    float currentX_;             // Last known X position in millimeters
    float currentA_;             // Last known A position in rotations
    float feedRateA_;            // Feed rate for A axis in rotations per minute

    /**
     * Handle G0/G1 moves by updating target positions and driving motors.
     * @param hasX Whether an X parameter was provided.
     * @param xValue The X coordinate or offset.
     * @param hasA Whether an A parameter was provided.
     * @param aValue The A rotation count or offset.
     */
    void handleLinearMove(bool hasX, float xValue, bool hasA, float aValue);

    /**
     * Turn motors on (M3) or off (M5).
     * @param on True to enable (M3), false to disable (M5).
     */
    void handleMotorEnable(bool on);

    /**
     * Switch between absolute (G90) and relative (G91) coordinate modes.
     * @param absolute True for absolute mode, false for relative.
     */
    void setCoordinateMode(bool absolute);

    /**
     * Perform homing (G28) on both axes, resetting positions to zero.
     */
    void handleHoming();
};

#endif // GCODE_PARSER_H
