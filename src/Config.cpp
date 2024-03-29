#include "Config.h"
#include <EEPROM.h>

void Config::load(EEPROMSettings& settings) {
  EEPROM.get(0, settings);
  Serial.print("EEPROM VERSION: ");
  Serial.println(settings.version);
  if (settings.version != EEPROM_VERSION)
  {
    Serial.println("Loading Defaults");
    loadDefaults(settings);
  }
}

void Config::save(EEPROMSettings& settings) {
  Serial.print("SAVING EEPROM VERSION: ");
  Serial.println(settings.version);
  EEPROM.put(0, settings);
  EEPROM.commit();
}

void Config::loadDefaults(EEPROMSettings& settings) {
    settings.version = EEPROM_VERSION;
    settings.carCanIndex = 1;
    settings.batteryCanIndex = 0;
    settings.seriesCells = 96;
    settings.overVSetpoint = 4200;
    settings.underVSetpoint = 3000;
    settings.chargeVsetpoint = 4100;
    settings.dischVsetpoint = 3200;
    settings.cellGapAlarm = 90;
    settings.warnOffset = 100;
    settings.useableAh = 20;
    //temperatures
    settings.overTSetpoint = 65;
    settings.underTSetpoint = -10;
    settings.chargeTSetpoint = 0; //derate charging at this temperature
    settings.disTSetpoint = 40;
    settings.warnTempOffset = 10;
    //charging
    settings.chargecurrentmax = 6; //6 amps
    settings.chargerTempDeRate = 70;

    settings.parallelStrings = 1;
    settings.balanceVoltage = 3900;
    settings.balanceHyst = 40;

    settings.acDetectionMethod = AC_METHOD_J1772;
    settings.hvPresentMethod = HV_PRESENT_A2_ANALOG;
}

void Config::fromJson(EEPROMSettings& settings, JsonObject &doc) {

    settings.version = doc["version"];
    settings.carCanIndex = doc["carCanIndex"];
    settings.batteryCanIndex = doc["batteryCanIndex"];
    settings.seriesCells = doc["seriesCells"];
    settings.overVSetpoint = doc["overVSetpoint"];
    settings.underVSetpoint = doc["underVSetpoint"];
    settings.chargeVsetpoint = doc["chargeVsetpoint"];
    settings.dischVsetpoint = doc["dischVsetpoint"];
    settings.useableAh = doc["useableAh"];

    //temperatures
    settings.overTSetpoint = doc["overTSetpoint"];
    settings.underTSetpoint = doc["underTSetpoint"];
    settings.chargeTSetpoint = doc["chargeTSetpoint"]; //derate charging at this temperature
    settings.disTSetpoint = doc["disTSetpoint"];
    //charging
    settings.chargecurrentmax = doc["chargecurrentmax"];

    settings.balanceVoltage = doc["balanceVoltage"];
    settings.balanceHyst = doc["balanceHyst"];
    settings.parallelStrings = doc["parallelStrings"]; //remove and use second pack can index

    settings.hvPresentMethod = doc["hvPresentMethod"];
    settings.acDetectionMethod = doc["acDetectionMethod"];
}

void Config::toJson(EEPROMSettings& settings, DynamicJsonDocument &root) {
    root["version"] = settings.version;
    root["carCanIndex"] = settings.carCanIndex;
    root["batteryCanIndex"] = settings.batteryCanIndex;
    root["seriesCells"] = settings.seriesCells;
    root["overVSetpoint"] = settings.overVSetpoint;
    root["underVSetpoint"] = settings.underVSetpoint;
    root["chargeVsetpoint"] = settings.chargeVsetpoint;
    root["dischVsetpoint"] = settings.dischVsetpoint;
    root["useableAh"] = settings.useableAh;
    //temperatures
    root["overTSetpoint"] = settings.overTSetpoint;
    root["underTSetpoint"] = settings.underTSetpoint;
    root["chargeTSetpoint"] = settings.chargeTSetpoint; //derate charging at this temperature
    root["disTSetpoint"] = settings.disTSetpoint;
    //charging
    root["chargecurrentmax"] = settings.chargecurrentmax;

    root["balanceVoltage"] = settings.balanceVoltage;
    root["balanceHyst"] = settings.balanceHyst;
    root["parallelStrings"] = settings.parallelStrings;

    root["hvPresentMethod"] = settings.hvPresentMethod;
    root["acDetectionMethod"] = settings.acDetectionMethod;

}
