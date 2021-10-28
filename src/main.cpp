// Import required libraries
#include "WiFi.h"
#include "ESPAsyncWebServer.h"
#include "SPIFFS.h"
#include "BmsCan.h"
#include "BMSModuleManager.h"
#include "AsyncJson.h"
#include "ArduinoJson.h"
#include "Config.h"
#include <TaskScheduler.h>
#include <SPI.h>
#include <esp_task_wdt.h>
#include <EEPROM.h>

#define WDT_TIMEOUT 3
#define EEPROM_SIZE 512
// Replace with your network credentials
const char* ssid = "VodafoneConnect59090278_24";
const char* password = "2umz394zbf9af7s";

int controlid = 0x0BA;
int moduleidstart = 0x1CC;
int cellspresent = 0;

BmsCan bmscan;
BMS_CAN_MESSAGE msg;
BMS_CAN_MESSAGE inMsg;
BMSModuleManager bms;
EEPROMSettings settings;
Config config;

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);
Scheduler ts;

void ms20000Callback();
void ms500Callback();
Task ms500Task(500, TASK_FOREVER, &ms500Callback);
Task ms20000Task(20000, TASK_FOREVER, &ms20000Callback);

void setup(){
  EEPROM.begin(EEPROM_SIZE);
  // Serial port for debugging purposes
  Serial.begin(115200);

  // Initialize SPIFFS
  if(!SPIFFS.begin(true)){
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }

  //AP and Station Mode
  WiFi.mode(WIFI_AP_STA);
  WiFi.hostname("ESP32-BMS");
  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  int i = 0;
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }

  settings = config.load();

  // Print ESP32 Local IP Address
  Serial.println(WiFi.localIP());
  server.on("/voltages", HTTP_GET, [] (AsyncWebServerRequest *request) {
    AsyncResponseStream *response = request->beginResponseStream("application/json");
    DynamicJsonDocument json(10240);

    bms.printPackDetailsJson(json);
    serializeJson(json, *response);
    request->send(response);
  });

  server.on("/config", HTTP_GET, [] (AsyncWebServerRequest *request) {
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
    [](AsyncWebServerRequest * request, uint8_t *data, size_t len, size_t index, size_t total) {
 
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

  SPI.begin();
  bmscan.begin(500000, 0);
  bmscan.begin(500000, 1);

  ts.addTask(ms500Task);
  ms500Task.enable();

  ts.addTask(ms20000Task);
  ms20000Task.enable();

  esp_task_wdt_init(WDT_TIMEOUT, true); //enable panic so ESP32 restarts
  esp_task_wdt_add(NULL); //add current thread to WDT watch

  Serial.print("MOSI: ");
  Serial.println(MOSI);
  Serial.print("MISO: ");
  Serial.println(MISO);
  Serial.print("SCK: ");
  Serial.println(SCK);
  Serial.print("SS: ");
  Serial.println(SS);
}

//TODO::move out somewhere else
void sendcommand()
{

  msg.id  = controlid;
  msg.len = 8;
  msg.buf[0] = 0x00;
  msg.buf[1] = 0x00;
  msg.buf[2] = 0x00;
  msg.buf[3] = 0x00;
  msg.buf[4] = 0x00;
  msg.buf[5] = 0x00;
  msg.buf[6] = 0x00;
  msg.buf[7] = 0x00;
  bmscan.write(msg, 0);
  delay(1);
  msg.id  = controlid;
  msg.len = 8;
  msg.buf[0] = 0x45;
  msg.buf[1] = 0x01;
  msg.buf[2] = 0x28;
  msg.buf[3] = 0x00;
  msg.buf[4] = 0x00;
  msg.buf[5] = 0x00;
  msg.buf[6] = 0x00;
  msg.buf[7] = 0x30;
  bmscan.write(msg, 0);
}

void statusCan() //communication with Victron system over CAN
{
  // msg.id  = 0x351;
  // msg.len = 8;

  // msg.buf[0] = lowByte(uint16_t((settings.ChargeVsetpoint * settings.Scells ) * 10));
  // msg.buf[1] = highByte(uint16_t((settings.ChargeVsetpoint * settings.Scells ) * 10));
  // msg.buf[2] = lowByte(chargecurrent);
  // msg.buf[3] = highByte(chargecurrent);
  // msg.buf[4] = lowByte(discurrent );
  // msg.buf[5] = highByte(discurrent);
  // msg.buf[6] = lowByte(uint16_t((settings.DischVsetpoint * settings.Scells) * 10));
  // msg.buf[7] = highByte(uint16_t((settings.DischVsetpoint * settings.Scells) * 10));

  // bmscan.write(msg, settings.carCanIndex);

  // msg.id  = 0x355;
  // msg.len = 8;
  // if (SOCoverride != -1) {
  //   msg.buf[0] = lowByte(SOCoverride);
  //   msg.buf[1] = highByte(SOCoverride);
  //   msg.buf[2] = lowByte(SOH);
  //   msg.buf[3] = highByte(SOH);
  //   msg.buf[4] = lowByte(SOCoverride * 10);
  //   msg.buf[5] = highByte(SOCoverride * 10);
  // } else {
  //   msg.buf[0] = lowByte(SOC);
  //   msg.buf[1] = highByte(SOC);
  //   msg.buf[2] = lowByte(SOH);
  //   msg.buf[3] = highByte(SOH);
  //   msg.buf[4] = lowByte(SOC * 10);
  //   msg.buf[5] = highByte(SOC * 10);
  // }

  // msg.buf[6] = lowByte(bmsstatus);
  // msg.buf[7] = highByte(bmsstatus);
  // bmscan.write(msg, settings.carCanIndex);

  msg.id  = 0x356;
  msg.len = 8;
  msg.buf[0] = lowByte(uint16_t(bms.getPackVoltage() * 100));
  msg.buf[1] = highByte(uint16_t(bms.getPackVoltage() * 100));
  // msg.buf[2] = lowByte(long(currentact / 100)); Acutal current
  // msg.buf[3] = highByte(long(currentact / 100));
  msg.buf[4] = lowByte(int16_t(bms.getAvgTemperature() * 10));
  msg.buf[5] = highByte(int16_t(bms.getAvgTemperature() * 10));
  msg.buf[6] = lowByte(uint16_t(bms.getAvgCellVolt() * 1000));
  msg.buf[7] = highByte(uint16_t(bms.getAvgCellVolt() * 1000));
  bmscan.write(msg, settings.carCanIndex);

  delay(2);
  // msg.id  = 0x35A;
  // msg.len = 8;
  // msg.buf[0] = alarm[0];//High temp  Low Voltage | High Voltage
  // msg.buf[1] = alarm[1]; // High Discharge Current | Low Temperature
  // msg.buf[2] = alarm[2]; //Internal Failure | High Charge current
  // msg.buf[3] = alarm[3];// Cell Imbalance
  // msg.buf[4] = warning[0];//High temp  Low Voltage | High Voltage
  // msg.buf[5] = warning[1];// High Discharge Current | Low Temperature
  // msg.buf[6] = warning[2];//Internal Failure | High Charge current
  // msg.buf[7] = warning[3];// Cell Imbalance
  // bmscan.write(msg, settings.carCanIndex);


  delay(2);
  msg.id  = 0x373;
  msg.len = 8;
  msg.buf[0] = lowByte(uint16_t(bms.getLowCellVolt() * 1000));
  msg.buf[1] = highByte(uint16_t(bms.getLowCellVolt() * 1000));
  msg.buf[2] = lowByte(uint16_t(bms.getHighCellVolt() * 1000));
  msg.buf[3] = highByte(uint16_t(bms.getHighCellVolt() * 1000));
  msg.buf[4] = lowByte(uint16_t(bms.getLowTemperature() + 273.15));
  msg.buf[5] = highByte(uint16_t(bms.getLowTemperature() + 273.15));
  msg.buf[6] = lowByte(uint16_t(bms.getHighTemperature() + 273.15));
  msg.buf[7] = highByte(uint16_t(bms.getHighTemperature() + 273.15));
  bmscan.write(msg, settings.carCanIndex);

  // delay(2);
  // msg.id  = 0x379; //Installed capacity
  // msg.len = 2;
  // msg.buf[0] = lowByte(uint16_t(settings.Pstrings * settings.CAP));
  // msg.buf[1] = highByte(uint16_t(settings.Pstrings * settings.CAP));

  // delay(2);
  // msg.id  = 0x372;
  // msg.len = 8;
  // msg.buf[0] = lowByte(bms.getNumModules());
  // msg.buf[1] = highByte(bms.getNumModules());
  // msg.buf[2] = 0x00;
  // msg.buf[3] = 0x00;
  // msg.buf[4] = 0x00;
  // msg.buf[5] = 0x00;
  // msg.buf[6] = 0x00;
  // msg.buf[7] = 0x00;
  // bmscan.write(msg, settings.carCanIndex);

}

//Execute every 20 seconds
void ms20000Callback() {
}

//Execute every half second
void ms500Callback() {
  sendcommand();
  statusCan();
}

void canread(int canInterfaceOffset, int idOffset)
{
  bmscan.read(inMsg, canInterfaceOffset);

  //TODO::this can probably be better
  if (inMsg.id < 0x300)//do VW BMS magic if ids are ones identified to be modules
  {
    inMsg.id = inMsg.id + idOffset;
    bms.decodecan(inMsg, 0); //do VW BMS if ids are ones identified to be modules
  }

  if ((inMsg.id & 0x1FFFFFFF) < 0x1A5554F0 && (inMsg.id & 0x1FFFFFFF) > 0x1A555400)   // Determine if ID is Temperature CAN-ID
  {
    inMsg.id = inMsg.id + idOffset/4; // the temps only require offsetting id by 8 (1/4 of 32) i.e. 1 can id per slave. 
    bms.decodetemp(inMsg, 0);
  }
  
}

void loop(){
  ts.execute();
  canread(0, 0);

  esp_task_wdt_reset();
}