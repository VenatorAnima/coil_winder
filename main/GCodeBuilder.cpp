// main/GCodeBuilder.cpp
#include "GCodeBuilder.h"

struct GCodeBuilder::Node {
    String line;
    Node*   next;
    Node(const String& l): line(l), next(nullptr) {}
};

GCodeBuilder::GCodeBuilder(CoilParams* co_p, CoilLogic* co_l, FrameParams* fr, GCodeParser* gc)
    : _coilParams(co_p), _coilLogic(co_l), _frameParams(fr), _gcode(gc) {
        reloadParameters();
    }

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

void GCodeBuilder::createGCodeList() {
    clearList();

    reloadParameters();
    if (!validateParameters()) return;

    calculateAuxiliary();
    
    int _turnsWork = 0;
    int _helpInt = 0;
    int _helpFrameDirection = 0;
    float _helpFloat = 0.0f;
    float _helpSpindleDirection = 0.0f;

    // Zerowanie
    addLine("G28");
    // Ruch relatywny
    addLine("G91");
    if (_windFromTheLeft) {
        addLine("G0 X" + String(_frameOffset + _positionLeft));
        _helpSpindleDirection = 1.0f;
    } else {
        addLine("G0 X" + String(_frameOffset + _frameWidth - _positionRight));
        _helpSpindleDirection = -1.0f;
    }
    if (_coilWindingDirection) {
        _helpFrameDirection = 1;
    } else {
        _helpFrameDirection = -1; //zweryfikowac
    }
    

    while (_numberOfLayers > 0) {
        _turnsWork = _numberOfTurnsPerLayer - ((_turnsBegin * 2) + (_turnsSlow * 2));

        if (_turnsWork < 0) {
            _helpInt = _numberOfTurnsPerLayer - (_turnsBegin * 2);
            if (_helpInt <= 0 ) {
                _turnsSlow = _numberOfTurnsPerLayer / 2;
                _turnsWork = _numberOfTurnsPerLayer % 2;
                _turnsBegin = 0;
            } else {
                _turnsSlow = _helpInt / 2;
                _turnsWork = _helpInt % 2;
            }
        }

        if (_missingTurns > 0) {
            _helpInt = static_cast<int>(round((_coilWidth - (_numberOfTurnsPerLayer * _wireDiameter)) / _wireDiameter));
            if (_helpInt >= _missingTurns) {
                _turnsWork = _turnsWork + _missingTurns;
                _missingTurns = 0;
            } else {
                _turnsWork = _turnsWork + _helpInt;
                _missingTurns = _missingTurns - _helpInt;
            }
        }

        if (_turnsBegin > 0)
        {
            _helpFloat = _wireDiameter * _turnsBegin;
            addLine("G1 X" + String(_helpSpindleDirection * _helpFloat) + " A" + String(_helpFrameDirection * _turnsBegin) + " F" + String(_speedBegin));
        }
        if (_turnsSlow > 0)
        {
            _helpFloat = _wireDiameter * _turnsSlow;
            addLine("G1 X" + String(_helpSpindleDirection * _helpFloat) + " A" + String(_helpFrameDirection * _turnsSlow) + " F" + String(_speedSlow));
        }
        if (_turnsWork > 0)
        {
            _helpFloat = _coilWidth - (((_turnsBegin + _turnsSlow) * 2) * _wireDiameter);
            addLine("G1 X" + String(_helpSpindleDirection * _helpFloat) + " A" + String(_helpFrameDirection * _turnsWork) + " F" + String(_speedWork));
        }
        if (_turnsSlow > 0)
        {
            _helpFloat = _wireDiameter * _turnsSlow;
            addLine("G1 X" + String(_helpSpindleDirection * _helpFloat) + " A" + String(_helpFrameDirection * _turnsSlow) + " F" + String(_speedSlow));
        }
        if (_turnsBegin > 0)
        {
            _helpFloat = _wireDiameter * _turnsBegin;
            addLine("G1 X" + String(_helpSpindleDirection * _helpFloat) + " A" + String(_helpFrameDirection * _turnsBegin) + " F" + String(_speedBegin));
        }

        _helpSpindleDirection = _helpSpindleDirection * -1.0f;
        _numberOfLayers--;
    }
    
}

void GCodeBuilder::executeAll() {
    for (Node* p = _head; p; p = p->next) {
        Serial.println(p->line);
        _gcode->parseLine(p->line);
        while (frameRotationWork || spindleWork) {
            //block
        }
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

void GCodeBuilder::reloadParameters() {
    _frameParams->get<float>("frameOffset", _frameOffset);
    _frameParams->get<float>("width", _frameWidth);

    _coilParams->get<float>("positionLeft", _positionLeft);
    _coilParams->get<float>("positionRight", _positionRight);
    _coilParams->get<float>("width", _coilWidth);
    _coilParams->get<float>("wireDiameter", _wireDiameter);
    _coilParams->get<int>("turns", _turns);
    _coilParams->get<bool>("coilWindingDirection", _coilWindingDirection);

    _coilLogic->get<float>("speedBegin", _speedBegin);
    _coilLogic->get<float>("speedSlow", _speedSlow);
    _coilLogic->get<float>("speedWork", _speedWork);
    _coilLogic->get<int>("turnsBegin", _turnsBegin);
    _coilLogic->get<int>("turnsSlow", _turnsSlow);
    _coilLogic->get<bool>("windFromTheLeft", _windFromTheLeft);
    _coilLogic->get<bool>("endAtStartingPoint", _endAtStartingPoint);
}

bool GCodeBuilder::validateParameters() {
    if ((_frameOffset + _frameWidth) > SCREW_LENGTH_MM) {
        Serial.println("");
        Serial.println("");
        Serial.println("ERR: Too big frame!");
        return false;
    }
    if ((_positionLeft + _positionRight + _coilWidth) > _frameWidth) {
        Serial.println("");
        Serial.println("");
        Serial.println("ERR: The coil will not fit on the frame!");
        return false;
    }
    if (_wireDiameter > _coilWidth) {
        Serial.println("");
        Serial.println("");
        Serial.println("ERR: Wire too thick to wind the coil!");
        return false;
    }

    return true;
}

void GCodeBuilder::calculateAuxiliary() {
    _maxTurnsPerLayer = static_cast<int>(round(_coilWidth / _wireDiameter));
    // _numberOfLayers = static_cast<int>(round(_turns * 1.0 / _maxTurnsPerLayer));
    _numberOfLayers = (_turns + _maxTurnsPerLayer - 1) / _maxTurnsPerLayer;

    if (_endAtStartingPoint) {
        if ((_numberOfLayers % 2) == 1) {
            _numberOfLayers++;
        }
    }

    _numberOfTurnsPerLayer = _turns / _numberOfLayers;
    _missingTurns = _turns - (_numberOfTurnsPerLayer * _numberOfLayers);
}

void GCodeBuilder::print() {
    Serial.println("");
    Serial.println("");
    Serial.println("Summary");
    Serial.println("");

    Serial.print("Maximum number of turns per layer: ");
    Serial.println(_maxTurnsPerLayer);

    Serial.print("Number of layers: ");
    Serial.println(_numberOfLayers);

    Serial.print("Number of turns per layer: ");
    Serial.println(_numberOfTurnsPerLayer);

    Serial.print("Excess turns added to subsequent layers if they fit: ");
    Serial.println(_missingTurns);
}