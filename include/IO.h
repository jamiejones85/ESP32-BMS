#ifndef IO_H
#define IO_H

#define CHARGE_IN 39
class IO
{
  public:
    void setup();
    bool isChargeEnabled();
    bool isDriveEnabled();
    void setChargeOverride(bool override);
    bool getChargeOverride();
  private:
    bool chargeOverride;
};

#endif