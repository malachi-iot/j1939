#include <Arduino.h>

#include <estd/istream.h>
#include <estd/ostream.h>

#include "transport.h"

using namespace estd;

bool can_online = false;

extern arduino_ostream cout;
extern arduino_istream cin;

void init_can(transport& t)
{
    // TODO: Consider using https://github.com/adafruit/Adafruit_CAN
    // because https://github.com/adafruit/arduino-CAN aka 
    // https://registry.platformio.org/libraries/adafruit/CAN%20Adafruit%20Fork seems to be obsolete

#ifdef AUTOWP_LIB
    cout << F("MCP2515 mode: ");
    MCP2515& mcp2515 = t.mcp2515;
    mcp2515.reset();
    can_online = mcp2515.setBitrate(CAN_125KBPS) == MCP2515::ERROR_OK;
    //mcp2515.setNormalMode();
    can_online &= mcp2515.setListenOnlyMode() == MCP2515::ERROR_OK;
#else
    cout << F("SAME5x mode: ");

    // start the CAN bus at 125 kbps.  Would go down to
    // 25 kbps where TWAI demo likes to be, but it seems
    // MPC2515 won't do it.  Also I suspect MPC2515 doesn't
    // want to go below 100kbit, but that's just speculation
    can_online = CAN.begin(125E3);
#endif

    cout << (can_online ? F("online") : F("offline")) << endl;
}