#ifndef BMS_H
#define BMS_H
#include <Arduino.h>
#include "BMSModuleManager.h"

#define Boot 0
#define Ready 1
#define Drive 2
#define Charge 3
#define Precharge 4
#define Error 5

class Bms {
  public:
    void setup(const EEPROMSettings& settings);
    void execute();
    void ms500Task(const EEPROMSettings& settings);
    void canRead(int canInterfaceOffset, int idOffset);
  private:
    void broadcastStatus(const EEPROMSettings& settings);
    BMSModuleManager bmsModuleManager;


};

#endif