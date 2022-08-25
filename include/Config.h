#ifndef CONFIG_H
#define CONFIG_H
#define EEPROM_VERSION      0x03   //update any time EEPROM struct below is changed.
#include <ArduinoJson.h>
#include "EepromSettings.h"

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