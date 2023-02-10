#ifndef CONFIG_H
#define CONFIG_H
#define MAX_MODULE_ADDR     0x3E
#define EEPROM_VERSION      0x03   //update any time EEPROM struct below is changed.

#define AC_METHOD_J1772 0
#define AC_METHOD_IEC_61851 1
#define AC_METHOD_D1_HIGH 2
#define AC_METHOD_D2_HIGH 3

#define HV_PRESENT_A2_ANALOG 0
#define HV_PRESENT_D1_HIGH 1
#define HV_PRESENT_D2_HIGH 2
#define AC_METHOD_CAN_BUS 3

#include "ArduinoJson.h"


typedef struct EEPROMSettings {
  int version;
  //can settings
  int carCanIndex;
  int batteryCanIndex;
  
  //battery pack settings
  int seriesCells;
  int useableAh;
  int parallelStrings;
  
  //voltages
  int overVSetpoint;
  int underVSetpoint;
  int chargeVsetpoint;
  int dischVsetpoint;
  int cellGapAlarm; //milivolt difference in min and max to raise alarm
  int warnOffset; //How close to min and max voltages to warn
  
  //temperatures
  int overTSetpoint;
  int underTSetpoint;
  int chargeTSetpoint;
  int disTSetpoint;
  int warnTempOffset; //How close to min and max temperatures to warn
  
  //charging
  int chargecurrentmax;
  int chargerTempDeRate; //bring down the amps when the charger over this
  int chargeHys;
  int chargeCurrentEnd; //When current tapers down to this value, we're full
  int acDetectionMethod;
  int hvPresentMethod;

  float balanceVoltage;
  float balanceHyst;


} EEPROMSettings;

class Config
{
  public:
    void load(EEPROMSettings& settings);
    void save(EEPROMSettings& settings);
    void toJson(EEPROMSettings& settings, DynamicJsonDocument &root);
    void fromJson(EEPROMSettings& settings, JsonObject &doc);
    void loadDefaults(EEPROMSettings& settings);
};

#endif