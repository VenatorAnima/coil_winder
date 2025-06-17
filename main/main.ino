// main/main.ino
#include <Arduino.h>
#include "config.h"
#include "pins.h"
#include "MotorControl.h"
#include "InputManager.h"
#include "Homing.h"

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

    if (cmd.equalsIgnoreCase("home")) {
      homeSpindle();
      Serial.println(F("HOMING OK"));
    }
    else if (cmd.equalsIgnoreCase("spPos")) {
      Serial.print(F("SPINDLE POS [mm]: "));
      Serial.println(getSpindlePosition());
    }
    else if (cmd.equalsIgnoreCase("getCW")) {
      Serial.print(F("FRAME CW COUNT: "));
      Serial.println(getFrameCWCount());
    }
    else if (cmd.equalsIgnoreCase("getCCW")) {
      Serial.print(F("FRAME CCW COUNT: "));
      Serial.println(getFrameCCWCount());
    }
    else if (cmd.equalsIgnoreCase("resetCW")) {
      resetFrameCWCount();
      Serial.println(F("FRAME CW COUNT RESET"));
    }
    else if (cmd.equalsIgnoreCase("resetCCW")) {
      resetFrameCCWCount();
      Serial.println(F("FRAME CCW COUNT RESET"));
    }

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
