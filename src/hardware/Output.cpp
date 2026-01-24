#include "Output.hpp"

Output::Output(int pin) : m_pin(pin), m_state(false) {
    pinMode(m_pin, OUTPUT);
    off();
}

void Output::on() {
    digitalWrite(m_pin, LOW); // active low
    m_state = true;
}

void Output::off() {
    digitalWrite(m_pin, HIGH);
    m_state = false;
}

bool Output::isOn() const { return m_state; }

int Output::getPin() const { return m_pin; }
