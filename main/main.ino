// main/main.ino
#include <Arduino.h>
#include "config.h"
#include "pins.h"
#include "MotorControl.h"
#include "InputManager.h"

// Globalny obiekt dla silnika A i B
StepperDriver motorA(A_DIR_PIN, A_STEP_PIN, A_ENABLE_PIN);
StepperDriver motorB(B_DIR_PIN, B_STEP_PIN, B_ENABLE_PIN);

void setup() {
  // Uruchomienie komunikacji szeregowej
  Serial.begin(115200);
  while (!Serial) { /* czekaj na otwarcie portu */ }

  // Konfiguracja wejścia krańcówki i selection pinu protokołu
  stopButton.begin();
  endstopA.begin();
  pinMode(PROTOCOL_SELECT_PIN, INPUT_PULLUP);

  // Wylaczenie silnikow, zeby sie nie grzaly w spoczynku
  motorA.disable();
  motorB.disable();

  Serial.println(F("READY"));  // informacja o gotowości
}

void loop() {
  // Prosty test: jeśli przyjdzie "testA", wykonaj 1 obrót silnika A
  if (Serial.available()) {
    String cmd = Serial.readStringUntil('\n');
    cmd.trim();
    if (cmd.equalsIgnoreCase("testA_left")) {
      // Wlaczenie silnika i ustawienie kierunku obrotu silnka
      motorA.enable();
      motorA.setDirection(true);
      // ruch wrzeciona w lewo o 10mm
      for (int i = 0; i < (STEPS_PER_REV*MICROSTEPS/8*10); ++i) {
        motorA.step();
      };
      motorA.disable();
      Serial.println(F("testA_left OK"));
    }
    
    if (cmd.equalsIgnoreCase("testA_right")) {
      // Wlaczenie silnika i ustawienie kierunku obrotu silnka
      motorA.enable();
      motorA.setDirection(false);
      // ruch wrzeciona w prawo o 10mm
      for (int i = 0; i < (STEPS_PER_REV*MICROSTEPS/8*10); ++i) {
        motorA.step();
      };
      motorA.disable();
      Serial.println(F("testA_right OK"));
    }
    
    if (cmd.equalsIgnoreCase("testB_clockwise")) {
      // Wlaczenie silnika i ustawienie kierunku obrotu silnka
      motorB.enable();
      motorB.setDirection(false);
      // 50 obrotow zgodnie do ruchu wskazowek zegara
      for (int i = 0; i < (50*STEPS_PER_REV*MICROSTEPS); ++i) {
        motorB.step();
        //delay(500);
      };
      motorB.disable();
      Serial.println(F("testB OK"));
    }

    if (cmd.equalsIgnoreCase("testB_counterclockwise")) {
      // Wlaczenie silnika i ustawienie kierunku obrotu silnka
      motorB.enable();
      motorB.setDirection(true);
      // 50 obrotow przeciwnie do ruchu wskazowek zegara
      for (int i = 0; i < (50*STEPS_PER_REV*MICROSTEPS); ++i) {
        motorB.step();
        //delay(500);
      };
      motorB.disable();
      Serial.println(F("testB OK"));
    }
  }
}
