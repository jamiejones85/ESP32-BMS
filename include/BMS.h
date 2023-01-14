#ifndef BMS_H
#define BMS_H
#include <Arduino.h>
#include "BMSModuleManager.h"
#include "OutlanderCharger.h"
#include "Config.h"
#include "IO.h"
#include "ArduinoJson.h"
#include "Shunt.h"


#define Boot 0
#define Ready 1
#define Drive 2
#define Charge 3
#define Precharge 4
#define Error 5

class Bms {
  public:
    Bms();
    void setup(const EEPROMSettings& settings);
    void execute();
    void ms500Task(const EEPROMSettings& settings);
    void ms1000Task();
    void canRead(int canInterfaceOffset, int idOffset);
    void printSummary();
    BMSModuleManager& getBMSModuleManager();
    OutlanderCharger& getOutlanderCharger();
    Shunt& getShunt();

    IO& getIO();
    byte getStatus();
    bool contactorsClosed();
    bool getBalanceCells();

  private:
    EEPROMSettings settings;
    byte status;
    BMSModuleManager bmsModuleManager;
    OutlanderCharger outlanderCharger;
    IO io;
    unsigned char alarm[4];
    unsigned char warning[4];
    unsigned long inverterLastRec;
    byte inverterStatus;
    bool inverterInForwardReverse;
    bool balanceCells;
    void broadcastStatus(const EEPROMSettings& settings);
    void updateAlarms(const EEPROMSettings& settings);
    void updateStatus();

};

#endif