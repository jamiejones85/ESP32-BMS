#include "Bms.h"
#include "BmsCan.h"
#include "BMSModuleManager.h"

BmsCan bmscan;
BMS_CAN_MESSAGE msg;
BMS_CAN_MESSAGE inMsg;

void Bms::setup(const EEPROMSettings& settings) {
  SPI.begin();
  bmscan.begin(500000, 0);
  bmscan.begin(500000, 1);
  bmscan.begin(500000, 2);
}

void Bms::execute() {
    Bms::canRead(0, 0);
}

void Bms::ms500Task(const EEPROMSettings& settings) {
    Bms::broadcastStatus(settings);
}

void Bms::canRead(int canInterfaceOffset, int idOffset)
{
  // bmscan.read(inMsg, canInterfaceOffset);

  // //TODO::this can probably be better
  // if (inMsg.id < 0x300)//do VW BMS magic if ids are ones identified to be modules
  // {
  //   inMsg.id = inMsg.id + idOffset;
  //   bmsModuleManager.decodecan(inMsg, 0); //do VW BMS if ids are ones identified to be modules
  // }

  // if ((inMsg.id & 0x1FFFFFFF) < 0x1A5554F0 && (inMsg.id & 0x1FFFFFFF) > 0x1A555400)   // Determine if ID is Temperature CAN-ID
  // {
  //   inMsg.id = inMsg.id + idOffset/4; // the temps only require offsetting id by 8 (1/4 of 32) i.e. 1 can id per slave. 
  //   bmsModuleManager.decodetemp(inMsg, 0);
  // }
  
}

void Bms::broadcastStatus(const EEPROMSettings& settings) {
      // msg.id  = 0x351;
  // msg.len = 8;

  // msg.buf[0] = lowByte(uint16_t((settings.ChargeVsetpoint * settings.Scells ) * 10));
  // msg.buf[1] = highByte(uint16_t((settings.ChargeVsetpoint * settings.Scells ) * 10));
  // msg.buf[2] = lowByte(chargecurrent);
  // msg.buf[3] = highByte(chargecurrent);
  // msg.buf[4] = lowByte(discurrent );
  // msg.buf[5] = highByte(discurrent);
  // msg.buf[6] = lowByte(uint16_t((settings.DischVsetpoint * settings.Scells) * 10));
  // msg.buf[7] = highByte(uint16_t((settings.DischVsetpoint * settings.Scells) * 10));

  // bmscan.write(msg, settings.carCanIndex);

  // msg.id  = 0x355;
  // msg.len = 8;
  // if (SOCoverride != -1) {
  //   msg.buf[0] = lowByte(SOCoverride);
  //   msg.buf[1] = highByte(SOCoverride);
  //   msg.buf[2] = lowByte(SOH);
  //   msg.buf[3] = highByte(SOH);
  //   msg.buf[4] = lowByte(SOCoverride * 10);
  //   msg.buf[5] = highByte(SOCoverride * 10);
  // } else {
  //   msg.buf[0] = lowByte(SOC);
  //   msg.buf[1] = highByte(SOC);
  //   msg.buf[2] = lowByte(SOH);
  //   msg.buf[3] = highByte(SOH);
  //   msg.buf[4] = lowByte(SOC * 10);
  //   msg.buf[5] = highByte(SOC * 10);
  // }

  // msg.buf[6] = lowByte(bmsstatus);
  // msg.buf[7] = highByte(bmsstatus);
  // bmscan.write(msg, settings.carCanIndex);

  msg.id  = 0x356;
  msg.len = 8;
  msg.buf[0] = lowByte(uint16_t(bmsModuleManager.getPackVoltage() * 100));
  msg.buf[1] = highByte(uint16_t(bmsModuleManager.getPackVoltage() * 100));
  // msg.buf[2] = lowByte(long(currentact / 100)); Acutal current
  // msg.buf[3] = highByte(long(currentact / 100));
  msg.buf[4] = lowByte(int16_t(bmsModuleManager.getAvgTemperature() * 10));
  msg.buf[5] = highByte(int16_t(bmsModuleManager.getAvgTemperature() * 10));
  msg.buf[6] = lowByte(uint16_t(bmsModuleManager.getAvgCellVolt() * 1000));
  msg.buf[7] = highByte(uint16_t(bmsModuleManager.getAvgCellVolt() * 1000));
  bmscan.write(msg, settings.carCanIndex);
  bmscan.write(msg, 2);

  delay(2);
  // msg.id  = 0x35A;
  // msg.len = 8;
  // msg.buf[0] = alarm[0];//High temp  Low Voltage | High Voltage
  // msg.buf[1] = alarm[1]; // High Discharge Current | Low Temperature
  // msg.buf[2] = alarm[2]; //Internal Failure | High Charge current
  // msg.buf[3] = alarm[3];// Cell Imbalance
  // msg.buf[4] = warning[0];//High temp  Low Voltage | High Voltage
  // msg.buf[5] = warning[1];// High Discharge Current | Low Temperature
  // msg.buf[6] = warning[2];//Internal Failure | High Charge current
  // msg.buf[7] = warning[3];// Cell Imbalance
  // bmscan.write(msg, settings.carCanIndex);


  delay(2);
  msg.id  = 0x373;
  msg.len = 8;
  msg.buf[0] = lowByte(uint16_t(bmsModuleManager.getLowCellVolt() * 1000));
  msg.buf[1] = highByte(uint16_t(bmsModuleManager.getLowCellVolt() * 1000));
  msg.buf[2] = lowByte(uint16_t(bmsModuleManager.getHighCellVolt() * 1000));
  msg.buf[3] = highByte(uint16_t(bmsModuleManager.getHighCellVolt() * 1000));
  msg.buf[4] = lowByte(uint16_t(bmsModuleManager.getLowTemperature() + 273.15));
  msg.buf[5] = highByte(uint16_t(bmsModuleManager.getLowTemperature() + 273.15));
  msg.buf[6] = lowByte(uint16_t(bmsModuleManager.getHighTemperature() + 273.15));
  msg.buf[7] = highByte(uint16_t(bmsModuleManager.getHighTemperature() + 273.15));
  bmscan.write(msg, settings.carCanIndex);
  bmscan.write(msg, 2);

  // delay(2);
  // msg.id  = 0x379; //Installed capacity
  // msg.len = 2;
  // msg.buf[0] = lowByte(uint16_t(settings.Pstrings * settings.CAP));
  // msg.buf[1] = highByte(uint16_t(settings.Pstrings * settings.CAP));

  // delay(2);
  // msg.id  = 0x372;
  // msg.len = 8;
  // msg.buf[0] = lowByte(bms.getNumModules());
  // msg.buf[1] = highByte(bms.getNumModules());
  // msg.buf[2] = 0x00;
  // msg.buf[3] = 0x00;
  // msg.buf[4] = 0x00;
  // msg.buf[5] = 0x00;
  // msg.buf[6] = 0x00;
  // msg.buf[7] = 0x00;
  // bmscan.write(msg, settings.carCanIndex);
}