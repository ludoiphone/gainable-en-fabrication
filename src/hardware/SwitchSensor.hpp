#pragma once

#include <wiringPi.h>

class SwitchSensor {
    
public:
    explicit SwitchSensor(int pin);
    bool isActive() const;
    
private:
    int m_pin;
};
