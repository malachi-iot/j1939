/**
 * References:
 *
 * 1. J1939-21 REV. DEC2006
 * 2. https://www.csselectronics.com/pages/j1939-explained-simple-intro-tutorial
 */
#pragma once

#include "fwd.h"

//#include "bits.hpp"
#include "can_id.h"
#include "pdu/header.h"
#include "pgn/enum.h"
#include "data_field.h"

#include <can/transport.h>

// Deviates from [1] 5.3 in that we do not include data field in pdu1 or pdu2.
// priority field is included.

namespace embr { namespace j1939 {



template <pgns pgn_>
class pdu1 : public pdu1_header,
    public data_field<pgn_>
{
    typedef pdu1_header id;
    typedef data_field<pgn_> data_field_type;

public:
    static constexpr pgns pgn = pgn_;
    static constexpr pgn::descriptor descriptor() { return pgn::get_descriptor<pgn>(); }

    pdu1() : id{descriptor().default_priority, pgn} {}

    ///
    /// @param _id - undefined if 'range' does not match template pgn
    /// @param data
    pdu1(can_id _id, const uint8_t* data) :
        id{_id},
        data_field_type{data}
    {}

    const pdu1_header& can_id() const { return *this; }
    pdu1_header& can_id() { return *this; }
    const data_field_type& payload() const { return *this; }
    data_field_type& payload() { return *this; }
};

template <pgns pgn_>
class pdu2 : public pdu2_header,
    public data_field<pgn_>
{
    typedef pdu2_header id;
    typedef data_field<pgn_> data_field_type;

public:
    static constexpr pgns pgn = pgn_;
    static constexpr pgn::descriptor descriptor() { return pgn::get_descriptor<pgn>(); }

    pdu2() : id{descriptor().default_priority, pgn} {}

    ///
    /// @param _id - undefined if 'range' does not match template pgn
    /// @param data
    pdu2(can_id _id, const uint8_t* data) :
        id{_id},
        data_field_type{data}
    {}

    const pdu2_header& can_id() const { return *this; }
    pdu2_header& can_id() { return *this; }
    const data_field_type& payload() const { return *this; }
    data_field_type& payload() { return *this; }
};


template <pgns pgn>
class pdu<pgn, void, internal::Range<(pgn < pgns::pdu2_boundary)> > : public pdu1<pgn>
{
    typedef pdu1<pgn> base_type;

public:
    ESTD_CPP_FORWARDING_CTOR(pdu)
};

template <pgns pgn>
class pdu<pgn, void, internal::Range<(pgn >= pgns::pdu2_boundary)> > : public pdu2<pgn>
{
    typedef pdu2<pgn> base_type;

public:
    ESTD_CPP_FORWARDING_CTOR(pdu)
};

// EXPERIMENTAL
template <class TFrame>
struct frame_traits
{
    typedef TFrame frame_type;

    using can_frame_traits = embr::can::frame_traits<frame_type>;

    // Create "plain old" CAN frame from pdu
    template <pgns pgn>
    static inline frame_type create(const pdu<pgn> p)
    {
        return can_frame_traits::create(
            p.can_id(),
            p.data(),
            p.size());
    }
};


// EXPERIMENTAL
template <class TTransport>
struct transport_traits
{
    using transport_type = TTransport;
    using frame_type = typename transport_type::frame;

    // DEBT: Consider a translated/specialized/more informative return code
    template <pgns pgn>
    inline static bool send(transport_type& t, const pdu<pgn>& p)
    {
        frame_type f = frame_traits<frame_type>::create(p);

        return t.send(f);
    }
};


}}
