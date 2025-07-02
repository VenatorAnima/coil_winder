// main/CoilLogic.h
#ifndef COIL_LGOIC_H
#define COIL_LGOIC_H

#include <Arduino.h>
#include <limits>
#include "CoilParams.h"
#include "FrameRotation.h"

/**
 * CoilLogic
 */
class CoilLogic {
public:

    CoilLogic();

    /**
     * Set a parameter by name.
     * @param key   Name of the parameter
     * @param value String value to parse and assign
     * @return      true if parameter exists and value is valid
     */
    bool set(const String& key, const String& value);

    void print();


private:
    int _turnsBegin;
    int _turnsSlow;

    float _speedBegin;
    float _speedSlow;
    float _speedWork;

    bool _windFromTheLeft;
    bool _endAtStartingPoint;

    // Individual setters
    bool setTurnsBegin(const String& val);
    bool setTurnsSlow(const String& val);
    bool setSpeedBegin(const String& val);
    bool setSpeedSlow(const String& val);
    bool setSpeedWork(const String& val);
    bool setSide(const String& val);
    bool setEndPoint(const String& val);

    // Helper: trim and parse float/int safely
    float  toFloatClamped(const String& val, float minV, float maxV, bool& ok);
    int32_t   toIntClamped (const String& val, long  minV, long  maxV, bool& ok);

    // Struktura mapująca klucz -> setter
    typedef bool (CoilLogic::*SetterFn)(const String&);
    struct ParamHandler {
        const char*     key;
        SetterFn        fn;
    };
    static const ParamHandler _handlers[];
};

#endif // COIL_LGOIC_H
