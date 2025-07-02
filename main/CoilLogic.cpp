// main/CoilLogic.cpp
#include "CoilLogic.h"

// --- Handlers ---
const CoilLogic::ParamHandler CoilLogic::_handlers[] = {
    { "turns_begin",        &CoilLogic::setTurnsBegin   },
    { "turns_slow",         &CoilLogic::setTurnsSlow    },
    { "speed_begin",        &CoilLogic::setSpeedBegin   },
    { "speed_slow",         &CoilLogic::setSpeedSlow    },
    { "speed_work",         &CoilLogic::setSpeedWork    },
    { "side",               &CoilLogic::setSide         },
    { "end_in_start_point", &CoilLogic::setEndPoint     },
    // Tutaj kolejne hendlery
};


/**
 * Constructor: store references and set defaults.
 */
CoilLogic::CoilLogic()
    : _turnsBegin(2), _turnsSlow(3), _speedBegin(10.0f), _speedSlow(60.0f), _speedWork(160.0f),
      _windFromTheLeft(true), _endAtStartingPoint(true) {}

bool CoilLogic::set(const String& key, const String& value) {
    // Przeszukiwanie po wszystkich zarejestrowanych handler-ach
    for (auto& h : _handlers) {
        if (key.equalsIgnoreCase(h.key)) {
            return (this->*(h.fn))(value);
        }
    }
    // Nie znaleziono parametru o takiej nazwie
    return false;
}

void CoilLogic::print() {
    Serial.println("");
    Serial.println("");
    Serial.println("Module (logic)");
    Serial.println("");

    Serial.print("Number of starting turns (turns_begin): ");
    Serial.println(_turnsBegin);

    Serial.print("Number of turns at low speed (turns_slow): ");
    Serial.println(_turnsSlow);

    Serial.print("Initial winding speed, spindle not moving (speed_begin): ");
    Serial.print(_speedBegin, 2);
    Serial.println(" RPM");

    Serial.print("Initial winding speed, spindle starts moving (speed_slow): ");
    Serial.print(_speedSlow, 2);
    Serial.println(" RPM");

    Serial.print("Operating speed (speed_work): ");
    Serial.print(_speedWork, 2);
    Serial.println(" RPM");

    Serial.print("Winding start direction (side): ");
    if (_windFromTheLeft) {
        Serial.println("left");
    } else {
        Serial.println("right");
    }

    Serial.print("Always end winding from the winding start side (end_in_start_point): ");
    if (_endAtStartingPoint) {
        Serial.println("yes");
    } else {
        Serial.println("no");
    }
}

bool CoilLogic::setTurnsBegin(const String& val) {
    bool ok = false;
    _turnsBegin = toIntClamped(val, 0, std::numeric_limits<int>::max(), ok);
    return ok;
}

bool CoilLogic::setTurnsSlow(const String& val) {
    bool ok = false;
    _turnsSlow = toIntClamped(val, 0, std::numeric_limits<int>::max(), ok);
    return ok;
}

bool CoilLogic::setSpeedBegin(const String& val) {
    bool ok = false;
    _speedBegin = toFloatClamped(val, MIN_SPEED_FRAME_ROTATION, MAX_SPEED_FRAME_ROTATION, ok);
    return ok;
}

bool CoilLogic::setSpeedSlow(const String& val) {
    bool ok = false;
    _speedSlow = toFloatClamped(val, MIN_SPEED_FRAME_ROTATION, MAX_SPEED_FRAME_ROTATION, ok);
    return ok;
}

bool CoilLogic::setSpeedWork(const String& val) {
    bool ok = false;
    _speedWork = toFloatClamped(val, MIN_SPEED_FRAME_ROTATION, MAX_SPEED_FRAME_ROTATION, ok);
    return ok;
}

bool CoilLogic::setSide(const String& val) {
    if (val == "left") {
        _windFromTheLeft = true;
    } else if (val == "right") {
        _windFromTheLeft = false;
    } else {
        return false;
    }
    return true;
}

bool CoilLogic::setEndPoint(const String& val) {
    if (val == "yes") {
        _endAtStartingPoint = true;
    } else if (val == "no") {
        _endAtStartingPoint = false;
    } else {
        return false;
    }
    return true;
}

float CoilLogic::toFloatClamped(const String& val, float minV, float maxV, bool& ok) {
    float f = val.toFloat();
    ok = !(isnan(f)) && f >= minV && f <= maxV;
    if (f > maxV) {
        f = maxV;
    } else if (f < minV) {
        f = minV;
    }
    return f;
}

int32_t CoilLogic::toIntClamped(const String& val, long  minV, long  maxV, bool& ok) {
    int32_t i = val.toInt();
    ok = (i >= minV && i <= maxV);
    if (i >  maxV) {
        i = maxV;
    } else if (i < minV) {
        i = minV;
    }
    return i;
}
