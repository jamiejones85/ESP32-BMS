// Import required libraries
#include "WiFi.h"
#include "ESPAsyncWebServer.h"
#include "SPIFFS.h"
#include "BmsCan.h"
#include "BMSModuleManager.h"
#include "AsyncJson.h"
#include "ArduinoJson.h"

#include <TaskScheduler.h>
// Replace with your network credentials
const char* ssid = "*";
const char* password = "*";

int controlid = 0x0BA;
int moduleidstart = 0x1CC;
int cellspresent = 0;

BmsCan bmscan;
BMS_CAN_MESSAGE msg;
BMS_CAN_MESSAGE inMsg;
BMSModuleManager bms;

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);
Scheduler ts;

void ms20000Callback();
void ms500Callback();
Task ms500Task(500, TASK_FOREVER, &ms500Callback);
Task ms20000Task(20000, TASK_FOREVER, &ms20000Callback);

void setup(){
  // Serial port for debugging purposes
  Serial.begin(115200);

  // Initialize SPIFFS
  if(!SPIFFS.begin(true)){
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }

  // Print ESP32 Local IP Address
  Serial.println(WiFi.localIP());
  server.on("/voltages", HTTP_GET, [] (AsyncWebServerRequest *request) {
    AsyncResponseStream *response = request->beginResponseStream("application/json");
    DynamicJsonDocument json(10240);

    bms.printPackDetailsJson(json);
    serializeJson(json, *response);
    request->send(response);
  });

  server.serveStatic("/", SPIFFS, "/").setDefaultFile("index.html");
  

  // Start server
  server.begin();
  bmscan.begin(500000, 0);

  ts.addTask(ms500Task);
  ms500Task.enable();

  ts.addTask(ms20000Task);
  ms20000Task.enable();
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

//Execute every 20 seconds
void ms20000Callback() {
}

//Execute every half second
void ms500Callback() {
  sendcommand();
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
}