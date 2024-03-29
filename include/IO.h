#ifndef IO_H
#define IO_H

#include "Config.h"

// Input 1 (GPIO 36) is a analog pin, when there is no conection the voltage on the pin shall be 2.3 volts, this will corispond to an adc value of 2750 
// when there is a 13a granny charger it it 1.94v and an adc value of 2200, therefore any value less than 2500 shall be pluged in.
// SO IF LESS THAN 50 IT ASSUMED NOT TO BE WIRED IN
// see IEC61851_J1772.png
// https://randomnerdtutorials.com/esp32-pinout-reference-gpios/
// Input 2 (GPIO 39) is an analog pin Use TBC (Moisture detection?)
// Input 3 (GPIO 34) is a digital pin to be conected to the cars ignition
// Input 4 (GPIO 35) is a digital pin and is to be used as the alternative chrge in pin, where high is pluged in

#define CHARGE_IN 36
#define DIN1 34
#define DIN2 35

class IO
{
  public:
    IO(EEPROMSettings& settings);
    void setup();
    bool isChargeEnabled();
    bool isDriveEnabled(bool inverterInForwardReverse);
    void setChargeOverride(bool override);
    bool getChargeOverride();
  private:
    EEPROMSettings& settings;
    bool chargeOverride;
};

#endif