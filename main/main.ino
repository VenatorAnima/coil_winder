// main/main.ino
#include <Arduino.h>
#include "config.h"
#include "pins.h"
#include "InputManager.h"
#include "Spindle.h"
#include "FrameRotation.h"
#include "CommandParser.h"
#include "GCodeParser.h"
#include "FrameParams.h"
#include "CoilParams.h"

// Two steppers: A = spindle, B = frame
// StepperDriver motorA(A_DIR_PIN, A_STEP_PIN, A_ENABLE_PIN);
Spindle motorSpindle(A_DIR_PIN, A_STEP_PIN, A_ENABLE_PIN);
FrameRotation motorFrameRotation(B_DIR_PIN, B_STEP_PIN, B_ENABLE_PIN);
GCodeParser gCodeParser(&motorSpindle, &motorFrameRotation);
CoilParams coilParams;
FrameParams frameParams;
CoilLogic coilLogic;

float currentSpeedFrameRotation = 20;

void setup() {
  Serial.begin(115200);
  while (!Serial);

  CommandParser_init(motorSpindle, motorFrameRotation, gCodeParser, coilParams, frameParams, coilLogic);
  gCodeParser.init();

  // init inputs
  stopButton.begin();
  endstopA.begin();

  // motors
  motorSpindle.enable();
  motorSpindle.home();

  motorFrameRotation.enable();
  CommandParser_begin();
}

void loop() {
  // Emergency-stop handling
  // if (stopButton.readState()) {
  //   motorA.disable();
  //   motorB.disable();
  //   Serial.println(F("EMERGENCY STOP"));
  //   // wait until button released
  //   while (stopButton.readState());
  //   Serial.println(F("RESUME READY"));
  // }
  
  // Run one parser cycle; if unknown command, report error
  if (CommandParser_process()) {
    // recognized and executed
  } else if (CommandParserErrorAvailable) {
    Serial.print(F("ERR: "));
    Serial.println(CommandParser_lastError());
  }

}
