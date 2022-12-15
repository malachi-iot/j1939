/**
 * References:
 *
 * 1. J1939-21 REV. DEC2006
 * 2. https://www.csselectronics.com/pages/j1939-explained-simple-intro-tutorial
 */
#pragma once

//#include "bits.hpp"
#include "can_id.h"
#include "pgn/enum.h"
#include "data_field.h"

#include <can/transport.h>

// Deviates from [1] 5.3 in that we do not include data field in pdu1 or pdu2.
// priority field is included.

namespace embr { namespace j1939 {

#if UNUSED
// As per [1] Section 5.2
// NOTE: This is PDU minus the 'Data Field' portion, thus the term 'header' whose name does not
// appear in the spec specifically
struct pdu_header : bits::material<bits::little_endian, bits::lsb_to_msb>
{
    typedef bits::material<bits::little_endian, bits::lsb_to_msb> base_type;

    struct d
    {
        static constexpr bits::descriptor P = bits::descriptor{2, 3};
        static constexpr bits::descriptor EDP = bits::descriptor{1, 1};
    };

    uint8_t priority() const { return get<uint8_t>(0, d::P); }
    void priority(uint8_t v) { set<uint8_t>(0, d::P, v); }

    bool extended_data_page() const { return get<bool>(0, d::EDP); }
    void extended_data_page(bool v) { set(0, d::EDP, v); }
};
#endif

struct pdu1_header : can_id
{
    constexpr pdu1_header(uint32_t v) : can_id(v) {}

    // EXPERIMENTAL
    constexpr pdu1_header(uint8_t priority, pgns pgn) :
    can_id{((uint32_t)priority << d::priority().bitpos) | (((uint32_t)pgn) << d::range_pdu1().bitpos)}
    {}

    uint8_t destination_address() const { return pdu_specific(); }
    void destination_address(uint8_t v) { pdu_specific(v); }

    // Limited 10-bit pdu1 command range
    uint16_t range() const { return value.get(d::range_pdu1()); }

    void range(uint16_t v) { value.set(d::range_pdu1(), v); }
};

struct pdu2_header : can_id
{
    constexpr pdu2_header(uint32_t v) : can_id(v) {}

    constexpr pdu2_header(uint8_t priority, pgns pgn) :
        can_id{((uint32_t)priority << d::priority().bitpos) | (((uint32_t)pgn) << d::range_pdu2().bitpos)}
    {}

    template <pgns pgn>
    static constexpr pdu2_header create()
    {
        return pdu2_header(pgn::get_descriptor<pgn>().default_priority, pgn);
    }

    // Full 18-bit PGN range
    uint32_t range() const { return value.get(d::range_pdu2()); }

    void range(uint32_t v) { value.set(d::range_pdu2(), v); }
};

namespace internal {

template <bool v>
using Range = estd::internal::Range<v>;

}

// TODO: Consider further specializing these based on transport, so that we can use native
// types to avoid copying and unnecessary allocating.  If so, it makes sense to do that by policy

///
/// @tparam pgn
/// @tparam TPolicy - place where perhaps we can specify underlying storage class preferences, etc
template <pgns pgn, class TPolicy = void, typename = internal::Range<true> >
class pdu;

template <pgns _pgn>
class pdu1 : public pdu1_header,
    public data_field<_pgn>
{
    typedef pdu1_header id;
    typedef data_field<_pgn> data_field_type;

public:
    static constexpr pgns pgn = _pgn;
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

template <pgns _pgn>
class pdu2 : public pdu2_header,
    public data_field<_pgn>
{
    typedef pdu2_header id;
    typedef data_field<_pgn> data_field_type;

public:
    static constexpr pgns pgn = _pgn;
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

    template <pgns pgn>
    inline static bool send(transport_type& t, const pdu<pgn>& p)
    {
        frame_type f = frame_traits<frame_type>::create(p);

        return t.send(f);
    }
};

inline bool is_bam(const pdu1_header& id)
{
    return id.destination_address() == spn::internal::address_type_traits_base::global;
}




}}