#include "SwitchSensor.hpp"

SwitchSensor::SwitchSensor(int pin) : m_pin(pin) {
    pinMode(m_pin, INPUT);
    pullUpDnControl(m_pin, PUD_UP);
}

bool SwitchSensor::isActive() const {
    return digitalRead(m_pin) == LOW;
}
