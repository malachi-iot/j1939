#include "ca.h"

void ArduinoLightingCommandSink::digital_write(unsigned pin, status s)
{
    switch(s)
    {
        case status::enable:
            digitalWrite(pin, HIGH);
            break;

        case status::disable:
            digitalWrite(pin, LOW);
            break;

        default: break;
    }
}
