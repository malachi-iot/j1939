#pragma once

#include <esp_log.h>

#include <estd/streambuf.h>
#include <estd/string.h>

namespace esp_idf { namespace impl {

// DEBT: Use value_evaporator for tag provider

namespace layer0 {

template <const char* TAG_>
struct tag_provider
{
    static constexpr const char* TAG() { return TAG_; };
};

}

struct tag_provider
{
    const char* TAG_;

    constexpr const char* TAG() { return TAG_; }
};

template <class Traits>
class log_ostreambuf :
    public estd::internal::impl::out_pos_streambuf_base<Traits>,
    public tag_provider
{
    using base_type = estd::internal::impl::streambuf_base<Traits>;

public:
    using traits_type = Traits;
    using typename base_type::char_type;
    using typename base_type::int_type;
    using tp_type = tag_provider;

protected:
    static constexpr const estd::size_t N = 32;

    // Trouble in paradise, esp-idf's esp_log_writev doesn't take
    // a count, mandating a double-buffer situation

    // DEBT: Resolve that Traits could specify a non-char char_type
    estd::layer1::string<N> s_;

    // DEBT: Make all this protected too
public:
    esp_log_level_t log_level_;

    void sync()
    {
        if(s_.empty()) return;

        // DEBT: Should probably use esp_log_writev here        
        esp_log_write(log_level_, tp_type::TAG(), s_.data());
        s_.clear();
    }

    estd::streamsize xsputn(const char_type* s, estd::streamsize count)
    {
        // crude overflow accomodator.  Needs more work
        if(s_.size() + count > s_.max_size())
            sync();

        // We get no indication whether append is successful.  Also not
        // sure if underlying string handles overflow gracefully (truncate)
        // DEBT: Consider either deviating from spec and returning `expected`
        // from append, or make some append-like API with that.  If fail state,
        // would return a special object denoting how many bytes *were* written
        s_.append(s, count);

        // DEBT: Make an s_.full() since layer1-layer3 that makes a lot of sense
        // where a std::string it wouldn't.  That way we wouldn't have to do
        // an underlying strlen each time for this
        if(s_.size() == s_.max_size())

        return 0;
        //return underlying().write((const uint8_t*)s, count);
    }

    int_type sputc(char_type ch)
    {
        s_ += ch;
        return traits_type::to_int_type(ch);
    }
};

}}