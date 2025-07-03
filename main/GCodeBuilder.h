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

/**
 * CoilLogic
 */
class GCodeBuilder {
public:

    GCodeBuilder(CoilParams* co_p, CoilLogic* co_l, FrameParams* fr, GCodeParser* gc);
    ~GCodeBuilder();

    void addLine(const String& line);

    void executeAll();

private:
    CoilParams* _coilParams;
    CoilLogic* _coilLogic;
    FrameParams* _frameParams;
    GCodeParser* _gcode;

    struct Node;

    Node* _head;
    Node* _tail;

    void clearList();
};

#endif // GCODE_BUILDER_H
