#ifndef OUTLANDER_CHARGER_H
#define OUTLANDER_CHARGER_H
#include <Arduino.h>
#include "BMSCan.h"
#include "Config.h"
#include "BMSModuleManager.h"

class OutlanderCharger {
  public:
    OutlanderCharger(EEPROMSettings& settings, BMSModuleManager& bmsModuleManager,  BmsCan &bmscan);
    void processMessage(BMS_CAN_MESSAGE &inMsg);
    void doCharge(BMS_CAN_MESSAGE &msg);
    bool isDoneCharging();
    int reported_voltage = 0;
    int reported_dc_current = 0;
    int reported_temp1 = 0;
    int reported_temp2 = 0;
    int request_current = 0;
    byte reported_status = 0;
    byte evse_duty = 0;
  private:
    int timeout = 0;
    int calculateCurrent();
    int chargeAmps = 0;
    EEPROMSettings& settings;
    BMSModuleManager& bmsModuleManager;
    BmsCan &bmscan;
};

#endif