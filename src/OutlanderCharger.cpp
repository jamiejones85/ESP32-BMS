#include "OutlanderCharger.h"

bool OutlanderCharger::isDoneCharging(EEPROMSettings &settings, BMSModuleManager& bmsModuleManager) {
    //if any cell goes over the set point, we are full
    if (bmsModuleManager.getHighCellVolt() > settings.chargeVsetpoint) {
        request_current = true;
    }

    //if current reaches the lowpoint setpoint, we are full

    false;
}
void OutlanderCharger::processMessage(BMS_CAN_MESSAGE &inMsg) {
    if (inMsg.id == 0x389) {
      reported_voltage = inMsg.buf[0] * 2;
      reported_dc_current = inMsg.buf[2];
      reported_temp1 = inMsg.buf[3] - 40;
      reported_temp2 = inMsg.buf[4] - 40;
      timeout = millis();

    } else if (inMsg.id == 0x38A) {
       reported_status = inMsg.buf[4];
       evse_duty = inMsg.buf[3];
    }
}

void OutlanderCharger::doCharge(EEPROMSettings &settings) {
    int chargeAmps = calculateCurrent(settings);
}

int OutlanderCharger::calculateCurrent(EEPROMSettings &settings) {

    //we want to limit the current to keep to the max set up until the voltage reaches the full battery
    //voltage, at which point we want to bring down the current to stop the voltage going over, implmenting 
    // constant current constant voltage phases of the charge.
    
    //voltage derate
    if (reported_voltage > (settings.chargeVsetpoint * settings.seriesCells)) {
        request_current--;
    }

    //tempearture derate
    if (reported_temp1 > settings.chargerTempDeRate || reported_temp2 > settings.chargerTempDeRate) {
        request_current--;
        return request_current;
    }

    if (request_current < settings.chargecurrentmax || reported_voltage < (settings.chargeVsetpoint * settings.seriesCells)) {
        request_current ++;
    }
}