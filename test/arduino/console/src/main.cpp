#include <Arduino.h>

#include <estd/istream.h>
#include <estd/ostream.h>
#include <estd/string.h>

#include <j1939/pdu.h>
#include <j1939/pgn.h>

#include <j1939/data_field/cm1.hpp>

// DEBT: Eventually we'd like to auto include this
#include <j1939/units/ostream.h>

#include "menu.h"
#include "transport.h"

uint32_t start_ms;

using namespace estd;
using namespace embr::j1939;
using namespace embr::units::literals;

arduino_ostream cout(Serial);
arduino_istream cin(Serial);

#ifdef AUTOWP_LIB
using transport = embr::can::autowp_transport;
static transport t(10);     // CS pin
#else
using transport = embr::can::adafruit_transport;
static transport t;
#endif

// DEBT: Can't alias directly frame_traits due to ambiguity
// with one in embr::j1939
using ft = embr::can::frame_traits<transport::frame>;
static uint8_t source_address = 0x77;

enum class States
{
    Entry,
    Running
};

States state = States::Entry;
estd::layer1::string<128> input;


namespace menu {


bool Navigator::up()
{
    if(current_->parent_ == nullptr) return false;

    current_ = current_->parent_;
    return true;
}

}



class SyntheticMenuAction : public menu::Action
{
    const int delineator_;

public:
    constexpr SyntheticMenuAction(int delineator) :
        delineator_{delineator}
    {}

    void render(ostream& out) const override
    {
        out << "synthetic#" << delineator_;
    }

    void action(ostream& out) override
    {
        out << "activated ";
        render(out);
    }
};


// EXPERIMENTAL
class UpMenuAction : public menu::Action
{
    menu::Navigator* navigator_;

public:
    constexpr UpMenuAction(menu::Navigator* navigator) :
        navigator_{navigator}
    {}

    void render(ostream& out) const override
    {
        out << "..";
    }

    void action(ostream& out) override
    {
        navigator_->up();
        navigator_->current()->render(out);
    }
};

template <pgns>
struct CanPGNActionImpl
{
    template <class TStreambuf>
    static void action(detail::basic_ostream<TStreambuf>&) {}

    template <class TPdu>
    static void prep(TPdu&) {}
};


#if __cpp_concepts
#endif

template <>
struct CanPGNActionImpl<pgns::cab_message1>
{
    using celcius = embr::units::centigrade<double>;
    
    celcius c;

    CanPGNActionImpl() : c(0) {}

    template <class TStreambuf>
    void action(detail::basic_ostream<TStreambuf>& out)
    {
        c += celcius(0.10);

        out << "Increasing temp to ";
        embr::units::write_abbrev(out, c);
    }

    void prep(pdu<pgns::cab_message1>& p)
    {
        p.destination_address(0x7);

        p.cab_interior_temperature_command(c);
        p.requested_percent_fan_speed(50_pct);
    }
};


template <pgns pgn>
class CanPGNAction : public menu::Action
{
    typedef CanPGNActionImpl<pgn> impl_type;

    impl_type impl;

    using traits = pgn::traits<pgn>;

    void render(ostream& out) const override
    {
        out << F("CAN (") << traits::abbrev();
        out << F("): ") << traits::name();
    }

    void action(ostream& out) override
    {
        out << F("Emit CAN ") << traits::abbrev();
        out << F(" - ");

        impl.action(out);

        pdu<pgn> p;

        p.source_address(source_address);

        impl.prep(p);

        auto frame = ft::create(p.can_id(), p.data(), p.size());

        t.send(frame);

        out << endl;
    }
};


// DEBT: Use proper iostream here
struct ios
{
    arduino_istream& in;
    arduino_ostream& out;

    using int_type = arduino_istream::int_type;
};



void menu1(menu::Navigator* nav, ios io)
{
    using traits = arduino_istream::traits_type;

    menu::Menu* menu = nav->current();

    if(state == States::Entry)
    {
        cout << F("Top level: ") << endl << endl;
        state = States::Running;

        menu->render(cout);

        return;
    }

    traits::int_type c = io.in.get();

    if(c == traits::eof()) return;

    io.out.put(c);

    switch(c)
    {
        case 10: break;

        case 13:
        //case 10:
        {
            int selected = -1;

            //cout << endl << F("Acting on: ") << input << endl;

            // DEBT: Somewhere we have a string-optimized version of this...
            from_chars(input.data(), input.data() + input.size(), selected);
            if(selected == 0)
            {
                if(!nav->up())
                    cout << F("Topmost menu, cannot go up further");
                else
                    nav->current()->render(io.out);
            }
            else if(selected != -1)
            {
                menu->activate(selected - 1, io.out);

                /* FIX: something's wrong with copy constructor
                expected<void, errc> r = menu->activate(selected - 1);

                if(r.has_error())
                {

                }   */
            }

            input.clear();
            break;
        }

        default:
            // DEBT: Should we have to cast this?
            input += (char)c;
            break;
    }
}

menu::Menu topLevel, submenu("submenu#1", &topLevel);
SyntheticMenuAction item1{0}, item2{1};
menu::Navigator nav(&topLevel);
CanPGNAction<pgns::oel> item4;
CanPGNAction<pgns::cab_message1> item5;
menu::MenuAction subitem1(&nav, &submenu);

bool can_online = false;

void setup() 
{
    Serial.begin(115200);

    topLevel.items.push_back(&item1);
    topLevel.items.push_back(&item2);
    topLevel.items.push_back(&subitem1);
    topLevel.items.push_back(&item4);
    topLevel.items.push_back(&item5);

    while(!Serial);

    // TODO: Consider using https://github.com/adafruit/Adafruit_CAN
    // because https://github.com/adafruit/arduino-CAN aka 
    // https://registry.platformio.org/libraries/adafruit/CAN%20Adafruit%20Fork seems to be obsolete

#ifdef AUTOWP_LIB
    cout << "MCP2515 mode" << endl;
    MCP2515& mcp2515 = t.mcp2515;
    mcp2515.reset();
    mcp2515.setBitrate(CAN_125KBPS);
    //mcp2515.setNormalMode();
    mcp2515.setListenOnlyMode();
#else
    cout << "SAME5x mode" << endl;

    // start the CAN bus at 125 kbps.  Would go down to
    // 25 kbps where TWAI demo likes to be, but it seems
    // MPC2515 won't do it.  Also I suspect MPC2515 doesn't
    // want to go below 100kbit, but that's just speculation
    can_online = CAN.begin(125E3);
    if (!can_online)    cout << F("Starting CAN failed!") << endl;
#endif
}

void loop() 
{
#ifdef AUTOWP_LIB
    struct can_frame canMsg;
    MCP2515::ERROR e = t.mcp2515.readMessage(&canMsg);

    if (e == MCP2515::ERROR_OK)
    {
        cout << hex << canMsg.can_id << ' ' << canMsg.can_dlc << ' ';

        for (int i = 0; i<canMsg.can_dlc; i++)
        {  // print the data
            Serial.print(canMsg.data[i],HEX);
            Serial.print(" ");
        }

        cout << endl;
    }
#else
    // try to parse packet
    transport::frame frame;

    if (can_online && t.receive(&frame))
    {
        cout << F("Received packet with id 0x") << hex << frame.id;
        cout << endl;
    }
#endif

/*
    uint32_t now_ms = millis();

    long count = now_ms - start_ms;

    cout << F("Please input something: ");

    estd::layer1::string<128> buffer;

    cin >> buffer;
    cout << F("You input: ") << buffer << estd::endl; */
    menu1(&nav, ios{cin, cout});
}
