#ifndef BMS_WEB_SERVER_H
#define BMS_WEB_SERVER_H
#include "ESPAsyncWebServer.h"
#include "AsyncJson.h"
#include "ArduinoJson.h"
#include "Config.h"
#include "Bms.h"

class BMSWebServer
{
  public:
    void setup(EEPROMSettings &settings, Config &config, Bms &bms);
  private:

};
#endif