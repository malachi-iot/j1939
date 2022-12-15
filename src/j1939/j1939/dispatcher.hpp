#pragma once

#include <estd/variant.h>

#include "pdu.h"
#include "pgn.h"
#include "spn.h"
#include "data_field.h"

namespace embr { namespace j1939 {

namespace events {

template <pgns pgn>
struct received
{
    can_id id;
    data_field<pgn> data;

    received(can_id id, const uint8_t* data) :
        id(id),
        data(data) {}

    received(const received&) = default;
};

}

// NOTE: We might be able to somehow combine our estd dev/exp/type-id dispatch techniques for use here
template <class TSubject, class TContext>
void dispatch(pdu1_header id, const uint8_t* payload, TSubject& subject, TContext& context)
{
    pgns pgn = (pgns)id.pdu_format();

    switch(pgn)
    {
        case pgns::acknowledgement:
            subject.notify(events::received<pgns::acknowledgement>{id, payload}, context);
            break;

        case pgns::request:
            subject.notify(events::received<pgns::request>{id, payload}, context);
            break;

        case pgns::tp_cm:
        {
            subject.notify(events::received<pgns::tp_cm>{id, payload}, context);
            break;
        }

        case pgns::tp_dt:
            subject.notify(events::received<pgns::tp_dt>{id, payload}, context);
            break;

        default:
            break;
    }
}

// NOTE: We might be able to somehow combine our estd dev/exp/type-id dispatch techniques for use here
template <class TSubject, class TContext>
void dispatch(pdu2_header id, const uint8_t* payload, TSubject& subject, TContext& context)
{
    pgns pgn = (pgns)id.range();

    switch(pgn)
    {
        case pgns::oel:
            subject.notify(events::received<pgns::oel>(id, payload), context);
            break;

        case pgns::lighting_command:
            subject.notify(events::received<pgns::lighting_command>(id, payload), context);
            break;

        case pgns::electronic_engine_controller_1:
            subject.notify(events::received<pgns::lighting_command>(id, payload), context);
            break;

        case pgns::ecu_identification:
            subject.notify(events::received<pgns::ecu_identification>(id, payload), context);
            break;

        default:
            break;
    }
}

// NOTE: We might be able to somehow combine our estd dev/exp/type-id dispatch techniques for use here
template <class TSubject, class TContext>
void dispatch(can_id id, const uint8_t* payload, TSubject& subject, TContext& context)
{
    if(id.is_pdu1())
    {
        dispatch(pdu1_header{id}, payload, subject, context);
    }
    else
    {
        dispatch(pdu2_header{id}, payload, subject, context);
    }
}

template <class TSubject>
inline void dispatch(can_id id, const uint8_t* payload, TSubject& subject)
{
    estd::monostate context;

    dispatch(id, payload, subject, context);
}

}}