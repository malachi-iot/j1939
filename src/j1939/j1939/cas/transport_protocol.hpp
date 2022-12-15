/**
 *
 * Supports J1939-21 [1] 5.10 "Transport protocol Functions"
 *
 * References:
 *
 * 1. J1939-21 (DEC2006)
 */
#include "../ca.h"
#include "../data_field/transport_protocol.hpp"

namespace embr { namespace j1939 {

namespace impl {

// Pertains to [1] 5.10
template <class TTransport>
struct transport_protocol_ca : impl::controller_application<TTransport>
{
    typedef j1939::impl::controller_application<TTransport> base_type;
    using typename base_type::transport_type;
    using typename base_type::frame_type;
    using typename base_type::frame_traits;

    template <pgns pgn>
    using pdu = pdu<pgn>;

    typedef transport_traits<transport_type> _transport_traits;

    using modes = pdu<pgns::tp_cm>::mode;

    // virtual connection as per [1] 5.10.3.1
    struct connection
    {
        uint16_t total_size;
        uint16_t remaining_size;    // DEBT: Rework this by doing total_size - sequence * 7
        pgns pgn;
        uint8_t sequence;
    };

    struct receive_connection : connection
    {
    };

    struct inline_receive_connection : receive_connection
    {
        uint8_t buffer[32];
    };

    inline_receive_connection test_connection;

    template <pgns pgn>
    inline bool process_incoming(transport_type& t, pdu<pgn> p) { return false; }

    // [1] 5.10.3.1
    // NOTE:
    // "If multiple RTSs are received from the same source address for the same PGN, then the most recent RTS shall be acted
    //  on and the previous RTSs will be abandoned. No abort message shall be sent for the abandoned RTSs in this specific
    //  case."
    void send_rts(transport_type& t, connection& c, bool bam = false)
    {
        pdu<pgns::tp_cm> p_rts;

        p_rts.payload().control(bam ? modes::bam : modes::rts);
        p_rts.payload().pgn((uint32_t)c.pgn);
        p_rts.payload().total_size(c.total_size);

        uint8_t total_packets = (7 + c.total_size) / 7;

        p_rts.payload().total_packets(total_packets);

        // DEBT: leaving max_packets at 0xFF (unlimited)
    }

    inline void send_bam(transport_type& t, connection& c)
    {
        send_rts(t, c, true);
    }

    // [1] 5.10.3.2
    void send_cts(transport_type& t, const pdu<pgns::tp_cm>& p)
    {
        pdu<pgns::tp_cm> p_resp;

        p_resp.payload().control(modes::cts);
        p_resp.can_id().destination_address(p.source_address());
        p_resp.payload().pgn(p.payload().pgn());

        // DEBT: Almost definitely these will need to be smarter for abort/resume
        p_resp.payload().can_send(p.max_packets());
        p_resp.payload().to_send(1);

        _transport_traits::send(t, p_resp);
    }

    // [1] 5.10.3.3
    void send_ack(transport_type& t, connection& c)
    {
        pdu<pgns::tp_cm> p_ack;

        p_ack.payload().control(modes::ack);
        p_ack.payload().total_size(c.total_size);
        p_ack.payload().total_packets(c.sequence);
        p_ack.payload().pgn((uint32_t)c.pgn);
    }

    // [1] 5.10.3.4
    void send_abort(transport_type& t, connection& c)
    {
        pdu<pgns::tp_cm> p_abort;

        p_abort.payload().control(modes::abort);
        p_abort.payload().pgn((uint32_t)c.pgn);
    }


    void receive_abort(transport_type& t, const pdu<pgns::tp_cm>& p, connection& c)
    {

    }

    void receive_abort(transport_type& t, const pdu<pgns::tp_cm>& p)
    {

    }

    void receive_ack(transport_type& t, const pdu<pgns::tp_cm>& p)
    {

    }

    bool process_incoming(transport_type& t, const pdu<pgns::tp_cm>& p)
    {
        switch(p.control())
        {
            case modes::bam:
                break;

            case modes::abort:
                receive_abort(t, p);
                break;

            case modes::ack:
                receive_ack(t, p);
                break;

            case modes::cts:
                break;

            // [1] 5.10.3.1
            case modes::rts:
                send_cts(t, p);
                break;

            default:
                return false;
        }
        return true;
    }

    void emit(const pdu<pgns::tp_dt>& p)
    {
        // TODO: Hand off to local CAs.  TBD whether they do manual reassembly or we do, though
        // for relatively small packets (say < 32 bytes) it makes sense for us to manually reassemble
        // otherwise stream processing is likely preferable, to avoid memory bloat
    }


    // [1] 5.10.4
    bool process_incoming(transport_type& t, const pdu<pgns::tp_dt>& p)
    {
        return false;
    }
};

}

}}