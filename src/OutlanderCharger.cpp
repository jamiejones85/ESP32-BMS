#include "OutlanderCharger.h"

OutlanderCharger::OutlanderCharger(EEPROMSettings& settings, BMSModuleManager& bmsModuleManager,  BmsCan &bmscan):
  settings { settings }, bmsModuleManager { bmsModuleManager }, bmscan { bmscan } {

}

bool OutlanderCharger::isDoneCharging() {
    return chargeAmps == 0;
}
void OutlanderCharger::processMessage(BMS_CAN_MESSAGE &inMsg) {
    if (inMsg.id == 0x389) {
      reported_voltage = inMsg.buf[0] * 2;
      reported_dc_current = inMsg.buf[2];
      reported_temp1 = inMsg.buf[3] - 40;
      reported_temp2 = inMsg.buf[4] - 40;
      timeout = millis();

    } else if (inMsg.id == 0x38A) {
       reported_status = inMsg.buf[4];
       evse_duty = inMsg.buf[3];
    }
}

void OutlanderCharger::doCharge(BMS_CAN_MESSAGE &msg) {
    chargeAmps = calculateCurrent();
    if (chargeAmps > 0) {

        msg.id = 0x285;
        msg.len = 8;
        msg.buf[0] = 0x0;
        msg.buf[1] = 0x0;
        msg.buf[2] = 0xb6;
        msg.buf[3] = 0x0;
        msg.buf[4] = 0x0;
        msg.buf[5] = 0x0;
        msg.buf[6] = 0x0;
        bmscan.write(msg, settings.carCanIndex);

        
        msg.id  = 0x286;
        msg.len = 8;
        msg.buf[0] = highByte(uint16_t(settings.chargeVsetpoint * settings.seriesCells * 10));//volage
        msg.buf[1] = lowByte(uint16_t(settings.chargeVsetpoint * settings.seriesCells * 10));
        msg.buf[2] = lowByte(chargeAmps);
        msg.buf[3] = 0x0;
        msg.buf[4] = 0x0;
        msg.buf[5] = 0x0;
        msg.buf[6] = 0x0;
        bmscan.write(msg, settings.carCanIndex);
    }
}

int OutlanderCharger::calculateCurrent() {

    int chargecurrent = settings.chargecurrentmax;
    float chargeHyVolts = settings.chargeHys / 1000;
    float chargeVsetpoint = settings.chargeVsetpoint / 1000;


      ///////All hard limits to into zeros
    if (bmsModuleManager.getLowTemperature() < settings.underTSetpoint)
    {
      chargecurrent = 0;
    }
    if (bmsModuleManager.getHighTemperature() > settings.overTSetpoint)
    {
      chargecurrent = 0;
    }
    if (bmsModuleManager.getHighCellVolt() > settings.overVSetpoint)
    {
      chargecurrent = 0;
    }
    if (bmsModuleManager.getHighCellVolt() > settings.overVSetpoint)
    {
      chargecurrent = 0;
    }

    //Modifying Charge current///
    if (chargecurrent > 0)
    {
      //Temperature based///
      if (bmsModuleManager.getLowTemperature() < settings.chargeTSetpoint) {
        chargecurrent = chargecurrent - map(bmsModuleManager.getLowTemperature(), settings.underTSetpoint, settings.chargeTSetpoint, (settings.chargecurrentmax), 0);
      }

      //taper based on highest cell, matching how it's done in simpBMS for now.
      if (bmsModuleManager.getHighCellVolt() > (chargeVsetpoint - chargeHyVolts)) {
        chargecurrent = chargecurrent - map(bmsModuleManager.getHighCellVolt(), (chargeVsetpoint - chargeHyVolts), chargeVsetpoint, 0, (settings.chargecurrentmax - settings.chargeCurrentEnd));
      }
    
    }

    if (chargecurrent < 0) {
        chargecurrent = 0;
    }
    

    Serial.print("Charge Current: ");
    Serial.println(chargecurrent);
    return chargecurrent;
}