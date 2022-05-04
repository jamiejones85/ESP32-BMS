#include "IO.h"
#include "Arduino.h"

void IO::setup() {
    pinMode(CHARGE_IN, INPUT);
    chargeOverride = false;
}

void IO::setChargeOverride(bool override) {
    chargeOverride = override;
}

bool IO::getChargeOverride() {
    return chargeOverride;
}

bool IO::isChargeEnabled() {
    return digitalRead(CHARGE_IN)  || chargeOverride == true;
}

bool IO::isDriveEnabled(bool inverterInForwardReverse) {
    //read pin or inverter status in forward/reverse
    return digitalRead(DRIVE_IN)  || inverterInForwardReverse == true;
}