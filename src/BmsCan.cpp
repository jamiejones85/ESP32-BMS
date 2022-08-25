#include "BmsCan.h"

bool started[] = {false, false, false};

CAN_FRAME BmsCan::convert(const BMS_CAN_MESSAGE &msg) {
  CAN_FRAME ret;

  ret.id = msg.id;
  ret.length = msg.len;
  ret.extended = msg.flags.extended;

  for(int i = 0; i < msg.len; i++) {
    ret.data.uint8[i] = msg.buf[i];
  }
  return ret;
}

BMS_CAN_MESSAGE BmsCan::convert(const CAN_FRAME &msg) {
  BMS_CAN_MESSAGE ret;

  ret.id = msg.id;
  ret.len = msg.length;
  ret.flags.extended = msg.extended;
  for(int i = 0; i < msg.length; i++) {
    ret.buf[i] = msg.data.uint8[i];
  }
  return ret;
}

int BmsCan::read (BMS_CAN_MESSAGE &msg, int interfaceIndex) {
  CAN_FRAME readMesg;
  int response;
  if (interfaceIndex == 0) {
    response = Can0.read(readMesg);
  } else if (interfaceIndex == 1) {
    response = Can1.read(readMesg);
  }
  msg = convert(readMesg);
  return response;
}

void BmsCan::begin(uint32_t baud, int interfaceIndex) {

  if (interfaceIndex == 0 && !started[interfaceIndex]) {
    Can0.begin(baud);
    Serial.println("Can0 Start") ;
    started[interfaceIndex] = true;
  } else if (interfaceIndex == 1 && !started[interfaceIndex]) {
    CAN1.begin(baud);
    Serial.println("Can1 Start") ;
    started[interfaceIndex] = true;
    }
}

int BmsCan::write(const BMS_CAN_MESSAGE &msg, int interfaceIndex) {
  CAN_FRAME toSend = convert(msg);

  if (interfaceIndex == 0) {
    Can0.sendFrame(toSend);
  } else if (interfaceIndex == 1) {
    Can1.sendFrame(toSend);
  }
  return 0;
}