#pragma once

#include <esp_log.h>

#include <estd/streambuf.h>
#include <estd/string.h>

namespace esp_idf {


namespace impl {

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
class log_ostreambuf;

struct setlevel : estd::internal::ostream_functor_tag
{
    const esp_log_level_t log_level_;

    constexpr setlevel(esp_log_level_t v) : log_level_{v} {}

    // DEBT: Should filter by proper stream here, but does work
    template <class TStreambuf, class TBase>
    void operator()(estd::detail::basic_ostream<TStreambuf, TBase>& out) const
    {
        auto rdbuf = out.rdbuf();

        rdbuf->pubsync();
        rdbuf->log_level_ = log_level_;
    }
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
    bool header_emitted_ = false;

    int sync()
    {
        if(s_.empty()) return 0;

        // DEBT: Should probably use esp_log_writev here        
        esp_log_write(log_level_, tp_type::TAG(), s_.data());
        s_.clear();

        return 0;
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
        //if(s_.size() == s_.max_size())
        //    return 0;
        //return underlying().write((const uint8_t*)s, count);

        return count;
    }

    int_type sputc(char_type ch)
    {
        s_ += ch;
        return traits_type::to_int_type(ch);
    }
};

template <class Traits>
using _log_ostreambuf = estd::internal::streambuf<log_ostreambuf<Traits> >;

}

template <class CharT, class Traits = estd::char_traits<CharT>>
using log_ostreambuf = impl::_log_ostreambuf<Traits>;


template <class CharT, class Traits = estd::char_traits<CharT>>
using basic_log_ostream = estd::detail::basic_ostream<esp_idf::log_ostreambuf<CharT, Traits>>;

using log_ostream = basic_log_ostream<char>;

constexpr impl::setlevel level_set(esp_log_level_t log_level)
{
    return { log_level };
}

template <class Traits>
constexpr estd::detail::basic_ostream<impl::_log_ostreambuf<Traits>>& info(
    estd::detail::basic_ostream<impl::_log_ostreambuf<Traits>>& out)
{
    return out << level_set(ESP_LOG_INFO);
}

template <class Traits>
constexpr estd::detail::basic_ostream<impl::_log_ostreambuf<Traits>>& verbose(
    estd::detail::basic_ostream<impl::_log_ostreambuf<Traits>>& out)
{
    return out << level_set(ESP_LOG_VERBOSE);
}


template <class Traits>
constexpr estd::detail::basic_ostream<impl::_log_ostreambuf<Traits> >& warn(
    estd::detail::basic_ostream<impl::_log_ostreambuf<Traits> >& out)
{
    return out << level_set(ESP_LOG_WARN);
}


}