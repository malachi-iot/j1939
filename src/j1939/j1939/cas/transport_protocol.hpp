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

#include <estd/internal/streambuf.h>

namespace embr { namespace j1939 {

namespace transport {

// virtual connection as per [1] 5.10.3.1
struct connection
{
    uint32_t pgn;
    uint16_t total_size;
    uint16_t remaining_size;    // DEBT: Rework this by doing total_size - sequence * 7
    //pgns pgn;
    uint8_t sequence;           ///<! total sequence/packets (corresponds to total_size)
};

}

namespace impl {

// DEBT: Use estd's streambuf pos helper base class
template <class Transport, class Traits>
class out_tp_dt_streambuf : public estd::internal::impl::streambuf_base<Traits>
{
    using base_type = estd::internal::impl::streambuf_base<Traits>;

    // DEBT: Do evaporate on this guy
    Transport& transport_;
    //transport::connection& connection_;
    pdu<pgns::tp_dt> p;
    unsigned pos_ = 0;
    using transport_type = Transport;
    using tt = transport_traits<transport_type>;
    //using ft = frame_traits<typename transport_type::frame_type>;

protected:
    int sync(bool* eol = nullptr)
    {
        // Can't send partial packets
        if(pos_ != 7)   return -1;

        p.sequence_number(p.sequence_number() + 1);

        // DEBT: Do a non blocking version of this and update eol properly
        tt::send(transport_, p);

        if(eol) *eol = false;

        pos_ = 0;

        return 0;
    }

public:
    using typename base_type::char_type;
    using typename base_type::int_type;

    // Check this after sending last bit of data and make sure we match
    // to connection->sequence
    constexpr unsigned sequence_number() const
    {
        return p.sequence_number();
    }

    /*
    out_tp_dt_streambuf(Transport& transport, transport::connection& connection) :
        transport_(transport),
        connection_(connection)
    {}  */

    out_tp_dt_streambuf(Transport& transport, uint8_t source_addr, uint8_t dest_addr) :
        transport_{transport}
    {
        p.source_address(source_addr);
        p.destination_address(dest_addr);
    }

    constexpr char_type* pbase() const
    {
        return reinterpret_cast<char_type*>(const_cast<uint8_t*>(p.data()));
    }

    char_type* pptr() const { return pbase() + pos_; }
    char_type* epptr() const { return pbase() + 7; }

    int_type overflow(int_type c)
    {
        bool eol;

        sync(&eol);

        if(eol) return Traits::eof();

        if(Traits::not_eof(c))
        {
            *pptr() = Traits::to_char_type(c);
            pos_++;
        }

        return c;
    }

    int_type sputc(char_type c)
    {
        return overflow(c);
    }

    int xsputn_ideal(const char_type* data, estd::size_t sz)
    {
        unsigned remaining = 7 - pos_;

        // DEBT: I think sputn does this for us... maybe?
        if(remaining == 0)
        {
            sync();
            remaining = 7;
        }

        sz = sz > remaining ? remaining : sz;

        memcpy(pptr(), data, sz);

        pos_ += sz;

        return sz;
    }

    // DEBT: Since neither ostream nor sputn wrapper auto loop, we have to.
    // it's OK to auto loop since:
    // a) it's expected underlying streambuf xsputn never blocks
    // b) in the case where xputn can't output further data, the sputn call to
    //    overflow would notice this and do an eof
    // https://github.com/malachi-iot/estdlib/issues/25
    estd::streamsize xsputn(const char_type* data, estd::streamsize sz)
    {
        // DEBT: No error/block checking - we'll do that with 'ideal' flavor
        // up above

        estd::streamsize orig = sz;

        while(sz > 0)
        {
            bool eol;

            sync(&eol);

            if(eol) return orig - sz;

            int chunk = sz > 7 ? 7 : (int)sz;

            memcpy(pptr(), data, chunk);
            pos_ += chunk;

            sz -= chunk;
        }

        return orig;
    }
};

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

    using connection = transport::connection;

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

        _transport_traits::send(t, p_rts);
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

        _transport_traits::send(t, p_ack);
    }

    // [1] 5.10.3.4
    void send_abort(transport_type& t, connection& c)
    {
        pdu<pgns::tp_cm> p_abort;

        p_abort.payload().control(modes::abort);
        p_abort.payload().pgn((uint32_t)c.pgn);

        _transport_traits::send(t, p_abort);
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

    void receive_cts(transport_type& t, const pdu<pgns::tp_cm>& p)
    {

    }

    void receive_rts(transport_type& t, const pdu<pgns::tp_cm>& p)
    {
        connection* c = &test_connection;

        c->pgn = p.payload().pgn();
        c->total_size = p.payload().total_size().value();
        c->sequence = p.payload().max_packets();

        send_cts(t, p);
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
                receive_cts(t, p);
                break;

            // [1] 5.10.3.1
            case modes::rts:
                receive_rts(t, p);
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

    // DEBT: For now we can only do one at a time, send or receive
    connection* get_connection(const pdu<pgns::tp_dt>&)
    {
        return &test_connection;
    }


    // [1] 5.10.4
    bool process_incoming(transport_type& t, const pdu<pgns::tp_dt>& p)
    {
        connection* c = get_connection(p);

        // TODO: Do something with this data!
        //p.data();

        if(p.sequence_number() == c->sequence)
        {

        }

        return false;
    }
};

}

// DEBT: Move this over to estd::detail::streambuf
template <class Transport, class Traits = estd::char_traits<char> >
using out_tp_dt_streambuf = estd::internal::streambuf<
    impl::out_tp_dt_streambuf<Transport, Traits>>;

}}