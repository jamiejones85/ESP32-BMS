#include "BmsCanMessage.h"

#ifndef IBMS_CAN_H
#define IBMS_CAN_H
class IBmsCan {
  public:
     virtual int write(const BMS_CAN_MESSAGE &msg, int interfaceIndex)  = 0;
     virtual void begin(uint32_t baud, int interfaceIndex) = 0;
     virtual int read (BMS_CAN_MESSAGE &msg, int interfaceIndex) = 0;
};
#endif