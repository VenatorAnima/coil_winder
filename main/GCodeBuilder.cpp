// main/GCodeBuilder.cpp
#include "GCodeBuilder.h"

struct GCodeBuilder::Node {
    String line;
    Node*   next;
    Node(const String& l): line(l), next(nullptr) {}
};

GCodeBuilder::GCodeBuilder(CoilParams* co_p, CoilLogic* co_l, FrameParams* fr, GCodeParser* gc)
    : _coilParams(co_p), _coilLogic(co_l), _frameParams(fr), _gcode(gc) {}

GCodeBuilder::~GCodeBuilder() {
    clearList();
}

void GCodeBuilder::addLine(const String& line) {
    Node* n = new Node(line);
    if (!_head) _head = _tail = n;
    else {
        _tail->next = n;
        _tail = n;
    }
}

void GCodeBuilder::executeAll() {
    for (Node* p = _head; p; p = p->next) {
        // _gcode->parseLine(p->line);
        Serial.println(p->line);
    }
}

void GCodeBuilder::clearList() {
    while (_head) {
        Node* tmp = _head;
        _head = _head->next;
        delete tmp;
    }
    _tail = nullptr;
}