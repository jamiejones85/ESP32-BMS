#include "unity.h"
#include "BmsCanMessage.h"
#include "BMSModule.h"
#include "NativeMillis.h"

void setUp(void)
{
  // set stuff up here
}

void tearDown(void)
{
  // clean stuff up here
}

void test_test() {
    BMSModule module;
    NativeMillis mils;
    module.setMillis(&mils);

    TEST_ASSERT_EQUAL(true, true);

}

int main( int argc, char **argv) {

  UNITY_BEGIN(); // IMPORTANT LINE!

  RUN_TEST(test_test);

  UNITY_END(); // stop unit testing

}
