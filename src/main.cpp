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

#define WDT_TIMEOUT 3
#define EEPROM_SIZE 512


// Replace with your network credentials
const char* ssid = "--";
const char* password = "--";

int moduleidstart = 0x1CC;
int cellspresent = 0;

byte status = 0x0;

Bms bms;
BMSModuleManager bmsModuleManager;
BMSWebServer bmsWebServer;

EEPROMSettings settings;
Config config;

// Create AsyncWebServer object on port 80
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
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }

  settings = config.load();

  // Print ESP32 Local IP Address
  Serial.println(WiFi.localIP());

  bmsWebServer.setup(settings, config, bmsModuleManager);
  bms.setup(settings);

  ts.addTask(ms500Task);
  ms500Task.enable();

  ts.addTask(ms20000Task);
  ms20000Task.enable();

  esp_task_wdt_init(WDT_TIMEOUT, true); //enable panic so ESP32 restarts
  esp_task_wdt_add(NULL); //add current thread to WDT watch

}

//Execute every 20 seconds
void ms20000Callback() {

}

//Execute every half second
void ms500Callback() {
  bms.ms500Task(settings);
}

void loop(){
  ts.execute();
  bms.execute();
  esp_task_wdt_reset();
}