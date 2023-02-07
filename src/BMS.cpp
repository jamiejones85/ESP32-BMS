#include "BMS.h"
#include "BMSCan.h"
#include "BMSModuleManager.h"
#include "OutlanderCharger.h"
#include "Shunt.h"

BmsCan bmscan;
BMS_CAN_MESSAGE msg;
BMS_CAN_MESSAGE inMsg;
Shunt shunt;
int cellspresent = 0;
String errorReason;

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
  bmsModuleManager.setBalanceHyst(settings.balanceHyst / 1000);

  bmscan.begin(500000, 0);
  bmscan.begin(500000, 1);
}

void Bms::execute() {
    Bms::canRead(0, 0);

    if (settings.carCanIndex > 0) {
      Bms::canRead(1, 0);
    }
    

    if(inverterLastRec + 200 < millis()) {
      inverterStatus = 0;
      inverterInForwardReverse = false;
    }

    if (cellspresent != bmsModuleManager.seriescells())
      {
        status = Error;
        errorReason = "Serial Cell Count Error";
      }
}

void Bms::ms1000Task() {
  Bms::printSummary();
  //comms check
  if (bmsModuleManager.checkcomms()) {
    if (status == Error) {
      status = Boot;
    }
  } else {
    status = Error;
    errorReason = "BMS Communication timeout";
  }
}

void Bms::ms500Task(const EEPROMSettings& settings) {
    bmsModuleManager.sendCommand(msg, bmscan);
    if (cellspresent == 0) {
          cellspresent = bmsModuleManager.seriescells();
    }

    bmsModuleManager.getAllVoltTemp();
    if (balanceCells == true)
    {
        bmsModuleManager.balanceCells(msg, bmscan, 0);//1 is debug
    }
    Bms::updateAlarms(settings);
    Bms::updateStatus();

    Bms::broadcastStatus(settings);

    if (status == Charge) {
      outlanderCharger.doCharge(settings, bmsModuleManager, msg, bmscan);
    }
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
      if (io.isChargeEnabled() || io.isDriveEnabled(inverterInForwardReverse)) {
        status = Precharge;
      }
      if (bmsModuleManager.getHighCellVolt() > (settings.balanceVoltage / 1000) && bmsModuleManager.getHighCellVolt() > bmsModuleManager.getLowCellVolt() + (settings.balanceHyst / 1000)) {
        balanceCells = true;
      } else {
        balanceCells = false;
      }
      break;
    case Precharge:
      if (io.isChargeEnabled() && contactorsClosed()) {
        status = Charge;
      } else if (io.isDriveEnabled(inverterInForwardReverse) && contactorsClosed()) {
        status = Drive;
      }
      break;
    case Drive:
      break;
    case Charge:
      if (!io.isChargeEnabled() || !contactorsClosed()) {
        status = Ready;
      }
      if (bmsModuleManager.getHighCellVolt() > (settings.balanceVoltage/ 1000)) {
        balanceCells = true; 
      } else {
        balanceCells = false;
      }
      if (outlanderCharger.isDoneCharging(settings, bmsModuleManager)) {
        status = Ready;
      }
      break;
    case Error:
      if (bmsModuleManager.getLowCellVolt() >= (settings.underVSetpoint/ 1000) && cellspresent == settings.seriesCells) {
        errorReason = "";
        status = Ready;
      }
      balanceCells = false;
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
      shunt.process(inMsg);
      outlanderCharger.processMessage(inMsg);
      //from inverter
      if (inMsg.id == 0x02) {
        inverterLastRec = millis();
        inverterStatus = inMsg.buf[0];
      } else if (inMsg.id == 0x01) {
        //CANIO in open inverter
        if ((inMsg.buf[1] & 0x80) == 0x80 || (inMsg.buf[2] & 0x01) == 0x01) {
          inverterInForwardReverse = true;
        } else {
          inverterInForwardReverse = false;
        }
      }
    }
  }
  
}

