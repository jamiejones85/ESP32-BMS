#ifndef Shunt_H
#define Shunt_H
#include "BmsCan.h"
#include "Config.h"

class Shunt
{
  public:
    void process(BMS_CAN_MESSAGE &inMsg);
    int getStateOfCharge(EEPROMSettings &settings);
  private:
    signed long voltage1, voltage2, voltage3 = 0; //mV only with ISAscale sensor
    double amphours, kilowatthours, kilowatts; //only with ISAscale sensor
    int32_t CANmilliamps;
};

#endif