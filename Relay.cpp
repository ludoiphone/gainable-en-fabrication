#include "Relay.hpp"

Relay::Relay(int pin) : m_pin(pin), m_state(false) {
    pinMode(m_pin, OUTPUT);
    off();
}

void Relay::on() {
    digitalWrite(m_pin, LOW); // active low
    m_state = true;
}

void Relay::off() {
    digitalWrite(m_pin, HIGH);
    m_state = false;
}

bool Relay::isOn() const { return m_state; }

int Relay::getPin() const { return m_pin; }
