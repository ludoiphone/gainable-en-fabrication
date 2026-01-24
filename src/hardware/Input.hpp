#pragma once

#include <wiringPi.h>

class Input {
    
public:
    explicit Input(int pin);
    bool isActive() const;
    int getPin() const;
    
private:
    int m_pin;
};
