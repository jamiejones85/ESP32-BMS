#ifndef BMS_WEB_SERVER_H
#define BMS_WEB_SERVER_H
#include "ESPAsyncWebServer.h"
#include "AsyncJson.h"
#include "ArduinoJson.h"
#include "Config.h"
#include "BMS.h"

class BMSWebServer
{
  public:
    void setup(EEPROMSettings &settings, Config &config, Bms &bms);
    void execute();
    void broadcast(const char * message);
    AsyncWebSocket& getWebSocket();
  private:

};
#endif