#include "unity.h"
#include "BmsCanMessage.h"
#include "IBmsCan.h"
#include "Shunt.h"

void setUp(void)
{
  // set stuff up here
}

void tearDown(void)
{
  // clean stuff up here
}

void test_positive_current_message() {
    BMS_CAN_MESSAGE m;
    m.id = 0x521; //Current id
    m.len = 6;
    m.buf[0] = 0x00;
    m.buf[1] = 0x01;
    m.buf[2] = 0x81;
    m.buf[3] = 0x6F;
    m.buf[4] = 0x00;
    m.buf[5] = 0x00;

    Shunt shunt;
    shunt.process(m);

    ShuntData d = shunt.getData();
    //mA
    TEST_ASSERT_EQUAL(28545, d.milliamps);
}

void test_negative_current_message() {
    BMS_CAN_MESSAGE m;
    m.id = 0x521; //Current id
    m.len = 6;
    m.buf[0] = 0x00;
    m.buf[1] = 0x05;
    m.buf[2] = 0x45;
    m.buf[3] = 0xE2;
    m.buf[4] = 0xFF;
    m.buf[5] = 0xFF;

    Shunt shunt;
    shunt.process(m);

    ShuntData d = shunt.getData();
    //mA
    TEST_ASSERT_EQUAL(-7611, d.milliamps);
}


void test_voltage1_message() {

    BMS_CAN_MESSAGE m;
    m.id = 0x522; //Voltage 1 id
    m.len = 6;
    m.buf[0] = 0x01;
    m.buf[1] = 0x0D;
    m.buf[2] = 0x8C;
    m.buf[3] = 0xA5;
    m.buf[4] = 0x05;
    m.buf[5] = 0x00;

    Shunt shunt;
    shunt.process(m);

    ShuntData d = shunt.getData();
    //mV
    TEST_ASSERT_EQUAL(370060, d.voltage1);
}

void test_watts_message() {
    BMS_CAN_MESSAGE m;
    m.id = 0x526; //watts id
    m.len = 6;
    m.buf[0] = 0x05;
    m.buf[1] = 0x07;
    m.buf[2] = 0x4A;
    m.buf[3] = 0x18;
    m.buf[4] = 0x00;
    m.buf[5] = 0x00;

    Shunt shunt;
    shunt.process(m);

    ShuntData d = shunt.getData();
    //mA
    TEST_ASSERT_EQUAL(6218, d.watts);
}

void test_amphours_message() {
    BMS_CAN_MESSAGE m;
    m.id = 0x527; //ampseconds id
    m.len = 6;
    m.buf[0] = 0x06;
    m.buf[1] = 0x02;
    m.buf[2] = 0xC2;
    m.buf[3] = 0x2B;
    m.buf[4] = 0x00;
    m.buf[5] = 0x00;

    Shunt shunt;
    shunt.process(m);

    ShuntData d = shunt.getData();
    //mA
    TEST_ASSERT_EQUAL_FLOAT(3.111667, d.amphours);
}

void test_kilowatthours_message() {
    BMS_CAN_MESSAGE m;
    m.id = 0x528; //ampseconds id
    m.len = 6;
    m.buf[0] = 0x07;
    m.buf[1] = 0x0B;
    m.buf[2] = 0xDC;
    m.buf[3] = 0x03;
    m.buf[4] = 0x00;
    m.buf[5] = 0x00;

    Shunt shunt;
    shunt.process(m);

    ShuntData d = shunt.getData();
    //mA
    TEST_ASSERT_EQUAL_FLOAT(0.988, d.kilowatthours);
}

void test_stateofcharge() {
    BMS_CAN_MESSAGE m;
    m.id = 0x527; //ampseconds id
    m.len = 6;
    m.buf[0] = 0x06;
    m.buf[1] = 0x02;
    m.buf[2] = 0xC2;
    m.buf[3] = 0x2B;
    m.buf[4] = 0x00;
    m.buf[5] = 0x00;

    EEPROMSettings settings;
    settings.useableAh = 9;

    Shunt shunt;
    shunt.setSettings(&settings);
    shunt.process(m);

    //mA
    TEST_ASSERT_EQUAL_FLOAT(65, shunt.getStateOfCharge());
}

int main( int argc, char **argv) {

  UNITY_BEGIN(); // IMPORTANT LINE!

  RUN_TEST(test_voltage1_message);
  RUN_TEST(test_negative_current_message);
  RUN_TEST(test_positive_current_message);
  RUN_TEST(test_watts_message);
  RUN_TEST(test_amphours_message);
  RUN_TEST(test_kilowatthours_message);
  RUN_TEST(test_stateofcharge);

  UNITY_END(); // stop unit testing

}
