#include "BMSWebServer.h"
#include "SPIFFS.h"

AsyncWebServer server(80);

void BMSWebServer::setup(EEPROMSettings &settings, Config &config, BMSModuleManager &bmsModuleManager)
{

    server.on("/voltages", HTTP_GET, [&] (AsyncWebServerRequest *request) {
        AsyncResponseStream *response = request->beginResponseStream("application/json");
        DynamicJsonDocument json(10240);

        bmsModuleManager.printPackDetailsJson(json);
        serializeJson(json, *response);
        request->send(response);
    });

    server.on("/config", HTTP_GET, [&] (AsyncWebServerRequest *request) {
        AsyncResponseStream *response = request->beginResponseStream("application/json");
        DynamicJsonDocument json(2048);

        config.toJson(settings, json);
        serializeJson(json, *response);
        request->send(response);
    });

    server.on(
        "/config",
        HTTP_POST,
        [](AsyncWebServerRequest * request){},
        NULL,
        [&](AsyncWebServerRequest * request, uint8_t *data, size_t len, size_t index, size_t total) {
    
        const size_t JSON_DOC_SIZE = 512U;
        DynamicJsonDocument jsonDoc(JSON_DOC_SIZE);
        
        if (DeserializationError::Ok == deserializeJson(jsonDoc, (const char*)data))
        {
            JsonObject obj = jsonDoc.as<JsonObject>();
            settings = config.fromJson(obj);
            config.save(settings);
        }
        request->send(200, "application/json", "success");
    });

    server.serveStatic("/", SPIFFS, "/").setDefaultFile("index.html");
  

  // Start server
  server.begin();
}