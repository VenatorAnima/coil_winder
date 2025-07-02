// main/CoilParams.cpp
#include "CoilParams.h"

// --- Handlers ---
const CoilParams::ParamHandler CoilParams::_handlers[] = {
    { "slide_left",     &CoilParams::setPositionLeft    },
    { "slide_right",    &CoilParams::setPositionRight   },
    { "width",          &CoilParams::setWidth           },
    { "turns",          &CoilParams::setTurns           },
    { "diameter",       &CoilParams::setDiameter        },
    { "direction",      &CoilParams::setDirection       },
    // Tutaj kolejne hendlery
};

String centerText(const String& value, int width) {
    // 1) Render the number into a String with the right precision
    // String num = String(value, decimalPlaces);  // e.g. "3.14"
    
    // 2) Compute how many spaces we need
    int len       = value.length();               // actual length of "3.14" == 4
    int totalPad  = width - len;                // e.g. 10 - 4 == 6
    if (totalPad <= 0) return value;              // if too long, just return the number
    
    // 3) Split padding into left/right to center
    int padLeft   = totalPad / 2;               // integer division
    int padRight  = totalPad - padLeft;         // remainder goes to the right
    
    // 4) Build the result
    String out = "";
    for (int i = 0; i < padLeft;  i++) out += ' ';
    out += value;
    for (int i = 0; i < padRight; i++) out += ' ';
    return out;
}

/**
 * Constructor: store references and set defaults.
 */
CoilParams::CoilParams()
    : _positionLeft(0.0f), _positionRight(0.0f), _width(29.5f), _Turns(120),
      _wireDiameter(0.35f), _coilWindingDirection(false), _pitchWinding(0.35f) {}

bool CoilParams::set(const String& key, const String& value) {
    // Przeszukiwanie po wszystkich zarejestrowanych handler-ach
    for (auto& h : _handlers) {
        if (key.equalsIgnoreCase(h.key)) {
            return (this->*(h.fn))(value);
        }
    }
    // Nie znaleziono parametru o takiej nazwie
    return false;
}

void CoilParams::print() {
    Serial.println("");
    Serial.println("");
    Serial.println("Module (coil)");
    Serial.println("");
    Serial.println(centerText("Coil dimensions in [mm]", 56));
    Serial.print("<-");
    Serial.print(centerText("(slide_left)", 16));
    Serial.print("->|<-");
    Serial.print(centerText("(width)", 10));
    Serial.print("->|<-");
    Serial.print(centerText("(slide_right)", 16));
    Serial.println("->");
    Serial.print(centerText(String(_positionLeft, 1), 20));
    Serial.print("|");
    Serial.print(centerText(String(_width, 1), 14));
    Serial.print("|");
    Serial.println(centerText(String(_positionRight, 1), 20));
    Serial.println("");

    Serial.print("Winding wire diameter (diameter): ");
    Serial.print(_wireDiameter, 2);
    Serial.println(" mm");

    Serial.print("Number of turns (turns): ");
    Serial.println(_Turns);

    Serial.print("Winding direction (direction): ");
    if (_coilWindingDirection) {
        Serial.println("CW");
    } else {
        Serial.println("CCW");
    }
}

bool CoilParams::setPositionLeft(const String& val) {
    bool ok = false;
    _positionLeft = toFloatClamped(val, 0.0f, std::numeric_limits<float>::max(), ok);
    return ok;
}

bool CoilParams::setPositionRight(const String& val) {
    bool ok = false;
    _positionRight = toFloatClamped(val, 0.0f, std::numeric_limits<float>::max(), ok);
    return ok;
}

bool CoilParams::setWidth(const String& val) {
    bool ok = false;
    _width = toFloatClamped(val, 0.0f, std::numeric_limits<float>::max(), ok);
    return ok;
}

bool CoilParams::setTurns(const String& val) {
    bool ok = false;
    _Turns = toIntClamped(val, 0, std::numeric_limits<int>::max(), ok);
    return ok;
}

bool CoilParams::setDiameter(const String& val) {
    bool ok = false;
    _wireDiameter = toFloatClamped(val, 0.0f, std::numeric_limits<float>::max(), ok);
    return ok;
}

bool CoilParams::setDirection(const String& val) {
    if (val == "CW") {
        _coilWindingDirection = true;
    } else if (val == "CCW") {
        _coilWindingDirection = false;
    } else {
        return false;
    }
    return true;
}

float CoilParams::toFloatClamped(const String& val, float minV, float maxV, bool& ok) {
    float f = val.toFloat();
    ok = !(isnan(f)) && f >= minV && f <= maxV;
    if (f > maxV) {
        f = maxV;
    } else if (f < minV) {
        f = minV;
    }
    return f;
}

int32_t CoilParams::toIntClamped(const String& val, long  minV, long  maxV, bool& ok) {
    int32_t i = val.toInt();
    ok = (i >= minV && i <= maxV);
    if (i >  maxV) {
        i = maxV;
    } else if (i < minV) {
        i = minV;
    }
    return i;
}
