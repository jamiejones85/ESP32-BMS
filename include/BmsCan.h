#ifndef BMS_CAN_H
#define BMS_CAN_H
#include <Arduino.h>
#include "BmsCanMessage.h"
#include "IBmsCan.h"
#include <ACAN_ESP32.h>
#include <ACAN2515.h>

//#define ALT
#ifdef ALT 
static const byte MCP2515_SCK_2 = GPIO_NUM_18 ; // SCK input of MCP2515
static const byte MCP2515_SI_2  = GPIO_NUM_23 ; // SI input of MCP2515
static const byte MCP2515_SO_2  = GPIO_NUM_19 ; // SO output of MCP2515
static const byte MCP2515_CS_2  = GPIO_NUM_5 ; // CS input of MCP2515
static const byte MCP2515_INT_2 = GPIO_NUM_27 ; // INT output of MCP2515

static const byte MCP2515_SCK = GPIO_NUM_25 ; // SCK input of MCP2515
static const byte MCP2515_SI  = GPIO_NUM_32 ; // SI input of MCP2515
static const byte MCP2515_SO  = GPIO_NUM_33 ; // SO output of MCP2515
static const byte MCP2515_CS  = GPIO_NUM_26 ; // CS input of MCP2515
static const byte MCP2515_INT = GPIO_NUM_13 ; // INT output of MCP2515
#else
static const byte MCP2515_SCK = GPIO_NUM_18 ; // SCK input of MCP2515
static const byte MCP2515_SI  = GPIO_NUM_23 ; // SI input of MCP2515
static const byte MCP2515_SO  = GPIO_NUM_19 ; // SO output of MCP2515
static const byte MCP2515_CS  = GPIO_NUM_5 ; // CS input of MCP2515
static const byte MCP2515_INT = GPIO_NUM_27 ; // INT output of MCP2515

static const byte MCP2515_SCK_2 = GPIO_NUM_25 ; // SCK input of MCP2515
static const byte MCP2515_SI_2  = GPIO_NUM_32 ; // SI input of MCP2515
static const byte MCP2515_SO_2  = GPIO_NUM_33 ; // SO output of MCP2515
static const byte MCP2515_CS_2  = GPIO_NUM_26 ; // CS input of MCP2515
static const byte MCP2515_INT_2 = GPIO_NUM_13 ; // INT output of MCP2515
#endif


class BmsCan: public IBmsCan {
  public:
     int write(const BMS_CAN_MESSAGE &msg, int interfaceIndex);
     void begin(uint32_t baud, int interfaceIndex);
     int read (BMS_CAN_MESSAGE &msg, int interfaceIndex);
  private:
     CANMessage convert(const BMS_CAN_MESSAGE &msg);
     BMS_CAN_MESSAGE convert(const CANMessage &msg);

};

#endif