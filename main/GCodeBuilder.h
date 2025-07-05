// main/GCodeBuilder.h
#ifndef GCODE_BUILDER_H
#define GCODE_BUILDER_H

#include <Arduino.h>
#include <limits>
#include "CoilParams.h"
#include "CoilLogic.h"
#include "FrameParams.h"
#include "FrameRotation.h"
#include "Spindle.h"
#include "GCodeParser.h"
#include "config.h"

/**
 * CoilLogic
 */
class GCodeBuilder {
public:

    GCodeBuilder(CoilParams* co_p, CoilLogic* co_l, FrameParams* fr, GCodeParser* gc);
    ~GCodeBuilder();

    void addLine(const String& line);

    void createGCodeList();
    void executeAll();

    void reloadParameters();
    bool validateParameters();
    void calculateAuxiliary();

    void print();

private:
    CoilParams* _coilParams;
    CoilLogic* _coilLogic;
    FrameParams* _frameParams;
    GCodeParser* _gcode;

    struct Node;

    Node* _head;
    Node* _tail;

    int _maxTurnsPerLayer;
    int _numberOfLayers;
    int _numberOfTurnsPerLayer;
    int _missingTurns;

    // Parameters from other module
    //FrameParams
    float _frameOffset;
    float _frameWidth;

    //CoilParams
    float _positionLeft;
    float _positionRight;
    float _coilWidth;
    float _wireDiameter;
    int _turns;
    bool _coilWindingDirection;

    //CoilLogic
    float _speedBegin;
    float _speedSlow;
    float _speedWork;
    int _turnsBegin;
    int _turnsSlow;
    bool _windFromTheLeft;
    bool _endAtStartingPoint;
    // End Parameters from other module

    void clearList();
};

#endif // GCODE_BUILDER_H
