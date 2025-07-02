// main/FrameParams.cpp
#include "FrameParams.h"

// --- Handlers ---
const FrameParams::ParamHandler FrameParams::_handlers[] = {
    { "offset",         &FrameParams::setFrameOffset        },
    { "width",          &FrameParams::setWidth              },
    // Tutaj kolejne hendlery
};


/**
 * Constructor: store references and set defaults.
 */
FrameParams::FrameParams()
    : _frameOffset(2.5f), _width(29.5f) {}

bool FrameParams::set(const String& key, const String& value) {
    // Przeszukiwanie po wszystkich zarejestrowanych handler-ach
    for (auto& h : _handlers) {
        if (key.equalsIgnoreCase(h.key)) {
            return (this->*(h.fn))(value);
        }
    }
    // Nie znaleziono parametru o takiej nazwie
    return false;
}

void FrameParams::print() {
    Serial.println("");
    Serial.println("");
    Serial.println("Module (frame)");
    Serial.println("");
    Serial.println(centerText("Frame dimensions in [mm]", 42));
    Serial.print("Position 0 |<-");
    Serial.print(centerText("(offset)", 10));
    Serial.print("->|<-");
    Serial.print(centerText("(width)", 10));
    Serial.println("->|");

    Serial.print("           |");
    Serial.print(centerText(String(_frameOffset, 2), 14));
    Serial.print("|");
    Serial.print(centerText(String(_width, 2), 14));
    Serial.println("|");
    Serial.println("");
}

bool FrameParams::setFrameOffset(const String& val) {
    bool ok = false;
    _frameOffset = toFloatClamped(val, 0.0f, std::numeric_limits<float>::max(), ok);
    return ok;
}

bool FrameParams::setWidth(const String& val) {
    bool ok = false;
    _width = toFloatClamped(val, 0.0f, std::numeric_limits<float>::max(), ok);
    return ok;
}

float FrameParams::toFloatClamped(const String& val, float minV, float maxV, bool& ok) {
    float f = val.toFloat();
    ok = !(isnan(f)) && f >= minV && f <= maxV;
    if (f > maxV) {
        f = maxV;
    } else if (f < minV) {
        f = minV;
    }
    return f;
}

int32_t FrameParams::toIntClamped(const String& val, long  minV, long  maxV, bool& ok) {
    int32_t i = val.toInt();
    ok = (i >= minV && i <= maxV);
    if (i >  maxV) {
        i = maxV;
    } else if (i < minV) {
        i = minV;
    }
    return i;
}
