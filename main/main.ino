// main/main.ino
#include <Arduino.h>
#include "config.h"
#include "pins.h"
#include "MotorControl.h"
#include "InputManager.h"
#include "Spindle.h"
#include "FrameRotation.h"

// Two steppers: A = spindle, B = frame
// StepperDriver motorA(A_DIR_PIN, A_STEP_PIN, A_ENABLE_PIN);
Spindle motorA(A_DIR_PIN, A_STEP_PIN, A_ENABLE_PIN);
FrameRotation motorB(B_DIR_PIN, B_STEP_PIN, B_ENABLE_PIN);

float currentSpeedFrameRotation = 20;

void setup() {
  Serial.begin(115200);
  while (!Serial);

  // init inputs
  Serial.println(F("1."));
  stopButton.begin();
  endstopA.begin();
  pinMode(PROTOCOL_SELECT_PIN, INPUT_PULLUP);

  // motors
  Serial.println(F("2."));
  motorA.enable();
  motorA.home();

  Serial.println(F("3."));
  motorB.enable();

  Serial.println(F("READY"));
  
  // Serial.println(F("=== ENDSTOP TEST ==="));
  // while(true) {
  //   bool raw = digitalRead(ENDSTOP_PIN);
  //   bool state = endstopA.readState();
  //   Serial.print("RAW: "); Serial.print(raw);
  //   Serial.print("  LOGICAL: "); Serial.println(state);
  //   delay(200);
  // }
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
  
  // simple test commands
  if (Serial.available()) {
    String cmd = Serial.readStringUntil('\n');
    cmd.trim();

    if (cmd.equalsIgnoreCase("home")) {
      // homeSpindle();
      motorA.home();
      Serial.println(F("HOMING OK"));
    }
    else if (cmd.equalsIgnoreCase("spPos")) {
      Serial.print(F("SPINDLE POS [mm]: "));
      // Serial.println(getSpindlePosition());
    }
    else if (cmd.equalsIgnoreCase("getCW")) {
      Serial.print(F("FRAME CW COUNT: "));
      // Serial.println(getFrameCWCount());
    }
    else if (cmd.equalsIgnoreCase("getCCW")) {
      Serial.print(F("FRAME CCW COUNT: "));
      // Serial.println(getFrameCCWCount());
    }
    else if (cmd.equalsIgnoreCase("resetCW")) {
      // resetFrameCWCount();
      Serial.println(F("FRAME CW COUNT RESET"));
    }
    else if (cmd.equalsIgnoreCase("resetCCW")) {
      // resetFrameCCWCount();
      Serial.println(F("FRAME CCW COUNT RESET"));
    }

    if (cmd.equalsIgnoreCase("testAL")) {
      motorA.setRelativePosition(-10.0f);
      Serial.println(F("Target_L set"));
    }

    if (cmd.equalsIgnoreCase("testAR")) {
      motorA.setRelativePosition(10.0f);
      Serial.println(F("Target_R set"));
    }
    
    if (cmd.equalsIgnoreCase("testA")) {
      motorA.run();
      Serial.println(F("testA OK"));
    }
    
    if (cmd.equalsIgnoreCase("testB_CW")) {
      motorB.setRelativePosition(10.0f);
      // motorB.setAbsolutePosition(10.0f);
      // targetPositionFrameRotation = 10 * 200 * 32;
      motorB.run();
      Serial.println(F("testB_CW OK"));
    }

    if (cmd.equalsIgnoreCase("testB_CCW")) {
      motorB.setRelativePosition(-10.0f);
      // motorB.setAbsolutePosition(-10.0f);
      motorB.run();
      Serial.println(F("testB_CCW OK"));
    }

    if (cmd.equalsIgnoreCase("testBS_U")) {
      currentSpeedFrameRotation += 10;
      motorB.setSpeed(currentSpeedFrameRotation);
      Serial.println(F("testBS_U OK"));
    }

    if (cmd.equalsIgnoreCase("testBS_D")) {
      currentSpeedFrameRotation -= 10;
      if (currentSpeedFrameRotation <= 0) currentSpeedFrameRotation = 1;
      motorB.setSpeed(currentSpeedFrameRotation);
      Serial.println(F("testBS_D OK"));
    }
  }
}
