#include "IO.h"
#include "Arduino.h"

void IO::setup() {
    //Digital input (+12v) for charge enabled
    //pinMode(CHARGE_IN_ALT, INPUT);
    chargeOverride = false;
    //digital input for drive (switched live from ignition, that doesnt go live on charge)
    pinMode(DRIVE_IN, INPUT);
}

void IO::setChargeOverride(bool override) {
    chargeOverride = override;
}

bool IO::getChargeOverride() {
    return chargeOverride;
}

bool IO::isChargeEnabled() {
    //return analogRead(CHARGE_IN) >50 || analogRead(CHARGE_IN) <2750 || digitalRead(CHARGE_IN_ALT) || chargeOverride == true;
    return !digitalRead(CHARGE_IN_ALT) || chargeOverride == true;
    
}

bool IO::isDriveEnabled(bool inverterInForwardReverse) {
    //read pin or inverter status in forward/reverse
    return !digitalRead(DRIVE_IN) || inverterInForwardReverse == true;
}

double IO::ChargeADC() 
{
  double AverageValue = 0;
  int MeasurementsToAverage = 16;
  for(int i = 0; i < MeasurementsToAverage; ++i)
  {
    AverageValue += analogRead(CHARGE_IN);
    //delay(1);
  }
  AverageValue /= MeasurementsToAverage;
  if(AverageValue < 1 || AverageValue > 4095) return 0;
  //https://github.com/G6EJD/ESP32-ADC-Accuracy-Improvement-function/blob/master/ESP32_ADC_Read_Voltage_Accurate.ino   
  // factor of 2 to corect volatge divider   
return 2*(-0.000000000000016 * pow(AverageValue,4) + 0.000000000118171 * pow(AverageValue,3)- 0.000000301211691 * pow(AverageValue,2)+ 0.001109019271794 * AverageValue + 0.034143524634089);
//return 2*(-0.000000000009824 * pow(AverageValue,3) + 0.000000016557283 * pow(AverageValue,2) + 0.000854596860691 * AverageValue + 0.065440348345433);
}