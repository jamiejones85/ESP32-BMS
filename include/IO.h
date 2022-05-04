#ifndef IO_H
#define IO_H

#define CHARGE_IN 39
#define DRIVE_IN 36

class IO
{
  public:
    void setup();
    bool isChargeEnabled();
    bool isDriveEnabled(bool inverterInForwardReverse);
    void setChargeOverride(bool override);
    bool getChargeOverride();
  private:
    bool chargeOverride;
};

#endif