#ifndef BMS_CAN_MESSAGE_H
#define BMS_CAN_MESSAGE_H
#include <stdint.h>

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
#endif