#include <chrono>
#include "NativeMillis.h"

using std::chrono::duration_cast;
using std::chrono::milliseconds;
using std::chrono::system_clock;

unsigned long NativeMillis::millis() {
    return duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
}