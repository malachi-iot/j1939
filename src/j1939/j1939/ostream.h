#include <estd/ostream.h>

#include "fwd.h"

#include "units/ostream.h"

namespace embr { namespace j1939 {

// DEBT: Probably would prefer this off in special pdu area
template <pgns pgn, class TStreambuf, class TBase>
estd::detail::basic_ostream<TStreambuf, TBase>& operator <<(
    estd::detail::basic_ostream<TStreambuf, TBase>& out,
    const pdu<pgn>& name)
{
}

}}