#pragma once

#include <wiringPi.h>

class Output {
    
public:
    Output(int pin);
    void on();
    void off();
    bool isOn() const;
    int getPin() const;
    
private:
    int m_pin;
    bool m_state;
};
