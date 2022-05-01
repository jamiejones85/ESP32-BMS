#include "Shunt.h"
#include "Arduino.h"

int Shunt::getStateOfCharge(EEPROMSettings &settings) {
  return ((settings.useableAh - amphours) / (settings.useableAh) ) * 100;

}

void Shunt::process(BMS_CAN_MESSAGE &inMsg) {
    if (inMsg.id == 0x527) {
        long ampseconds = inMsg.buf[2] + (inMsg.buf[3] << 8) + (inMsg.buf[4] << 16) + (inMsg.buf[5] << 24);
        amphours = ampseconds/3600.0f;
    } else if (inMsg.id == 0x521) {
          CANmilliamps = inMsg.buf[2] + (inMsg.buf[3] << 8) + (inMsg.buf[4] << 16) + (inMsg.buf[5] << 24);
          Serial.print("CANmilliamps ");
          Serial.println(CANmilliamps);
        //   RawCur = CANmilliamps; 
        //   getcurrent();

     } else if (inMsg.id == 0x522) {
        voltage1 = inMsg.buf[2] + (inMsg.buf[3] << 8) + (inMsg.buf[4] << 16) + (inMsg.buf[5] << 24);
     } else if (inMsg.id == 0x523) {
        voltage2 = inMsg.buf[2] + (inMsg.buf[3] << 8) + (inMsg.buf[4] << 16) + (inMsg.buf[5] << 24);
     } else if (inMsg.id == 0x526) {
        long watt = inMsg.buf[2] + (inMsg.buf[3] << 8) + (inMsg.buf[4] << 16) + (inMsg.buf[5] << 24);
        kilowatts = watt/1000.0f;
     } else if (inMsg.id == 0x527) {
        long ampseconds = inMsg.buf[2] + (inMsg.buf[3] << 8) + (inMsg.buf[4] << 16) + (inMsg.buf[5] << 24);
        amphours = ampseconds/3600.0f;
     } else if (inMsg.id == 0x528) {
        long wh = inMsg.buf[2] + (inMsg.buf[3] << 8) + (inMsg.buf[4] << 16) + (inMsg.buf[5] << 24);
        kilowatthours = wh/1000.0f;
     }
}