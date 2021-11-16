#ifndef OUTLANDER_CHARGER_H
#define OUTLANDER_CHARGER_H
#include <Arduino.h>
#include "BmsCan.h"
#include "Config.h"

class OutlanderCharger {
  public:
    void processMessage(BMS_CAN_MESSAGE &inMsg);
    void doCharge(EEPROMSettings &settings);
  private:
    int reported_voltage = 0;
    int reported_dc_current = 0;
    int reported_temp1 = 0;
    int reported_temp2 = 0;
    int request_current = 0;
    byte reported_status = 0;
    byte evse_duty = 0;
    int timeout = 0;
    int calculateCurrent(EEPROMSettings &settings);
};

#endif