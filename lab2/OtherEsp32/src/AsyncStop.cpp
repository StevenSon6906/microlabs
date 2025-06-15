#include "asyncstop.h"
#include <Arduino.h>

void AsyncStop::request() {
    stopUntil = millis() + duration;
}

bool AsyncStop::isActive() {
    return millis() < stopUntil;
}

void AsyncStop::setDuration(unsigned long ms) {
    duration = ms;
}
AsyncStop& AsyncStop::getInstance() {
    static AsyncStop instance;  
    return instance;
}