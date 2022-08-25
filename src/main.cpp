// Import required libraries
#include "WiFi.h"
#include "ESPAsyncWebServer.h"
#include "SPIFFS.h"
#include "Bms.h"
#include "BMSWebServer.h"
#include "Config.h"
#include <TaskScheduler.h>
#include <SPI.h>
#include <esp_task_wdt.h>
#include <EEPROM.h>
#include <Ticker.h>
#include <Update.h>
#include <ArduinoOTA.h>
#include <Arduino.h>
#define WDT_TIMEOUT 3
#define EEPROM_SIZE 512
#define HOSTNAME "ESP32-BMS"
int moduleidstart = 0x1CC;

Bms bms;
BMSWebServer bmsWebServer;
EEPROMSettings settings;
Config config;

// Create AsyncWebServer object on port 80
Scheduler ts;

void staCheck();
void ms1000Callback();
void ms500Callback();
Task ms500Task(500, TASK_FOREVER, &ms500Callback);
Task ms1000Task(1000, TASK_FOREVER, &ms1000Callback);
// Ticker sta_tick(staCheck, 5000, 0, MICROS);

// void staCheck(){
//   sta_tick.stop();

//   if(!(uint32_t)WiFi.localIP()){
//     WiFi.mode(WIFI_AP); //disable station mode
//   }
// }

void setup(){
  pinMode(MCP2515_CS, OUTPUT);
  digitalWrite(MCP2515_CS, true);
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
  WiFi.hostname(HOSTNAME);
  // Connect to Wi-Fi
  WiFi.begin();
  // sta_tick.start();

  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
      //make sure the dog is kicked
      esp_task_wdt_reset();
  });

  ArduinoOTA.setHostname(HOSTNAME);
  ArduinoOTA.begin();

  settings = config.load();

  bmsWebServer.setup(settings, config, bms);
  bms.setup(settings);

  ts.addTask(ms500Task);
  ms500Task.enable();

  ts.addTask(ms1000Task);
  ms1000Task.enable();

  esp_task_wdt_init(WDT_TIMEOUT, true); //enable panic so ESP32 restarts
  esp_task_wdt_add(NULL); //add current thread to WDT watch
  Serial.println("Setup Done");
}

//Execute every 1 seconds
void ms1000Callback() {
  bms.ms1000Task();
}

//Execute every half second
void ms500Callback() {
  bms.ms500Task(settings);
}

void loop(){
  ArduinoOTA.handle();
  // sta_tick.update();
  ts.execute();
  bmsWebServer.execute();
  bms.execute();
  esp_task_wdt_reset();
}