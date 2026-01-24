#include "Input.hpp"

Input::Input(int pin) : m_pin(pin) {
    pinMode(m_pin, INPUT);
    pullUpDnControl(m_pin, PUD_UP);
}

bool Input::isActive() const {
    return digitalRead(m_pin) == LOW;
}

int Input::getPin() const {
    return m_pin;
}
