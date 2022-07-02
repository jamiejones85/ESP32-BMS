
#ifndef EEPROM_SETTINGS_H
#define EEPROM_SETTINGS_H

typedef struct EEPROMSettings {
  int version;
  //can settings
  int carCanIndex;
  int firstBatteryCanIndex;
  int secondBatteryCanIndex;
  //battery pack settings
  int seriesCells;
  int useableAh;
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
  int chargerTempDeRate;
  int chargeHys;
  int chargeCurrentEnd;

  int parallelStrings;

  float balanceVoltage;
  float balanceHyst;

} EEPROMSettings;

#endif