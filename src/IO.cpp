#include "IO.h"
#include "Arduino.h"

IO::IO(EEPROMSettings& settings): settings { settings } {

}

void IO::setup() {
    chargeOverride = false;
    //digital input for drive (switched live from ignition, that doesnt go live on charge)
    pinMode(DIN1, INPUT);
    pinMode(DIN2, INPUT);

}

void IO::setChargeOverride(bool override) {
    chargeOverride = override;
}

bool IO::getChargeOverride() {
    return chargeOverride;
}

bool IO::isChargeEnabled() {

    if(chargeOverride) {
        return true;
    }

    if (settings.acDetectionMethod == AC_METHOD_D1_HIGH) {
        //need to invert, low means 12v connected
        return !digitalRead(DIN1);
    }

    if (settings.acDetectionMethod == AC_METHOD_D2_HIGH) {
        //need to invert, low means 12v connected
        return !digitalRead(DIN2);
    }
    if (settings.acDetectionMethod == AC_METHOD_J1772) {
        //TODO: move to a private method
        int averageValue = 0;
        float ppVoltage = 0 ;
        int measurementsToAverage = 16;
        for(int i = 0; i < measurementsToAverage; ++i) {
            averageValue += analogRead(CHARGE_IN);
            //delay(1);
        }
        averageValue /= measurementsToAverage;
        if(averageValue < 1 || averageValue > 4095) {
            return 0;
        }

        //https://github.com/G6EJD/ESP32-ADC-Accuracy-Improvement-function/blob/master/ESP32_ADC_Read_Voltage_Accurate.ino   
        //factor of 2 to corect volatge divider on board  
        ppVoltage = int(1000* 2*(-0.000000000000016 * pow(averageValue,4) + 0.000000000118171 * pow(averageValue,3)- 0.000000301211691 * pow(averageValue,2)+ 0.001109019271794 * averageValue + 0.034143524634089));
        // Refer to "J1772_IEC61851.ods" in docs folder for calc For J1772, AC is avaialbe when the voltage on the PP detect pin is between 1.5v and 1.7v to allow for inaccuracies we are going to say between 1.5v and 2.0v
        // above and below this value we will say no ac avaialabe. Note PP_Voltage is mV.
        return ppVoltage > 1500 && ppVoltage < 2000;
    
    }
    return false;

}

bool IO::isDriveEnabled(bool inverterInForwardReverse) {
    //DOTO::use configuration method
    //read pin or inverter status in forward/reverse
    return !digitalRead(DIN1) || inverterInForwardReverse == true;
}