void Bms::broadcastStatus(const EEPROMSettings& settings) {
  msg.id  = 0x351;
  msg.len = 8;

  msg.buf[0] = lowByte(uint16_t((settings.chargeVsetpoint * settings.seriesCells ) * 10));
  msg.buf[1] = highByte(uint16_t((settings.chargeVsetpoint * settings.seriesCells ) * 10));
  // msg.buf[2] = lowByte(chargecurrent); workout limits
  // msg.buf[3] = highByte(chargecurrent);
  // msg.buf[4] = lowByte(discurrent);
  // msg.buf[5] = highByte(discurrent);
  msg.buf[6] = lowByte(uint16_t((settings.dischVsetpoint * settings.seriesCells) * 10));
  msg.buf[7] = highByte(uint16_t((settings.dischVsetpoint * settings.seriesCells) * 10));

  bmscan.write(msg, settings.carCanIndex);

  int soc = shunt.getStateOfCharge(settings);
  msg.id  = 0x355;
  msg.len = 8;
  msg.buf[0] = lowByte(soc);
  msg.buf[1] = highByte(soc);
  msg.buf[2] = lowByte(50); //soh, no idea about this for the VW modules
  msg.buf[3] = highByte(50);  //soh, no idea about this for the VW modules
  msg.buf[4] = lowByte(soc * 10);
  msg.buf[5] = highByte(soc * 10);
  msg.buf[6] = lowByte(status);
  msg.buf[7] = highByte(status);
  bmscan.write(msg, settings.carCanIndex);

  ShuntData data = shunt.getData();
  msg.id  = 0x356;
  msg.len = 8;
  msg.buf[0] = lowByte(uint16_t(bmsModuleManager.getPackVoltage() * 100));
  msg.buf[1] = highByte(uint16_t(bmsModuleManager.getPackVoltage() * 100));
  msg.buf[2] = lowByte(long(data.milliamps / 100));
  msg.buf[3] = highByte(long(data.milliamps / 100));
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
  // Serial.print("Get: ");
  // Serial.println(outlanderCharger.evse_duty, HEX);
  return outlanderCharger;
}

BMSModuleManager& Bms::getBMSModuleManager() {
    return bmsModuleManager;
}

IO& Bms::getIO() {
  return io;
}

Shunt& Bms::getShunt() {
  return shunt;
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
  if ((settings.overVSetpoint / 1000) < bmsModuleManager.getHighCellVolt())
  {
    Bms::alarm[0] = 0x04;
  }
  if (bmsModuleManager.getLowCellVolt() < (settings.underVSetpoint/ 1000))
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

  if (bmsModuleManager.getHighCellVolt() > ((settings.overVSetpoint / 1000) - settings.warnOffset))
  {
    Bms::warning[0] = 0x04;
  }
  if (bmsModuleManager.getLowCellVolt() < ((settings.underVSetpoint / 1000) + settings.warnOffset))
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

bool Bms::getBalanceCells() {
  return balanceCells;
}

void Bms::printSummary() {
  Serial.print("BMS Status: ");

  switch (status)
    {
      case (Boot):
        Serial.print("Boot");
        break;

      case (Ready):
        Serial.print("Ready");
        break;

      case (Precharge):
        Serial.print("Precharge");
        break;

      case (Drive):
        Serial.print("Drive");
        break;

      case (Charge):
        Serial.print("Charge");
        break;

      case (Error):
        Serial.print("Error");
        break;
    }
  
  Serial.print("  ");
  if (io.isChargeEnabled()) {
    Serial.print("| AC Present");
  }
  
  if (balanceCells == true)
  {
    Serial.print("| Balancing Active ");
  }
  Serial.println("  ");
  if (status == Error) {
    Serial.print("Reason: ");
    Serial.println(errorReason);
  }
  bmsModuleManager.printPackSummary();

}