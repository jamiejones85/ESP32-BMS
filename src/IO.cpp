#include "IO.h"
#include "Arduino.h"

void IO::setup() {
    pinMode(CHARGE_IN, INPUT);
}

bool IO::isChargeEnabled() {
    return digitalRead(CHARGE_IN);
}

bool IO::isDriveEnabled() {

}