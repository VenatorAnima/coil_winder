// main/CommandParser.h
#ifndef COMMAND_PARSER_H
#define COMMAND_PARSER_H

#include <Arduino.h>
#include "config.h"
#include "pins.h"
#include "Spindle.h"
#include "FrameRotation.h"
#include "GCodeParser.h"
#include "CoilParams.h"

extern bool CommandParserErrorAvailable;

void CommandParser_init(Spindle& sp, FrameRotation& fr, GCodeParser& gc, CoilParams& co);

/**
 * @brief Initialize the ASCII command parser.
 * 
 * Configures the protocol‐select pin (ASCII vs. JSON)
 * and loads default coil parameters.
 * Called once in setup().
 */
void CommandParser_begin();

/**
 * @brief Read and execute one line of ASCII command.
 * 
 * Should be called repeatedly in loop().  
 * Returns true if a known command was found and run; 
 * false otherwise (so you can print an error).
 */
bool CommandParser_process();

/**
 * @brief Returns a human‐readable description of the last parse error.
 * 
 * Use after process() returns false and Serial.available() is still true.
 */
String CommandParser_lastError();

#endif // COMMAND_PARSER_H