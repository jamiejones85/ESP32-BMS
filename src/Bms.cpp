#include "Bms.h"
#include "BmsCan.h"
#include "BMSModuleManager.h"
#include "OutlanderCharger.h"

BmsCan bmscan;
BMS_CAN_MESSAGE msg;
BMS_CAN_MESSAGE inMsg;

Bms::Bms() {
  Serial.println("BMS instansiated");
  status = Boot;
}

void Bms::setup(const EEPROMSettings& settings) {
  io.setup();
  SPI.begin();
  this->settings = settings;

  //only supports 1 pack at the moment
  bmsModuleManager.setPstrings(settings.parallelStrings);
  bmscan.begin(500000, 0);
  bmscan.begin(500000, 1);
  bmscan.begin(500000, 2);
}

void Bms::execute() {
    Bms::canRead(0, 0);

    if(inverterLastRec + 200 < millis()) {
      inverterStatus = 0;
    }
}

void Bms::ms500Task(const EEPROMSettings& settings) {
    bmsModuleManager.sendCommand(msg, bmscan);
    bmsModuleManager.getAllVoltTemp();
    Bms::updateAlarms(settings);
    Bms::updateStatus();

    Bms::broadcastStatus(settings);
}

//Boot goes to ready
//Ready can go to Precharge
//Precharge can go to Charge or Drive
void Bms::updateStatus() {
  switch (status) {
    case Boot: 
      status = Ready;
      break;
    case Ready:
      //check for AC input and go to charge or precharge
      if (io.isChargeEnabled()) {
        status = Precharge;
      }
      break;
    case Precharge:
      if (io.isChargeEnabled() && contactorsClosed()) {
        status = Charge;
      } else if (io.isDriveEnabled() && contactorsClosed()) {
        status = Drive;
      }
      break;
    case Drive:
      break;
    case Charge:
      break;
    case Error:
      break;
  }
}

void Bms::canRead(int canInterfaceOffset, int idOffset)
{
  
  if (bmscan.read(inMsg, canInterfaceOffset)) {
    //TODO::this can probably be better
    if (inMsg.id < 0x300)//do VW BMS magic if ids are ones identified to be modules
    {
        inMsg.id = inMsg.id + idOffset;
        bmsModuleManager.decodecan(inMsg, 0); //do VW BMS if ids are ones identified to be modules
    }

    if ((inMsg.id & 0x1FFFFFFF) < 0x1A5554F0 && (inMsg.id & 0x1FFFFFFF) > 0x1A555400)   // Determine if ID is Temperature CAN-ID
    {
        inMsg.id = inMsg.id + idOffset/4; // the temps only require offsetting id by 8 (1/4 of 32) i.e. 1 can id per slave. 
        bmsModuleManager.decodetemp(inMsg, 0);
    }

    if (settings.carCanIndex == canInterfaceOffset) {
      outlanderCharger.processMessage(inMsg);
      //from inverter
      if (inMsg.id == 0x02) {
        inverterLastRec = millis();
        inverterStatus = inMsg.buf[0];
      }
    }
  }
  
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

  msg.id  = 0x355;
  msg.len = 8;
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

  msg.buf[6] = lowByte(status);
  msg.buf[7] = highByte(status);
  bmscan.write(msg, settings.carCanIndex);

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

  delay(2);
  msg.id  = 0x35A;
  msg.len = 8;
  msg.buf[0] = Bms::alarm[0];//High temp  Low Voltage | High Voltage
  msg.buf[1] = Bms::alarm[1]; // High Discharge Current | Low Temperature
  msg.buf[2] = Bms::alarm[2]; //Internal Failure | High Charge current
  msg.buf[3] = Bms::alarm[3];// Cell Imbalance
  msg.buf[4] = Bms::warning[0];//High temp  Low Voltage | High Voltage
  msg.buf[5] = Bms::warning[1];// High Discharge Current | Low Temperature
  msg.buf[6] = Bms::warning[2];//Internal Failure | High Charge current
  msg.buf[7] = Bms::warning[3];// Cell Imbalance
  bmscan.write(msg, settings.carCanIndex);


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

}

OutlanderCharger& Bms::getOutlanderCharger() {
  Serial.print("Get: ");
  Serial.println(outlanderCharger.evse_duty, HEX);
  return outlanderCharger;
}

BMSModuleManager& Bms::getBMSModuleManager() {
    return bmsModuleManager;
}

IO& Bms::getIO() {
  return io;
}

byte Bms::getStatus() {
  return status;
}

bool Bms::contactorsClosed() {
  // if inverter in RUN mode
  if (inverterStatus == 0x01) {
    return true;
  }
  return false;
}

void Bms::updateAlarms(const EEPROMSettings& settings) {
  Bms::alarm[0] = 0x00;
  if (settings.overVSetpoint < bmsModuleManager.getHighCellVolt())
  {
    Bms::alarm[0] = 0x04;
  }
  if (bmsModuleManager.getLowCellVolt() < settings.underVSetpoint)
  {
    Bms::alarm[0] |= 0x10;
  }
  if (bmsModuleManager.getHighTemperature() > settings.overTSetpoint)
  {
    Bms::alarm[0] |= 0x40;
  }
  Bms::alarm[1] = 0;
  if (bmsModuleManager.getLowTemperature() < settings.underTSetpoint)
  {
    Bms::alarm[1] = 0x01;
  }
  Bms::alarm[3] = 0;
  if ((bmsModuleManager.getHighCellVolt() - bmsModuleManager.getLowCellVolt()) > settings.cellGapAlarm)
  {
    Bms::alarm[3] = 0x01;
  }

  ///warnings///
  Bms::warning[0] = 0;

  if (bmsModuleManager.getHighCellVolt() > (settings.overVSetpoint - settings.warnOffset))
  {
    Bms::warning[0] = 0x04;
  }
  if (bmsModuleManager.getLowCellVolt() < (settings.underVSetpoint + settings.warnOffset))
  {
    Bms::warning[0] |= 0x10;
  }

  if (bmsModuleManager.getHighTemperature() > (settings.overTSetpoint - settings.warnTempOffset))
  {
    Bms::warning[0] |= 0x40;
  }
  Bms::warning[1] = 0;
  if (bmsModuleManager.getLowTemperature() < (settings.underTSetpoint + settings.warnTempOffset))
  {
    Bms::warning[1] = 0x01;
  }
}