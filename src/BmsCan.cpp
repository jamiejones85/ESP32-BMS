#include "BmsCan.h"

ACAN2515* can1;
ACAN2515* can2;
bool started[] = {false, false, false};
SPIClass SPI2(HSPI);

CANMessage BmsCan::convert(const BMS_CAN_MESSAGE &msg) {
  CANMessage ret;

  ret.id = msg.id;
  ret.len = msg.len;
  ret.ext = msg.flags.extended;

  for(int i = 0; i < msg.len; i++) {
    ret.data[i] = msg.buf[i];
  }
  return ret;
}

BMS_CAN_MESSAGE BmsCan::convert(const CANMessage &msg) {
  BMS_CAN_MESSAGE ret;

  ret.id = msg.id;
  ret.len = msg.len;
  ret.flags.extended = msg.ext;
  for(int i = 0; i < msg.len; i++) {
    ret.buf[i] = msg.data[i];
  }
  return ret;
}

int BmsCan::read (BMS_CAN_MESSAGE &msg, int interfaceIndex) {
  CANMessage readMesg;
  int response;
  if (interfaceIndex == 0) {
    response = ACAN_ESP32::can.receive(readMesg);
  } else if (interfaceIndex == 1) {
    response = can1->receive(readMesg);
  } else if (interfaceIndex == 2) {
    response = can2->receive(readMesg);
  }
  msg = convert(readMesg);
  return response;
}

void BmsCan::begin(uint32_t baud, int interfaceIndex) {

  if (interfaceIndex == 0 && !started[interfaceIndex]) {
    ACAN_ESP32_Settings settings(baud);
    settings.mRxPin = GPIO_NUM_16;
    settings.mTxPin = GPIO_NUM_17;

    ACAN_ESP32::can.begin(settings);
    started[interfaceIndex] = true;
  } else if (interfaceIndex == 1 && !started[interfaceIndex]) {
    can1 = new ACAN2515 (MCP2515_CS, SPI, MCP2515_INT) ;
    ACAN2515Settings settings(16 * 1000 * 1000, baud);
    can1->begin(settings, [] { can1->isr () ; });
    started[interfaceIndex] = true;

  } else if (interfaceIndex == 2 && !started[interfaceIndex]) {
    SPI2.begin();
    can2 = new ACAN2515 (MCP2515_CS_2, SPI2, MCP2515_INT_2) ;
    ACAN2515Settings settings(16 * 1000 * 1000, baud);
    can2->begin(settings, [] { can2->isr () ; });
    started[interfaceIndex] = true;
  } 

}

int BmsCan::write(const BMS_CAN_MESSAGE &msg, int interfaceIndex) {
  CANMessage toSend = convert(msg);

  if (interfaceIndex == 0) {
    // ACAN_ESP32::can.tryToSend(toSend);
  } else if (interfaceIndex == 1) {
    can1->tryToSend(toSend);
  } else if (interfaceIndex == 2 && can2 != NULL) {
    can2->tryToSend(toSend);
  }
  return 0;
}