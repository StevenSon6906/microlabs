#pragma once

class AsyncStop 
{
private:
    unsigned long stopUntil = 0;
    unsigned long duration = 15000;

public:
    void request();
    bool isActive();
    void setDuration(unsigned long ms);
    static AsyncStop& getInstance();
};
