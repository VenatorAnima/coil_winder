// main/main.ino
#include <Arduino.h>
#include "config.h"
#include "pins.h"
#include "MotorControl.h"
#include "InputManager.h"

// Two steppers: A = spindle, B = frame
StepperDriver motorA(A_DIR_PIN, A_STEP_PIN, A_ENABLE_PIN);
StepperDriver motorB(B_DIR_PIN, B_STEP_PIN, B_ENABLE_PIN);

void setup() {
  Serial.begin(115200);
  while (!Serial);

  // init inputs
  stopButton.begin();
  endstopA.begin();
  pinMode(PROTOCOL_SELECT_PIN, INPUT_PULLUP);

  // disable both motors
  motorA.disable();
  motorB.disable();

  Serial.println(F("READY"));
}

void loop() {
  // Emergency-stop handling
  if (stopButton.readState()) {
    motorA.disable();
    motorB.disable();
    Serial.println(F("EMERGENCY STOP"));
    // wait until button released
    while (stopButton.readState());
    Serial.println(F("RESUME READY"));
  }
  
  // simple test commands
  if (Serial.available()) {
    String cmd = Serial.readStringUntil('\n');
    cmd.trim();

    if (cmd.equalsIgnoreCase("testA_L")) {
      motorA.enable();
      motorA.setDirection(true);
      for (int i = 0; i < (STEPS_PER_REV*MICROSTEPS/8*10); ++i) {
        motorA.step();
      };
      motorA.disable();
      Serial.println(F("testA_L OK"));
    }
    
    if (cmd.equalsIgnoreCase("testA_R")) {
      motorA.enable();
      motorA.setDirection(false);
      for (int i = 0; i < (STEPS_PER_REV*MICROSTEPS/8*10); ++i) {
        motorA.step();
      };
      motorA.disable();
      Serial.println(F("testA_R OK"));
    }
    
    if (cmd.equalsIgnoreCase("testB_CW")) {
      motorB.enable();
      motorB.setDirection(false);
      for (int i = 0; i < (50*STEPS_PER_REV*MICROSTEPS); ++i) {
        motorB.step();
      };
      motorB.disable();
      Serial.println(F("testB_CW OK"));
    }

    if (cmd.equalsIgnoreCase("testB_CCW")) {
      motorB.enable();
      motorB.setDirection(true);
      for (int i = 0; i < (50*STEPS_PER_REV*MICROSTEPS); ++i) {
        motorB.step();
      };
      motorB.disable();
      Serial.println(F("testB_CCW OK"));
    }
  }
}
