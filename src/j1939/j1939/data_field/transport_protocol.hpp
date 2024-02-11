/**
 *
 * Supports J1939-21 [1] 5.10 "Transport protocol Functions"
 *
 * References:
 *
 * 1. J1939-21 (DEC2006)
 * 2. https://copperhilltech.com/blog/sae-j1939-programming-with-arduino-multipacket-broadcast-bam-session/
 * 3. https://stackoverflow.com/questions/8016780/undefined-reference-to-static-constexpr-char
 */
#pragma once

#include "base.hpp"

namespace embr { namespace j1939 {

namespace spn {

}


// [1] Section 5.10.3
template <class TContainer>
struct data_field<pgns::tp_cm, TContainer> :
    internal::data_field_base<TContainer>
{
    typedef internal::data_field_base<TContainer> base_type;

    ESTD_CPP_FORWARDING_CTOR(data_field)

    enum mode
    {
        rts = 16,           ///< Destination Specific Request To Send [1] 5.10.3.1
        cts = 17,           ///< Destination Specific Clear To Send
        ack = 19,           ///< End of message acknowledge
        bam = 32,           ///< Broadcast Announce Message
        abort = 255
    };

    // We are forced to do inline methods here rather than pure constexpr's due to a clumsy behavior
    // of C++11 [3]
    struct d
    {
        static constexpr spn::descriptor control_byte {1, 1, 8};
        static constexpr spn::descriptor pgn()
            { return spn::descriptor{6, 1, 24}; };

        // CM_RTS mode
        // Also used by CM_BAM, CM_EndOfMsgACK
        static constexpr spn::descriptor total_size()
            { return spn::descriptor{2, 1, 16}; }
        // Also used by CM_BAM, CM_EndOfMsgACK
        static constexpr spn::descriptor total_packets()
            { return spn::descriptor{4, 1, 8}; }
        static constexpr spn::descriptor max_packets()
            { return spn::descriptor{5, 1, 8}; }

        // CM_CTS mode
        static constexpr spn::descriptor can_send()
            { return spn::descriptor{2, 1, 8}; }
        static constexpr spn::descriptor to_send()
            { return spn::descriptor{3, 1, 8}; }

        // Conn_Abort mode
        static constexpr spn::descriptor abort_reason()
            { return spn::descriptor{2, 1, 8}; }
    };

    inline mode control() const
    {
        constexpr spn::descriptor _d = d::control_byte;
        return (mode)base_type::template get<uint8_t>(_d);
    }

    void control(mode m)
    {
        constexpr spn::descriptor _d = d::control_byte;
        return base_type::template set<uint8_t>(_d, m);
    }

    uint8_t max_packets() const
    {
        return base_type::template get<uint8_t>(d::max_packets());
    }

    void max_packets(uint8_t v)
    {
        return base_type::template set<uint8_t>(d::max_packets(), v);
    }

    void pgn(uint32_t v)
    {
        base_type::template set<uint32_t>(d::pgn(), v);
    }

    uint32_t pgn() const
    {
        return base_type::template get<uint32_t>(d::pgn());
    }

    // RTS [1] 5.10.3.1
    EMBR_J1939_PROPERTY_INLINE2(total_size)
    EMBR_J1939_PROPERTY_INLINE2(total_packets)

    // CTS [1] 5.10.3.2
    EMBR_J1939_PROPERTY_INLINE2(can_send)
    EMBR_J1939_PROPERTY_INLINE2(to_send)
};


template <class TContainer>
struct data_field<pgns::tp_dt, TContainer> :
    internal::data_field_base<TContainer>
{
    typedef internal::data_field_base<TContainer> base_type;

    struct d
    {
        static constexpr spn::descriptor sequence_number()
        {
            return {1, 1, 8};
        }
    };

    ESTD_CPP_FORWARDING_CTOR(data_field)

    uint8_t sequence_number() const
    {
        return base_type::template get<uint8_t>(d::sequence_number());
    }

    void sequence_number(uint8_t v)
    {
        base_type::set(d::sequence_number(), v);
    }

    // DEBT: Kinda confusing two different data()
    uint8_t* data() { return base_type::data() + 1; }
    const uint8_t* data() const { return base_type::data() + 1; }
};

}}