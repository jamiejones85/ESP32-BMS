#include "Shunt.h"

Shunt::Shunt(IBmsCan *bms, EEPROMSettings *s) {
   bmsCanRef = bms;
   settings = s;
}

Shunt::Shunt() {
}

void Shunt::setSettings(EEPROMSettings *s) {
   this->settings = s;
}
void Shunt::setBmsCan(IBmsCan *can) {
  this->bmsCanRef = can;
}

int Shunt::getStateOfCharge() {
  return ((settings->useableAh - data.amphours) / (settings->useableAh) ) * 100;
}

ShuntData Shunt::getData() {
    return data;
}

void Shunt::resetCounters(BMS_CAN_MESSAGE &msg) {
  msg.id  = 0x411;
  msg.len = 8;
  msg.buf[0] = 0x3F;
  msg.buf[1] = 0x00;
  msg.buf[2] = 0x00;
  msg.buf[3] = 0x00;
  msg.buf[4] = 0x00;
  msg.buf[5] = 0x00;
  msg.buf[6] = 0x00;
  msg.buf[7] = 0x00;
  bmsCanRef->write(msg, settings->carCanIndex);
}

void Shunt::process(BMS_CAN_MESSAGE &inMsg) {
    if (inMsg.id == 0x527) {
        long ampseconds = inMsg.buf[2] + (inMsg.buf[3] << 8) + (inMsg.buf[4] << 16) + (inMsg.buf[5] << 24);
        data.amphours = ampseconds/3600.0f;
    } else if (inMsg.id == 0x521) {
        data.milliamps = inMsg.buf[2] + (inMsg.buf[3] << 8) + (inMsg.buf[4] << 16) + (inMsg.buf[5] << 24);
     } else if (inMsg.id == 0x522) {
        data.voltage1 = inMsg.buf[2] + (inMsg.buf[3] << 8) + (inMsg.buf[4] << 16) + (inMsg.buf[5] << 24);
     } else if (inMsg.id == 0x523) {
        data.voltage2 = inMsg.buf[2] + (inMsg.buf[3] << 8) + (inMsg.buf[4] << 16) + (inMsg.buf[5] << 24);
     } else if (inMsg.id == 0x526) {
        data.watts = inMsg.buf[2] + (inMsg.buf[3] << 8) + (inMsg.buf[4] << 16) + (inMsg.buf[5] << 24);
     } else if (inMsg.id == 0x527) {
        long ampseconds = inMsg.buf[2] + (inMsg.buf[3] << 8) + (inMsg.buf[4] << 16) + (inMsg.buf[5] << 24);
        data.amphours = ampseconds/3600.0f;
     } else if (inMsg.id == 0x528) {
        long wh = inMsg.buf[2] + (inMsg.buf[3] << 8) + (inMsg.buf[4] << 16) + (inMsg.buf[5] << 24);
        data.kilowatthours = wh/1000.0f;
     }
}