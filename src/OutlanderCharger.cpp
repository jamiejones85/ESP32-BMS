#include "OutlanderCharger.h"

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

    //hard stop
    if (settings.chargeVsetpoint) {
        request_current = 0;
    }

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