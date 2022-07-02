#ifndef IMILLIS_H
#define IMILLIS_H
#include "IMillis.h"

class NativeMillis: public IMillis {
  public:
     unsigned long millis();
};

#endif