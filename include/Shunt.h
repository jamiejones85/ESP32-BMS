#ifndef Shunt_H
#define Shunt_H
#include "BMSCan.h"
#include "Config.h"

struct ShuntData {
    signed long voltage1, voltage2, voltage3;
    double amphours, kilowatthours, kilowatts;
    int32_t milliamps;
};

class Shunt
{
  public:
    Shunt(EEPROMSettings& settings);
    void resetCounters(BMS_CAN_MESSAGE &msg, BmsCan &bmscan);
    void process(BMS_CAN_MESSAGE &inMsg);
    int getStateOfCharge();
    ShuntData getData();
  private:
    ShuntData data;
    EEPROMSettings& settings;
};

#endif