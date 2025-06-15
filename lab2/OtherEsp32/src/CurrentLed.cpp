#include "CurrentLed.h"

CurrentLed& CurrentLed::getInstance() {
    static CurrentLed instance;  // Created on first use, then reused
    return instance;
}

CurrentLed::CurrentLed() : ledNumber(0) {
    // Additional initialization if necessary
}
