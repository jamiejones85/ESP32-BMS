#include "BMSWebServer.h"
#include "SPIFFS.h"
#include "Shunt.h"

AsyncWebServer server(80);
AsyncWebSocket ws("/ws");
AsyncEventSource events("/events");

AsyncWebSocket& BMSWebServer::getWebSocket() {
    return ws;
}
void BMSWebServer::execute() {
    ws.cleanupClients();
}

void BMSWebServer::broadcast(const char * message) {
    ws.printfAll(message);
}

void BMSWebServer::setup(EEPROMSettings &settings, Config &config, Bms &bms)
{
    // ws.onEvent(onWsEvent);
    server.addHandler(&ws);

    server.on("/wifi", [&] (AsyncWebServerRequest *request) {
        bool updated = true;
        if(request->hasParam("apSSID", true) && request->hasParam("apPW", true)) 
        {
            WiFi.softAP(request->arg("apSSID").c_str(), request->arg("apPW").c_str());
        }
        else if(request->hasParam("staSSID", true) && request->hasParam("staPW", true)) 
        {
            WiFi.mode(WIFI_AP_STA);
            WiFi.begin(request->arg("staSSID").c_str(), request->arg("staPW").c_str());
        }
        else
        {
            File file = SPIFFS.open("/wifi.html", "r");
            String html = file.readString();
            file.close();
            html.replace("%staSSID%", WiFi.SSID());
            html.replace("%apSSID%", WiFi.softAPSSID());
            html.replace("%staIP%", WiFi.localIP().toString());
            request->send(200, "text/html", html);
            updated = false;
        }

        if (updated)
        {
            request->send(SPIFFS, "/wifi-updated.html");
        } 
    });

    server.on("/dashboard", HTTP_GET, [&] (AsyncWebServerRequest *request) {
        BMSModuleManager& bmsModuleManager = bms.getBMSModuleManager();
        OutlanderCharger& outlanderCharger = bms.getOutlanderCharger();
        ShuntData data = bms.getShunt().getData();
        IO io = bms.getIO();
        AsyncResponseStream *response = request->beginResponseStream("application/json");
        DynamicJsonDocument json(10240);

        json["packVolts"] = bmsModuleManager.getPackVoltage();
        json["minVolt"] = bmsModuleManager.getLowCellVolt();
        json["maxVolt"] = bmsModuleManager.getHighCellVolt();
        json["cellDelta"] = bmsModuleManager.getHighCellVolt() - bmsModuleManager.getLowCellVolt();
        json["avgTemp"] = bmsModuleManager.getAvgTemperature();
        json["evseDuty"] = outlanderCharger.evse_duty;
        json["chargerStatus"] = outlanderCharger.reported_status;
        json["chargerTemp1"] = outlanderCharger.reported_temp1;
        json["chargerTemp2"] = outlanderCharger.reported_temp2;
        json["chargerVoltage"] = outlanderCharger.reported_voltage;
        json["chargerCurrent"] = outlanderCharger.reported_dc_current;
        json["chargerRequestedCurrent"] = outlanderCharger.request_current;
        json["contactorStatus"] = bms.contactorsClosed();
        json["chargeEnabled"] = io.isChargeEnabled();
        json["chargeOverride"] = io.getChargeOverride();
        json["ahUsed"] = data.amphours;
        json["soc"] = bms.getShunt().getStateOfCharge(settings);
        json["balanceActive"] = bms.getBalanceCells();
        json["test"] = (settings.balanceVoltage / 1000);
        byte status = bms.getStatus();
        if (status == Boot) {
            json["status"] = "Boot";
        } else if (status == Ready) {
            json["status"] = "Ready";
        } else if (status == Drive) {
            json["status"] = "Drive";
        } else if (status == Charge) {
            json["status"] = "Charge";
        } else if (status == Precharge) {
            json["status"] = "Precharge";
        } else if (status == Error) {
            json["status"] = "Error";
        }

        serializeJson(json, *response);
        request->send(response);
    });

    server.on("/voltages", HTTP_GET, [&] (AsyncWebServerRequest *request) {
        BMSModuleManager& bmsModuleManager = bms.getBMSModuleManager();
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
        Serial.println("Config POST");
        const size_t JSON_DOC_SIZE = 1024U;
        DynamicJsonDocument jsonDoc(JSON_DOC_SIZE);
        
        if (DeserializationError::Ok == deserializeJson(jsonDoc, (const char*)data))
        {
            JsonObject obj = jsonDoc.as<JsonObject>();
            settings = config.fromJson(obj);
            config.save(settings);
            request->send(200, "application/json", "success");

        } else {
            request->send(200, "application/json", "DeserializationError");
        }
    });

    server.on("/edit", 
        HTTP_POST, 
        [](AsyncWebServerRequest * request){},
        [&](AsyncWebServerRequest *request, const String& filename, size_t index, uint8_t *data, size_t len, bool final) {
            if (!index) {
                // open the file on first call and store the file handle in the request object
                request->_tempFile = SPIFFS.open("/" + filename, "w");

            }

            if (len) {
                // stream the incoming chunk to the opened file
                request->_tempFile.write(data, len);
            }

            if (final) {
                // close the file handle as the upload is now done
                request->_tempFile.close();

                if (filename.substring(filename.lastIndexOf(".")).equals("bin")) {
                    Serial.println("Firmware uploaded, restarting");
                    request->send(200, "application/json", "restarting");
                    ESP.restart();   
                }
                request->redirect("/");
            }
        }
        
    );

    server.on("/data", 
        HTTP_POST, 
        [](AsyncWebServerRequest * request){},
        NULL,
        [&](AsyncWebServerRequest * request, uint8_t *data, size_t len, size_t index, size_t total) {
            if(request->hasParam("chargeOverride", true)) {
                AsyncWebParameter* p = request->getParam("chargeOverride", true);
                bms.getIO().setChargeOverride(p->value().c_str() == "true");
            }

            request->send(200, "application/json", "success");

        }     
        
    );

    server.serveStatic("/", SPIFFS, "/").setDefaultFile("index.html");
  

  // Start server
  Serial.println("Starting Web Server");

  server.begin();
}