// main/CoilParams.h
#ifndef COIL_PARAMS_H
#define COIL_PARAMS_H

#include <Arduino.h>
#include <limits>

/**
 * CoilParams holds all winding parameters and provides
 * a uniform set(key,value) interface.
 */
class CoilParams {
public:

    CoilParams();

    /**
     * Set a parameter by name.
     * @param key   Name of the parameter, e.g. "width", "turns"
     * @param value String value to parse and assign
     * @return      true if parameter exists and value is valid
     */
    bool set(const String& key, const String& value);

    void print();


private:
    float _positionLeft;        //Extreme position of the coil from the left side of the casing [mm]
    float _positionRight;       //Extreme position of the coil from the right side of the casing [mm]
    float _width;               //Coil width, note the algorithm should verify that the given width is not greater than the possible winding area [mm]
    int _Turns;                 //Number of coil turns, cannot be less than 0 and must be an integer []
    float _wireDiameter;        //Diameter of the winding wire, on the basis of which the minimum pitch is determined. [mm]
    bool _coilWindingDirection; //Coil winding direction. CW - true, CCW - false []

    
    float _pitchWinding;        //The pitch of the coil turns, taken into account when provided by the user. Basically selected to fill the entire coil width as densely as possible with the minimum number of layers. [mm]

    // Individual setters
    bool setPositionLeft(const String& val);
    bool setPositionRight(const String& val);
    bool setWidth(const String& val);
    bool setTurns(const String& val);
    bool setDiameter(const String& val);
    bool setDirection(const String& val);

    // Helper: trim and parse float/int safely
    float  toFloatClamped(const String& val, float minV, float maxV, bool& ok);
    int32_t   toIntClamped (const String& val, long  minV, long  maxV, bool& ok);

    // Struktura mapująca klucz -> setter
    typedef bool (CoilParams::*SetterFn)(const String&);
    struct ParamHandler {
        const char*     key;
        SetterFn        fn;
    };
    static const ParamHandler _handlers[];
};

#endif // COIL_PARAMS_H
