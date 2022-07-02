#ifndef Shunt_H
#define Shunt_H
#include "IBmsCan.h"
#include "EepromSettings.h"
#include <stdint.h>

struct ShuntData {
    signed long voltage1, voltage2, voltage3;
    double amphours, kilowatthours, watts;
    int32_t milliamps;
};

class Shunt
{
  public:
    Shunt(IBmsCan *can, EEPROMSettings *s);
    Shunt();
    void resetCounters(BMS_CAN_MESSAGE& msg);
    void process(BMS_CAN_MESSAGE& inMsg);
    void setSettings(EEPROMSettings *s);
    void setBmsCan(IBmsCan *can);
    int getStateOfCharge();
    ShuntData getData();
  private:
    ShuntData data;
    IBmsCan *bmsCanRef;
    EEPROMSettings *settings;
};

#endif