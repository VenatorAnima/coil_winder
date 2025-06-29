// main/Spindle.cpp
#include "Spindle.h"

uint32_t currentPositionSpindle = 0;
uint32_t targetPositionSpindle = 0;

bool volatile spindleWork = LOW;

// Store pins, configure OUTPUT, and disable motor by default
Spindle::Spindle(uint8_t dirPin, uint8_t stepPin, uint8_t enPin)
  : _dirPin(dirPin), _stepPin(stepPin), _enPin(enPin)
{
    spindleWork = LOW;
    currentPositionSpindle = 0;
    targetPositionSpindle = 0;

    float f = ((SCREW_LENGTH_MM / REV_TRAVEL_MM) * STEPS_PER_REV_SPINDLE * MICROSTEPS_SPINDLE);
    float r = roundf(f);
    if (r > static_cast<float>(INT32_MAX))                  _maxPosition = 0;
    else if (r < static_cast<float>(INT32_MIN) || r < 0)    _maxPosition = 0;
    else                                                    _maxPosition = static_cast<int32_t>(r);

    _CLK_DIVIDER    = 2;
    _clkFreq        = SystemCoreClock / _CLK_DIVIDER;
    _KFreq          = _clkFreq / 1000000;
    _RA_ticks       = calculateTicks(static_cast<float>(HIGH_PULSE_TIME_SPINDLE));
    _RC_ticks       = calculateTicks(static_cast<float>(MIN_PERIOD_SPINDLE));

    pinMode(_dirPin, OUTPUT);
    pinMode(_stepPin, OUTPUT);
    configureTimer();
    pinMode(_enPin, OUTPUT);
    // enable();
    disable(); 
}

// --- PUBLIC ---

void Spindle::setDirection(bool cw) {
  // HIGH = left, LOW = right
  digitalWrite(_dirPin, cw ? HIGH : LOW);
}

void Spindle::step() {
  // Generate one pulse: 31 µs HIGH + 31 µs LOW → ~62 µs period
  digitalWrite(_stepPin, HIGH);
  delayMicroseconds(26);
  digitalWrite(_stepPin, LOW);
  delayMicroseconds(26);
}

void Spindle::enable() {
    digitalWrite(_enPin, LOW);
}

void Spindle::disable() {
    digitalWrite(_enPin, HIGH);
    brake();
}

void Spindle::run() {
    if (spindleWork) {
        return;
    } else if (targetPositionSpindle > currentPositionSpindle) {
        //right
        setDirection(LOW);
    } else if (targetPositionSpindle < currentPositionSpindle) {
        //left
        setDirection(HIGH);
    } else {
        // disable(); //dla testow usunac pozniej
        //spindleWork = LOW;
        return;
    }
    spindleWork = HIGH;
    // enable();
    TC->TC_CHANNEL[TC_CH].TC_IER = TC_IER_CPCS;
    TC->TC_CHANNEL[TC_CH].TC_CCR = TC_CCR_CLKEN | TC_CCR_SWTRG;
}

void Spindle::brake() {
    TC->TC_CHANNEL[TC_CH].TC_IDR = TC_IDR_CPCS;
    TC->TC_CHANNEL[TC_CH].TC_CCR = TC_CCR_CLKDIS;
    spindleWork = LOW;
}

void Spindle::setTarget(int32_t target) {
    if (spindleWork) {
        return;
    } else if (target < 0) {
        targetPositionSpindle = 0;
    } else if (target > _maxPosition) {
        targetPositionSpindle = _maxPosition;
    } else {
        targetPositionSpindle = static_cast<uint32_t>(target);
    }
}

bool Spindle::setSpeed(float speed) {
    if (LOW) {
        return LOW;
    } else if (speed > MAX_SPEED_SPINDLE) {
        //tutaj ustawic min period
        setPeriod(MIN_PERIOD_SPINDLE);
        return LOW;
    } else if (speed < MIN_SPEED_SPINDLE) {
        setPeriod(calculatePeriod(0.001f));
    } else {
        //tutaj przeliczy period na bazie speed i ustawic
        setPeriod(calculatePeriod(speed));
        //zwrocic prawda, bo zostala ustawiona zadana predkosc
        return HIGH;
    }
    return LOW;
}

void Spindle::setAbsolutePosition(float position) {
    if (spindleWork) {
        return;
    } else if (position < 0){
        targetPositionSpindle = 0;
    } else if (position > SCREW_LENGTH_MM) {
        targetPositionSpindle = calculateTickPosition(SCREW_LENGTH_MM);
    } else {
        targetPositionSpindle = calculateTickPosition(position);
    }
}

void Spindle::setRelativePosition(float position) {
    if (spindleWork) {
        return;
    } 
    setAbsolutePosition(calculatePosition(targetPositionSpindle) + position);
}

