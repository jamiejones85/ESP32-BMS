#ifndef BMS_MODULE_MANAGER_H
#define BMS_MODULE_MANAGER_H

#define CONTROLID 0x0BA;

#include "config.h"
#include "BMSModule.h"
#include "BMSCan.h"
#include "ArduinoJson.h"

class BMSModuleManager
{
  public:
    BMSModuleManager();
    int seriescells();
    void clearmodules();
    void decodecan(BMS_CAN_MESSAGE &msg, int debug);
    void decodetemp(BMS_CAN_MESSAGE &msg, int debug);
    void getAllVoltTemp();
    void readSetpoints();
    void setBatteryID(int id);
    void setPstrings(int Pstrings);
    void setUnderVolt(float newVal);
    void setOverVolt(float newVal);
    void setOverTemp(float newVal);
    void setBalanceV(float newVal);
    void setBalanceHyst(float newVal);
    void setSensors(int sensor, float Ignore, float VoltDelta);
    float getPackVoltage();
    float getAvgTemperature();
    float getHighTemperature();
    float getLowTemperature();
    float getAvgCellVolt();
    float getLowCellVolt();
    float getHighCellVolt();
    float getHighVoltage();
    float getLowVoltage();
    /*
      void processCANMsg(CAN_FRAME &frame);
    */
    void printAllCSV(unsigned long timestamp, float current, int SOC);
    void printPackSummary();
    void printPackDetails(int digits);
    void printPackDetailsJson(DynamicJsonDocument &root);

    int getNumModules();
    bool checkcomms();
    void sendCommand(BMS_CAN_MESSAGE &msg, BmsCan &bmscan);

  private:
    float packVolt;                         // All modules added together
    int Pstring;
    float LowCellVolt;
    float HighCellVolt;
    float lowestPackVolt;
    float highestPackVolt;
    float lowestPackTemp;
    float highestPackTemp;
    float highTemp;
    float lowTemp;
    BMSModule modules[MAX_MODULE_ADDR + 1]; // store data for as many modules as we've configured for.
    int batteryID;
    int numFoundModules;                    // The number of modules that seem to exist
    bool isFaulted;
    int spack;

};
#endif