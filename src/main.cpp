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

#define WDT_TIMEOUT 3
#define EEPROM_SIZE 512

int moduleidstart = 0x1CC;
int cellspresent = 0;

Bms bms;
BMSWebServer bmsWebServer;

EEPROMSettings settings;
Config config;

// Create AsyncWebServer object on port 80
Scheduler ts;

void staCheck();
void ms20000Callback();
void ms500Callback();
Task ms500Task(500, TASK_FOREVER, &ms500Callback);
Task ms20000Task(20000, TASK_FOREVER, &ms20000Callback);
Ticker sta_tick(staCheck, 5000, 0, MICROS);

void staCheck(){
  sta_tick.stop();

  if(!(uint32_t)WiFi.localIP()){
    WiFi.mode(WIFI_AP); //disable station mode
  }
}

void setup(){
  EEPROM.begin(EEPROM_SIZE);
  // Serial port for debugging purposes
  Serial.begin(115200);

  // Initialize SPIFFS
  if(!SPIFFS.begin(true)){
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }

  Serial.println(SPIFFS.exists("/firmware.bin"));

  File file = SPIFFS.open("/firmware.bin");
  
  if(SPIFFS.exists("/firmware.bin") && file){
      Serial.println("Updating....");
      size_t fileSize = file.size();
 
      if(!Update.begin(fileSize)){
        Serial.println("Cannot do the update");
      } else {
        Update.writeStream(file);

        if(Update.end()){
          Serial.println("Successful update");  
        }else {
          Serial.println("Error Occurred: " + String(Update.getError()));
        }
      }

     
      file.close();
      Serial.println("Deleting firmware.bin");
      SPIFFS.remove("/firmware.bin");
      Serial.println("Reset in 4 seconds...");
      delay(4000);
      ESP.restart();
  }


  //AP and Station Mode
  WiFi.mode(WIFI_AP_STA);
  WiFi.hostname("ESP32-BMS");
  // Connect to Wi-Fi
  WiFi.begin();
  sta_tick.start();

  settings = config.load();

  bmsWebServer.setup(settings, config, bms);
  bms.setup(settings);

  ts.addTask(ms500Task);
  ms500Task.enable();

  ts.addTask(ms20000Task);
  ms20000Task.enable();

  esp_task_wdt_init(WDT_TIMEOUT, true); //enable panic so ESP32 restarts
  esp_task_wdt_add(NULL); //add current thread to WDT watch
  Serial.println("Setup Done");
}

//Execute every 20 seconds
void ms20000Callback() {

}

//Execute every half second
void ms500Callback() {
  bms.ms500Task(settings);
}

void loop(){
  sta_tick.update();
  ts.execute();
  bms.execute();
  esp_task_wdt_reset();
}