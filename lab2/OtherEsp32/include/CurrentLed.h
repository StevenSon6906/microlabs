    #ifndef CURRENTLED_H
#define CURRENTLED_H

#include <cstdint>

class CurrentLed {
public:
    uint8_t ledNumber;
    static CurrentLed& getInstance();

private:
    CurrentLed();
    CurrentLed(const CurrentLed&) = delete;
    CurrentLed& operator=(const CurrentLed&) = delete;
};

#endif 
