#ifndef BMS_CAN_H
#define BMS_CAN_H
#include <Arduino.h>
#include "BmsCanMessage.h"
#include "IBmsCan.h"
#include "esp32_can.h"

class BmsCan: public IBmsCan {
  public:
     int write(const BMS_CAN_MESSAGE &msg, int interfaceIndex);
     void begin(uint32_t baud, int interfaceIndex);
     int read (BMS_CAN_MESSAGE &msg, int interfaceIndex);
  private:
     CAN_FRAME convert(const BMS_CAN_MESSAGE &msg);
     BMS_CAN_MESSAGE convert(const CAN_FRAME &msg);

};

#endif