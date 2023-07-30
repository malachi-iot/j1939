#include <Arduino.h>

#undef _abs     // ESP32 specifically has this additional annoying macro set

#include <estd/chrono.h>
#include <estd/istream.h>
#include <estd/ostream.h>
#include <estd/string.h>
#include <estd/thread.h>

#include <j1939/pdu.h>
#include <j1939/pgn.h>
#include <j1939/ca.hpp>

#include <j1939/cas/diagnostic.hpp>
#include <j1939/cas/network.hpp>
#include <j1939/cas/internal/prng_address_manager.h>

#include <j1939/data_field/all.hpp>

#include <j1939/ostream.h>

#include "main.h"
#include "menu.hpp"
#include "scheduler.h"
#include "transport.h"

uint32_t start_ms;

using namespace estd;
using namespace estd::chrono_literals;
using namespace embr::j1939;
using namespace embr::units::literals;

arduino_ostream cout(Serial);
arduino_istream cin(Serial);

#ifdef AUTOWP_LIB
static transport t(10);     // CS pin
#else
static transport t;
#endif

embr::j1939::impl::network_ca_base::states nca_last_state {};
// DEBT: Can't alias directly frame_traits due to ambiguity
// with one in embr::j1939
using ft = embr::can::frame_traits<transport::frame>;
#ifdef __AVR__
uint8_t source_address_ = 0x7;
constexpr embr::word<3> ecu_instance(0);
#else
#if ADAFRUIT_FEATHER_M4_CAN
uint8_t source_address_ = 0x77;
constexpr embr::word<3> ecu_instance(1);
#else
// Assuming ESP32
uint8_t source_address_ = 0x70;
constexpr embr::word<3> ecu_instance(2);
#endif
#endif

#define FEATURE_AGGREGATED_CA 0

scheduler_type scheduler;
using dca_type = diagnostic_ca<transport, arduino_ostream>;

using proto_name = embr::j1939::layer0::NAME<true,
    industry_groups::process_control,
    vehicle_systems::ig5_not_available, // DEBT: Change to a better IG/Veh Sys,
    function_fields::ig5_not_available>;

using nca_type = embr::j1939::impl::network_ca<transport,
    scheduler_type,
    embr::j1939::internal::prng_address_manager>;

#if FEATURE_AGGREGATED_CA
embr::j1939::impl::controller_application_aggregator<dca_type, nca_type>
    app_ca(
        cout,
        nca_type::get_init(
            proto_name::sparse{0, 0, ecu_instance.value()},
            scheduler));
nca_type& nca = estd::get<0>(app_ca.child_cas);
#else
dca_type dca(cout);
nca_type nca(proto_name::sparse{0, 0, ecu_instance.value()}, scheduler);
#endif

enum class States
{
    Entry,
    Running
};

States state = States::Entry;
estd::layer1::string<128> input;

uint8_t source_address()
{
    if(nca.state == impl::network_ca_base::states::claimed)
        return nca.address().value();
    else
        return source_address_;
}


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

        out << F("Increasing temp to ") << embr::put_unit(c);
    }

    void prep(pdu<pgns::cab_message1>& p)
    {
        // DEBT: Old flip/flop dest trick won't work here now
        p.destination_address(source_address() == 7 ? 0x77 : 0x7);

        p.cab_interior_temperature_command(c);
        p.requested_percent_fan_speed(50_pct);
    }
};

template <pgns pgn>
void send(pdu<pgn>& p, arduino_ostream* out = nullptr)
{
    p.source_address(source_address());

    auto frame = ft::create(p.can_id(), p.data(), p.size());

    if(t.send(frame) == false && out != nullptr)
    {
        *out << F("Problem sending frame");

#ifdef AUTOWP_LIB
        // Coming back 'ERROR_ALLTXBUSY' which indicates 3 internal buffers
        // got filled for output, but output never happened
        // DEBT: casting to int is obnoxious, but we're still defaulting
        // to 'char' cast somehow
        *out << F(": ERROR=") << (int)t.last_error;
#endif

        *out << endl;
    }
}


void testOut()
{
    // TODO: Document difference between time_date and time_and_date
    pdu<pgns::time_date> p;

    send(p);

    //auto frame2 = ft::create(0x123);

    //t.send(frame2);

#if !defined(AUTOWP_LIB) && !defined(ESP_PLATFORM)
    CAN.beginPacket(0x123);
    CAN.endPacket();
#endif

}


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

        out << endl;

        pdu<pgn> p;

        impl.prep(p);

        send(p, &out);
    }
};

class InitiateNetworkCAAction : public menu::Action
{
    void render(ostream& out) const override
    {
        out << F("Start up Network (addressing) CA") << endl;
    }

