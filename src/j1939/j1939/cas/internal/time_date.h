#pragma once

#include <estd/chrono.h>

namespace embr { namespace j1939 { namespace internal {

// Specifically for use with scheduler
// Depends on C standard time/gmtime
template <class Transport>
struct emit_time_date_functor
{
    Transport& transport;

    // DEBT: Prefer a bit more interaction with notifies, but this ought
    // to do in the short term
    const uint8_t& source_address;

    using ft = embr::can::frame_traits<typename Transport::frame>;

    template <class TimePoint>
    void operator()(TimePoint* wake, TimePoint current)
    {
        time_t timer = time(nullptr);
        struct tm* buf = gmtime(&timer);

        pdu<pgns::time_date> message;

        message.hours(buf->tm_hour);
        message.minutes(buf->tm_min);
        message.seconds(buf->tm_sec);
        message.day(buf->tm_mday);
        message.month(buf->tm_mon);

        message.source_address(source_address);

        auto frame = ft::create(message.can_id(),
            message.data(),
            message.size());

        // DEBT: Need to pay attention to send failures
        transport.send(frame)

        *wake += estd::chrono::seconds(10);

        //cout << F("Emitting: ") << message.seconds().count() << estd::endl;
    }
};

}}}