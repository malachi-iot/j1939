#include <Arduino.h>

#include <SPI.h>

#include <estd/istream.h>
#include <estd/ostream.h>

#include "transport.h"

using namespace estd;

bool can_online = false;

//extern uint8_t source_address;

extern arduino_ostream cout;
extern arduino_istream cin;

void twai_init();

void init_can(transport& t)
{
    // TODO: Consider using https://github.com/adafruit/Adafruit_CAN
    // because https://github.com/adafruit/arduino-CAN aka 
    // https://registry.platformio.org/libraries/adafruit/CAN%20Adafruit%20Fork seems to be obsolete

#ifdef ESP_PLATFORM
    cout << F("ESP32 native CAN mode: ");

    twai_init();

    // We'll abort if CAN is not online
    can_online = true;

#elif defined(AUTOWP_LIB)
    cout << F("AUTOWP/MCP2515 mode: ");
    
    //SPI.begin();

    MCP2515& mcp2515 = t.mcp2515;
    can_online = mcp2515.reset() == MCP2515::ERROR_OK;

    // Oh my god, I spent 3+ hours diagnosing problems here only to find out
    // we need MCP_8MHZ here.  Thanks to stumbling on this from 
    // https://how2electronics.com/interfacing-mcp2515-can-bus-module-with-arduino/
    //can_online &= mcp2515.setBitrate(CAN_125KBPS) == MCP2515::ERROR_OK;
    can_online &= mcp2515.setBitrate(CAN_125KBPS, MCP_8MHZ) == MCP2515::ERROR_OK;
    
    if(!can_online) cout << F("Issue setting bit rate") << endl;

    can_online &= mcp2515.setNormalMode() == MCP2515::ERROR_OK;

    //can_online &= mcp2515.setListenOnlyMode() == MCP2515::ERROR_OK;
    //mcp2515.setLoopbackMode();

    //source_address = 0x7;

    // comes back '0' which seems to map to 'no status'
    //cout << F ("status=") << mcp2515.getStatus();

#else
#if ADAFRUIT_FEATHER_M4_CAN
    cout << F("SAME5x mode: ");

    pinMode(PIN_CAN_STANDBY, OUTPUT);
    digitalWrite(PIN_CAN_STANDBY, false); // turn off STANDBY
    pinMode(4, OUTPUT); // BOOST_ENABLE
    digitalWrite(4, true); // turn on booster
#elif defined(MCP2515_DEFAULT_CS_PIN)
    cout << F("Adafruit/MCP2515 mode (pin=") << MCP2515_DEFAULT_CS_PIN << F("): ");
#else
    cout << F("Adafruit mode: ");
#endif

    // start the CAN bus at 125 kbps.  Would go down to
    // 25 kbps where TWAI demo likes to be, but it seems
    // MPC2515 won't do it.  Also I suspect MPC2515 doesn't
    // want to go below 100kbit, but that's just speculation
    can_online = CAN.begin(125E3);
#endif

    cout << (can_online ? F("online") : F("offline")) << endl;
}