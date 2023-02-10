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
    BMSWebServer(EEPROMSettings& settings);
    void setup(Config &config, Bms &bms);
    void execute();
    void broadcast(const char * message);
    AsyncWebSocket& getWebSocket();
  private:
    EEPROMSettings& settings;
};
#endif