// main/FrameRotation.cpp
#include "FrameRotation.h"

int32_t currentPositionFrameRotation = 0;
int32_t targetPositionFrameRotation = 0;

bool volatile frameRotationWork = LOW;

// Store pins, configure OUTPUT, and disable motor by default
FrameRotation::FrameRotation(uint8_t dirPin, uint8_t stepPin, uint8_t enPin)
  : _dirPin(dirPin), _stepPin(stepPin), _enPin(enPin)
{
    frameRotationWork = LOW;
    currentPositionFrameRotation = 0;
    targetPositionFrameRotation = 0;

    _CLK_DIVIDER    = 2;
    _clkFreq        = SystemCoreClock / _CLK_DIVIDER;
    _KFreq          = _clkFreq / 1000000;
    _RA_ticks       = calculateTicks(static_cast<float>(HIGH_PULSE_TIME_FRAME_ROTATION));
    _RC_ticks       = calculateTicks(static_cast<float>(MIN_PERIOD_FRAME_ROTATION));

    pinMode(_dirPin, OUTPUT);
    pinMode(_stepPin, OUTPUT);
    configureTimerPWM(); //
    pinMode(_enPin, OUTPUT);
    // enable();
    disable(); 
}

// --- PUBLIC ---

void FrameRotation::setDirection(bool cw) {
  // HIGH = CCW, LOW = CW
  digitalWrite(_dirPin, cw ? HIGH : LOW);
}

void FrameRotation::step() {
  // Generate one pulse: 31 µs HIGH + 31 µs LOW → ~62 µs period
  digitalWrite(_stepPin, HIGH);
  delayMicroseconds(26);
  digitalWrite(_stepPin, LOW);
  delayMicroseconds(26);
}

void FrameRotation::enable() {
    digitalWrite(_enPin, LOW);
}

void FrameRotation::disable() {
    digitalWrite(_enPin, HIGH);
    brake();
}

void FrameRotation::run() {
    if (frameRotationWork) {
        return;
    } else if (targetPositionFrameRotation > currentPositionFrameRotation) {
        //right
        setDirection(LOW);
    } else if (targetPositionFrameRotation < currentPositionFrameRotation) {
        //left
        setDirection(HIGH);
    } else {
        // disable(); //dla testow usunac pozniej
        frameRotationWork = LOW;
        return;
    }
    frameRotationWork = HIGH;
    // enable();
    PWM->PWM_IER1 = (1u << PWM_CH);
    PWM->PWM_ENA = (1u << PWM_CH);
}

void FrameRotation::brake() {
    PWM->PWM_IDR1 = (1u << PWM_CH);
    PWM->PWM_DIS  = (1u << PWM_CH);
    frameRotationWork = LOW;
}

void FrameRotation::setTarget(int32_t target) {
    if (frameRotationWork) {
        return;
    } else {
        targetPositionFrameRotation = static_cast<int32_t>(target);
    }
}

bool FrameRotation::setSpeed(float speed) {
    if (LOW) {
        return LOW;
    } else if (speed > MAX_SPEED_FRAME_ROTATION) {
        //tutaj ustawic min period
        setPeriod(MIN_PERIOD_FRAME_ROTATION);
        return LOW;
    } else if (speed < MIN_SPEED_FRAME_ROTATION) {
        setPeriod(calculatePeriod(MIN_SPEED_FRAME_ROTATION));
        return LOW;
    } else {
        //tutaj przeliczy period na bazie speed i ustawic
        setPeriod(calculatePeriod(speed));
        //zwrocic prawda, bo zostala ustawiona zadana predkosc
        return HIGH;
    }
    return LOW;
}

void FrameRotation::setAbsolutePosition(float position) {
    if (frameRotationWork) {
        return;
    } else {
        targetPositionFrameRotation = calculateTickPosition(position);
    }
}

void FrameRotation::setRelativePosition(float position) {
    if (frameRotationWork) {
        return;
    } 
    setAbsolutePosition( calculatePosition(currentPositionFrameRotation) + position);
}


// --- PRIVATE ---

void FrameRotation::configureTimerPWM() {
    // pmc_set_writeprotect(false);
    pmc_enable_periph_clk(ID_PWM_PERIPH);

    PIOX->PIO_PDR = PIO;
    PIOX->PIO_ABSR |= PIO;

    PWM->PWM_CLK = PWM_CLK_DIVA(_CLK_DIVIDER);

    PWM->PWM_CH_NUM[PWM_CH].PWM_CMR =
        PWM_CMR_CPRE_CLKA;

    PWM->PWM_CH_NUM[PWM_CH].PWM_CPRD = _RC_ticks;
    PWM->PWM_CH_NUM[PWM_CH].PWM_CDTY = _RA_ticks;

    NVIC_EnableIRQ(PWM_IRQn);
    PWM->PWM_IER1 = (1u << PWM_CH);
    PWM->PWM_ENA = (1u << PWM_CH);
    // Nie właczam tutaj kanalu PWM to robi run
}

float FrameRotation:: calculatePeriod(float speed) {
    if (speed <= 0.0f) {
        return static_cast<float>(UINT32_MAX);
    }
    // obr/min -> pulse/s
    float pulse = (speed / 60) * STEPS_PER_REV_SPINDLE * MICROSTEPS_SPINDLE;
    // pulse/s -> period [us]
    return ((1 / pulse) * 1000000);
}

void FrameRotation::setPeriod(float period) {
    float newPeriod = static_cast<float>(MIN_PERIOD_FRAME_ROTATION);
    if (newPeriod > period) {
        _RC_ticks = calculateTicks(newPeriod);
    } else {
        _RC_ticks = calculateTicks(period);
    }
    PWM->PWM_CH_NUM[PWM_CH].PWM_CPRD = _RC_ticks;
}

uint32_t FrameRotation::calculateTicks(float time) {
    if (time > static_cast<float>(UINT32_MAX / _KFreq)) {
        return UINT32_MAX;
    }
    return static_cast<uint32_t>(time * _KFreq);
}

int32_t FrameRotation::calculateTickPosition(float position) {
    return static_cast<int32_t>(roundf(position * STEPS_PER_REV_FRAME_ROTATION * MICROSTEPS_FRAME_ROTATION));
}

float FrameRotation::calculatePosition(int32_t ticks) {
    return static_cast<float>(ticks * 1.0f / (STEPS_PER_REV_FRAME_ROTATION * MICROSTEPS_FRAME_ROTATION));
}


// --- REMAINING ---

void PWM_Handler() {
    uint32_t status = PWM->PWM_ISR1;
    if (status & (1u << PWM_CH)) {
        if (digitalRead(B_DIR_PIN)){
            currentPositionFrameRotation--; //jesli druga strona to zmienic na ++
            if (currentPositionFrameRotation <= targetPositionFrameRotation) {
                PWM->PWM_IDR1 = (1u << PWM_CH);
                PWM->PWM_DIS  = (1u << PWM_CH);
                frameRotationWork = LOW;
            }
        } else {
            currentPositionFrameRotation++; //jesli druga strona to zmienic na --
            if (currentPositionFrameRotation >= targetPositionFrameRotation) {
                PWM->PWM_IDR1 = (1u << PWM_CH);
                PWM->PWM_DIS  = (1u << PWM_CH);
                frameRotationWork = LOW;
            }
        }
    }
}
