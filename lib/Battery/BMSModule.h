#ifndef BMS_MODULE_H
#define BMS_MODULE_H
#define MAX_MODULE_ADDR     0x3E

#include "BmsCanMessage.h"
#include "ISerial.h"
#include "IMillis.h"

class BMSModule
{
  public:
    BMSModule();
    void setSerial(ISerial *s);
    void setMillis(IMillis *m);

    void decodecan(int Id, BMS_CAN_MESSAGE &msg);
    void decodetemp(BMS_CAN_MESSAGE &msg);
    void clearmodule();
    int getscells();
    float getCellVoltage(int cell);
    float getLowCellV();
    float getHighCellV();
    float getAverageV();
    float getLowTemp();
    float getHighTemp();
    float getHighestModuleVolt();
    float getLowestModuleVolt();
    float getHighestCellVolt(int cell);
    float getLowestCellVolt(int cell);
    float getHighestTemp();
    float getLowestTemp();
    float getAvgTemp();
    float getModuleVoltage();
    float getTemperature(int temp);
    uint8_t getFaults();
    uint8_t getAlerts();
    uint8_t getCOVCells();
    uint8_t getCUVCells();
    void setAddress(int newAddr);
    int getAddress();
    int getBalStat();
    bool isExisting();
    bool isReset();
    void setReset(bool ex);
    void setExists(bool ex);
    void settempsensor(int tempsensor);
    void setIgnoreCell(float Ignore);
    void setDelta(float ex);


  private:
    float cellVolt[13];          // calculated as 16 bit value * 6.250 / 16383 = volts
    float lowestCellVolt[13];
    float highestCellVolt[13];
    float moduleVolt;          // calculated as 16 bit value * 33.333 / 16383 = volts
    float temperatures[3];     // Don't know the proper scaling at this point
    float lowestTemperature;
    float highestTemperature;
    float lowestModuleVolt;
    float highestModuleVolt;
    float IgnoreCell;
    float VoltDelta;
    bool exists;
    bool reset;
    int alerts;
    int faults;
    int COVFaults;
    int CUVFaults;
    int sensor;
    uint8_t moduleAddress;     //1 to 0x3E
    int scells;
    uint32_t balstat;
    uint32_t lasterror;
    uint8_t cmuerror;
    uint32_t timeout;

    ISerial *serial;
    IMillis *millis;

};
#endif