void Spindle::home() {
    bool stateEndstop = endstopA.rawState();
    setSpeed(10.0f);
    if (stateEndstop) {
        // endstop pressed
        currentPositionSpindle = calculateTickPosition(2.0f);
        setAbsolutePosition(6.0f);
    } else {
        // endstop no pressed
        currentPositionSpindle = calculateTickPosition(SCREW_LENGTH_MM * 2);
        setAbsolutePosition(0.0f);
        run();
        while (!endstopA.rawState())
        {
            /* waite */
        }
        brake();
        currentPositionSpindle = calculateTickPosition(2.0f);
        setAbsolutePosition(5.0f);
    }
    setSpeed(4.0f);
    run();
    while (spindleWork)
    {
        /* waite */
    }
    
    setAbsolutePosition(0.0f);
    run();
    while (!endstopA.rawState())
    {
        /* waite */
    }
    brake();
    currentPositionSpindle = 0;
    setSpeed(1.0f);
    setRelativePosition(1.0f);
    run();
    while (spindleWork)
    {
        /* waite */
    }
    currentPositionSpindle = 0;
    targetPositionSpindle = 0;
    setSpeed(20.0f);
}


// --- PRIVATE ---

void Spindle::configureTimer() {
    pmc_set_writeprotect(false);
    pmc_enable_periph_clk(ID_TC_CH);

    PIOX->PIO_PDR = PIO;
    PIOX->PIO_ABSR |= PIO;

    TC->TC_CHANNEL[TC_CH].TC_CMR =
          TC_CMR_TCCLKS_TIMER_CLOCK1
        | TC_CMR_WAVE
        | TC_CMR_WAVSEL_UP
        | TC_CMR_ACPA_CLEAR
        | TC_CMR_ACPC_SET;

    TC->TC_CHANNEL[TC_CH].TC_RA = _RA_ticks;
    TC->TC_CHANNEL[TC_CH].TC_RC = _RC_ticks;

    NVIC_EnableIRQ(TC6_IRQn);
    TC->TC_CHANNEL[TC_CH].TC_IER = TC_IER_CPCS;
    TC->TC_CHANNEL[TC_CH].TC_CCR = TC_CCR_CLKEN | TC_CCR_SWTRG;
}

float Spindle::calculatePeriod(float speed) {
    // mm/s -> pulse/s
    float pulse = (speed / REV_TRAVEL_MM) * STEPS_PER_REV_SPINDLE * MICROSTEPS_SPINDLE;
    // pulse/s -> period [us]
    return ((1 / pulse) * 1000000);
}

void Spindle::setPeriod(float period) {
    float newPeriod = static_cast<float>(MIN_PERIOD_SPINDLE);
    if (newPeriod > period) {
        _RC_ticks = calculateTicks(newPeriod);
    } else {
        _RC_ticks = calculateTicks(period);
    }
    TC->TC_CHANNEL[TC_CH].TC_RC = _RC_ticks;
}

uint32_t Spindle::calculateTicks(float time) {
    if (time > static_cast<float>(UINT32_MAX / _KFreq)) {
        return UINT32_MAX;
    }
    return static_cast<uint32_t>(time * _KFreq);
}

uint32_t Spindle::calculateTickPosition(float position) {
    return static_cast<uint32_t>(roundf(position / REV_TRAVEL_MM  * STEPS_PER_REV_SPINDLE * MICROSTEPS_SPINDLE));
}

float Spindle::calculatePosition(uint32_t ticks) {
    return static_cast<float>(ticks * REV_TRAVEL_MM / (STEPS_PER_REV_SPINDLE * MICROSTEPS_SPINDLE));
}


// --- REMAINING ---

void TC6_Handler() {
    uint32_t status = TC->TC_CHANNEL[TC_CH].TC_SR;
    if (status & TC_SR_CPCS) {
        if (digitalRead(A_DIR_PIN)){
            currentPositionSpindle--; //jesli druga strona to zmienic na ++
            if (currentPositionSpindle <= targetPositionSpindle) {
                TC->TC_CHANNEL[TC_CH].TC_IDR = TC_IDR_CPCS;
                TC->TC_CHANNEL[TC_CH].TC_CCR = TC_CCR_CLKDIS;
                spindleWork = LOW;
            } else {
                TC->TC_CHANNEL[TC_CH].TC_CCR = TC_CCR_SWTRG;
            }
        } else {
            currentPositionSpindle++; //jesli druga strona to zmienic na --
            if (currentPositionSpindle >= targetPositionSpindle) {
                TC->TC_CHANNEL[TC_CH].TC_IDR = TC_IDR_CPCS;
                TC->TC_CHANNEL[TC_CH].TC_CCR = TC_CCR_CLKDIS;
                spindleWork = LOW;
            } else {
                TC->TC_CHANNEL[TC_CH].TC_CCR = TC_CCR_SWTRG;
            }
        }
    }
}