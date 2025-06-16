// main/main.ino
#include <Arduino.h>
#include "config.h"
#include "pins.h"
#include "MotorControl.h"

// Globalny obiekt dla silnika A
StepperDriver motorA(A_DIR_PIN, A_STEP_PIN, A_ENABLE_PIN);

// Sprawdza stan krańcówki z uwzględnieniem NO/NC
bool readEndstop() {
  bool raw = digitalRead(ENDSTOP_PIN);
  // Jeśli krańcówka NO, to HIGH = niezwarty; dla NC odwrotnie.
  return ENDSTOP_NORMALLY_OPEN ? !raw : raw;
}

void setup() {
  // Uruchomienie komunikacji szeregowej
  Serial.begin(115200);
  while (!Serial) { /* czekaj na otwarcie portu */ }

  // Konfiguracja wejścia krańcówki i selection pinu protokołu
  pinMode(ENDSTOP_PIN, INPUT_PULLUP);
  pinMode(PROTOCOL_SELECT_PIN, INPUT_PULLUP);

  // Włączenie silnika A na potrzeby testu
  motorA.enable();

  Serial.println(F("READY"));  // informacja o gotowości
}

void loop() {
  // Prosty test: jeśli przyjdzie "testA", wykonaj 1 obrót silnika A
  if (Serial.available()) {
    String cmd = Serial.readStringUntil('\n');
    cmd.trim();
    if (cmd.equalsIgnoreCase("testA")) {
      // Ustawiamy kierunek CW (=true)
      motorA.setDirection(true);
      // 200 kroków = jeden obrót
      for (int i = 0; i < STEPS_PER_REV; ++i) {
        motorA.step();
      }
      Serial.println(F("testA OK"));
    }
  }
}
