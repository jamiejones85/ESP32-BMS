#ifndef CONFIG_H
#define CONFIG_H
#define MAX_MODULE_ADDR     0x3E
#define EEPROM_VERSION      0x02   //update any time EEPROM struct below is changed.
#include "ArduinoJson.h"


typedef struct EEPROMSettings {
  int version;
  //can settings
  int chargerCanIndex;
  int carCanIndex;
  int firstBatteryCanIndex;
  int secondBatteryCanIndex;
  //battery pack settings
  int seriesCells;
  //voltages
  int overVSetpoint;
  int underVSetpoint;
  int chargeVsetpoint;
  int dischVsetpoint;
  //temperatures
  int overTSetpoint;
  int underTSetpoint;
  int chargeTSetpoint;
  int disTSetpoint;
  //charging
  int chargecurrentmax;
  int chargerTempDeRate;

} EEPROMSettings;

class Config
{
  public:
    EEPROMSettings load();
    void save(const EEPROMSettings& settings);
    void toJson(const EEPROMSettings& settings, DynamicJsonDocument &root);
    EEPROMSettings fromJson(JsonObject &doc);
    EEPROMSettings loadDefaults();
};

#endif