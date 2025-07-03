// main/FrameParams.h
#ifndef FRAME_PARAMS_H
#define FRAME_PARAMS_H

#include <Arduino.h>
#include <limits>
#include "CoilParams.h"

/**
 * FrameParams
 */
class FrameParams {
public:

    FrameParams();

    /**
     * Set a parameter by name.
     * @param key   Name of the parameter
     * @param value String value to parse and assign
     * @return      true if parameter exists and value is valid
     */
    bool set(const String& key, const String& value);

    template<typename T>
    bool get(const String& key, T& out);

    void print();


private:
    float _frameOffset;
    float _width;

    // Individual setters
    bool setFrameOffset(const String& val);
    bool setWidth(const String& val);

    // Helper: trim and parse float/int safely
    float  toFloatClamped(const String& val, float minV, float maxV, bool& ok);
    int32_t   toIntClamped (const String& val, long  minV, long  maxV, bool& ok);

    // Struktura mapująca klucz -> setter
    typedef bool (FrameParams::*SetterFn)(const String&);
    struct ParamHandler {
        const char*     key;
        SetterFn        fn;
    };
    static const ParamHandler _handlers[];
};

#endif // FRAME_PARAMS_H