    void action(ostream& out) override
    {
        if(nca.state == impl::network_ca_base::states::unstarted)
        {
            out << F("Starting network CA...");
            nca.start(t);
        }
        else
        {
            out << F("Already started");

            if(nca.state == impl::network_ca_base::states::claimed)
            {
                out << endl;
                out << F("Claimed address: ") << (unsigned) nca.address().value();
            }
        }

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


// This might be happier as a string_view
// DEBT: Consider mildly reworking basic_string to utiize/and or resemble something like a
// detail::basic_ostream - i.e. char, traits, allocator and maybe policy all tucked
// awai in an impl
// NOTE: Interesting discussion why this wasn't done in std
// https://stackoverflow.com/questions/39169310/why-is-there-no-stdfrom-string.
template <class TChar, class Traits, class Allocator, class Policy, class T>
estd::from_chars_result from_string(
    const estd::basic_string<TChar, Traits, Allocator, Policy>& s, T& v)
{
    const TChar* data = s.clock();
    estd::from_chars_result r = from_chars(data, data + s.size(), v);
    s.cunlock();
    return r;
}



void menu1(menu::Navigator* nav, ios io)
{
    using traits = arduino_istream::traits_type;

    menu::Menu* menu = nav->current();

    if(state == States::Entry)
    {
        cout << F("Top level: source_address=") << source_address() << endl << endl;
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

            estd::from_chars_result r1 = from_string(input, selected);
            if(selected == 0)
            {
                if(!nav->up())
                    cout << F("Topmost menu, cannot go up further");
                else
                    nav->current()->render(io.out);
            }
            else if(selected != -1)
            {
                expected<void, errc> r(menu->activate(selected - 1, io.out));

                /* FIX: something's wrong with copy constructor/assignment
                expected<void, errc> r = menu->activate(selected - 1); */

                if(r.has_value() == false)
                {
                    cout << F("Invalid menu item: ") << selected << endl;
                }
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

namespace layer1 {

struct exp_action1
{
    void render(arduino_ostream& out)
    {
        out << F("action1");
    }
};

struct exp_action2
{
    void render(arduino_ostream& out)
    {
        out << F("action2");
    }
};

using menu_type = menu::layer1::Menu<
    exp_action1,
    exp_action2>;

}

menu::Menu topLevel, submenu("Management", &topLevel);
SyntheticMenuAction item1{0}, item2{1};
menu::Navigator nav(&topLevel);
CanPGNAction<pgns::oel> item4;
CanPGNAction<pgns::cab_message1> item5;
menu::MenuAction subitem1(&nav, &submenu);

InitiateNetworkCAAction subitem1_1;

void setup() 
{
    Serial.begin(115200);

    topLevel.items.push_back(&item1);
    topLevel.items.push_back(&item2);
    topLevel.items.push_back(&subitem1);
    topLevel.items.push_back(&item4);
    topLevel.items.push_back(&item5);

    submenu.items.push_back(&subitem1_1);

    while(!Serial);

    init_can(t);

    // NOTE: Not verified if this works yet, because we are using prng
    srand(analogRead(0));

    ::layer1::menu_type{}.render(cout);
}


#define EXPLICIT_CAN_LOG 0

void nca_report()
{
    if(nca_last_state != nca.state)
    {
        // DEBT: I think ostream isn't reverting back to default hex/dec states properly
        cout << estd::dec;

        switch(nca.state)
        {
            case impl::network_ca_base::states::claimed:
                cout << F("Claimed address: ") << nca.address().value() << endl;
                break;

            default:
                if(nca_last_state != impl::network_ca_base::states::unstarted)
                    cout << F("Lost address") << endl;
                break;
        }

        nca_last_state = nca.state;
    }
}

void loop() 
{
    bool r = false;

#ifdef AUTOWP_LIB
    struct can_frame canMsg;
    MCP2515::ERROR e = t.mcp2515.readMessage(&canMsg);

    if (e == MCP2515::ERROR_OK)
    {
#if EXPLICIT_CAN_LOG
        // DEBT: Adapt and move this into the diagnostic fallback code - mostly done now
        // keeping this around just incase diagnostic code has issues
        cout << F("id: ") << hex << canMsg.can_id << ' ' << canMsg.can_dlc << ' ';

        for (int i = 0; i<canMsg.can_dlc; i++)
        {  // print the data
            Serial.print(canMsg.data[i],HEX);
            Serial.print(" ");
        }

        cout << endl;
#endif

#if FEATURE_AGGREGATED_CA
        process_incoming(app_ca, t, canMsg);
#else
        r = process_incoming(dca, t, canMsg);
        process_incoming(nca, t, canMsg);
#endif

        cout << endl;
    }
#else
    // try to parse packet
    transport::frame frame;

    if (can_online && t.receive(&frame))
    {
#if EXPLICIT_CAN_LOG
        // DEBT: See above EXPLICIT_CAN_LOG debt
        cout << F("Received packet with id 0x") << hex << frame.id;
        cout << ' ' << frame.dlc;
        cout << endl;
#endif

        r = process_incoming(dca, t, frame);
        process_incoming(nca, t, frame);
    }

    //testOut();
    //estd::this_thread::sleep_for(100ms);
#endif

    menu1(&nav, ios{cin, cout});

    scheduler.process();

    nca_report();
}
