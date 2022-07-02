#include "BmsCanMessage.h"

#ifndef ISERIAL_H
#define ISERIAL_H
class ISerial {
  public:
     virtual void println()  = 0;
     virtual void println(float f)  = 0;
     virtual void println(const char c[]) = 0;
     virtual void println(uint8_t c) = 0;
     virtual void println(uint32_t c) = 0;

     virtual void print(const char c[]) = 0;
     virtual void print(uint8_t c) = 0;
     virtual void print(uint32_t c) = 0;

};
#endif