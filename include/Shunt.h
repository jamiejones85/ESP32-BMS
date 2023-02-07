#ifndef Shunt_H
#define Shunt_H
#include "BMSCan.h"
#include "Config.h"

typedef struct ShuntData {
    signed long voltage1, voltage2, voltage3;
    double amphours, kilowatthours, kilowatts;
    int32_t milliamps;
};

class Shunt
{
  public:
    void resetCounters(BMS_CAN_MESSAGE &msg, BmsCan &bmscan, const EEPROMSettings &settings);
    void process(BMS_CAN_MESSAGE &inMsg);
    int getStateOfCharge(const EEPROMSettings &settings);
    ShuntData getData();
  private:
    ShuntData data;
};

#endif