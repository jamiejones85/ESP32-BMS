#include "Config.h"
#include <EEPROM.h>

EEPROMSettings Config::load() {
  EEPROMSettings settings;
  EEPROM.get(0, settings);
  Serial.println(settings.version);
  if (settings.version != EEPROM_VERSION)
  {
    Serial.println("Loading Defaults");
    settings = loadDefaults();
  }

  return settings;
}

void Config::save(const EEPROMSettings& settings) {
    Serial.println(settings.version);
    EEPROM.put(0, settings);
    EEPROM.commit();
}

EEPROMSettings Config::loadDefaults() {
    EEPROMSettings settings = EEPROMSettings();
    settings.version = EEPROM_VERSION;
    settings.carCanIndex = 1;
    settings.firstBatteryCanIndex = 0;
    settings.secondBatteryCanIndex = -1;
    settings.chargerCanIndex = 1;
    settings.seriesCells = 96;
    settings.overVSetpoint = 4200;
    settings.underVSetpoint = 3000;
    settings.chargeVsetpoint = 4100;
    settings.dischVsetpoint = 3200;
    //temperatures
    settings.overTSetpoint = 65;
    settings.underTSetpoint = -10;
    settings.chargeTSetpoint = 0; //derate charging at this temperature
    settings.disTSetpoint = 40;
    //charging
    settings.chargecurrentmax = 6; //6 amps
    settings.chargerTempDeRate = 70;
    return settings;
}

EEPROMSettings Config::fromJson(JsonObject &doc) {
    EEPROMSettings settings = EEPROMSettings();

    settings.version = doc["version"];
    settings.carCanIndex = doc["carCanIndex"];
    settings.firstBatteryCanIndex = doc["firstBatteryCanIndex"];
    settings.secondBatteryCanIndex = doc["secondBatteryCanIndex"];
    settings.chargerCanIndex = doc["chargerCanIndex"];
    settings.seriesCells = doc["seriesCells"];
    settings.overVSetpoint = doc["overVSetpoint"];
    settings.underVSetpoint = doc["underVSetpoint"];
    settings.chargeVsetpoint = doc["chargeVsetpoint"];
    settings.dischVsetpoint = doc["dischVsetpoint"];
    //temperatures
    settings.overTSetpoint = doc["overTSetpoint"];
    settings.underTSetpoint = doc["underTSetpoint"];
    settings.chargeTSetpoint = doc["chargeTSetpoint"]; //derate charging at this temperature
    settings.disTSetpoint = doc["disTSetpoint"];
    //charging
    settings.chargecurrentmax = doc["chargecurrentmax"];

    return settings;
}

void Config::toJson(const EEPROMSettings& settings, DynamicJsonDocument &root) {
    root["version"] = settings.version;
    root["carCanIndex"] = settings.carCanIndex;
    root["firstBatteryCanIndex"] = settings.firstBatteryCanIndex;
    root["secondBatteryCanIndex"] = settings.secondBatteryCanIndex;
    root["chargerCanIndex"] = settings.chargerCanIndex;
    root["seriesCells"] = settings.seriesCells;
    root["overVSetpoint"] = settings.overVSetpoint;
    root["underVSetpoint"] = settings.underVSetpoint;
    root["chargeVsetpoint"] = settings.chargeVsetpoint;
    root["dischVsetpoint"] = settings.dischVsetpoint;
    //temperatures
    root["overTSetpoint"] = settings.overTSetpoint;
    root["underTSetpoint"] = settings.underTSetpoint;
    root["chargeTSetpoint"] = settings.chargeTSetpoint; //derate charging at this temperature
    root["disTSetpoint"] = settings.disTSetpoint;
    //charging
    root["chargecurrentmax"] = settings.chargecurrentmax;


}
