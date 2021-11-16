#ifndef BMS_CAN_H
#define BMS_CAN_H
#include <Arduino.h>
#include <ACAN_ESP32.h>
#include <ACAN2515.h>

static const byte MCP2515_SCK = GPIO_NUM_18 ; // SCK input of MCP2515
static const byte MCP2515_SI  = GPIO_NUM_23 ; // SI input of MCP2515
static const byte MCP2515_SO  = GPIO_NUM_19 ; // SO output of MCP2515
static const byte MCP2515_CS  = GPIO_NUM_5 ; // CS input of MCP2515
static const byte MCP2515_INT = GPIO_NUM_2 ; // INT output of MCP2515

static const byte MCP2515_SCK_2 = GPIO_NUM_14 ; // SCK input of MCP2515
static const byte MCP2515_SI_2  = GPIO_NUM_13 ; // SI input of MCP2515
static const byte MCP2515_SO_2  = GPIO_NUM_12 ; // SO output of MCP2515
static const byte MCP2515_CS_2  = GPIO_NUM_15 ; // CS input of MCP2515
static const byte MCP2515_INT_2 = GPIO_NUM_33 ; // INT output of MCP2515

typedef struct BMS_CAN_MESSAGE {
    uint32_t id;
    uint16_t timestamp;
    struct {
        uint8_t extended:1; // identifier is extended (29-bit)
        uint8_t remote:1;   // remote transmission request packet type
        uint8_t overrun:1;  // message overrun
        uint8_t reserved:5;
    } flags;
    uint8_t len;          // length of data
    uint8_t buf[8];
} BMS_CAN_MESSAGE;

class BmsCan {
  public:
     int write(const BMS_CAN_MESSAGE &msg, int interfaceIndex);
     void begin(uint32_t baud, int interfaceIndex);
     int read (BMS_CAN_MESSAGE &msg, int interfaceIndex);
  private:
     CANMessage convert(const BMS_CAN_MESSAGE &msg);
     BMS_CAN_MESSAGE convert(const CANMessage &msg);

};

#endif