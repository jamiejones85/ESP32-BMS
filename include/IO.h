#ifndef IO_H
#define IO_H

// Input 1 (GPIO 36) is a analog pin, when there is no conection the voltage on the pin shall be 2.3 volts, this will corispond to an adc value of 2750 
// when there is a 13a granny charger it it 1.94v and an adc value of 2200, therefore any value less than 2500 shall be pluged in.
// SO IF LESS THAN 50 IT ASSUMED NOT TO BE WIRED IN
// see IEC61851_J1772.png
// https://randomnerdtutorials.com/esp32-pinout-reference-gpios/
// Input 2 (GPIO 39) is an analog pin Use TBC (Moisture detection?)
// Input 3 (GPIO 34) is a digital pin to be conected to the cars ignition
// Input 4 (GPIO 35) is a digital pin and is to be used as the alternative chrge in pin, where high is pluged in

#define CHARGE_IN 36
#define CHARGE_IN_ALT 35
#define DRIVE_IN 34

class IO
{
  public:
    void setup();
    bool isChargeEnabled();
    double ChargeADC();
    bool isDriveEnabled(bool inverterInForwardReverse);
    void setChargeOverride(bool override);
    bool getChargeOverride();
  private:
    bool chargeOverride;
};

#endif