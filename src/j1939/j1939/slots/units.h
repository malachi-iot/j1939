#pragma once

#include "../spn/enum.h"
#include "fwd.h"

#include <estd/internal/macro/push.h>

// EXPERIMENTAL

namespace embr { namespace j1939 { namespace slot { inline namespace v1 {

template <slots slot_>
struct unit : slot_traits<slot_>::type
{
    using traits_type = slot_traits<slot_>;
    using h = typename traits_type::h;
    using base_type = typename traits_type::type;
    using typename base_type::root_rep;
    using na = spn::ranges::not_available<root_rep>;
    using vs = spn::ranges::valid_signal<root_rep>;

    template <class ...Args>
    constexpr unit(Args&&...args) :
        base_type(std::forward<Args>(args)...)
    {}

    // DEBT: What should we name these? Ambiguity between:
    // 1) raw j1939 integer min/max
    // 2) slot offset'd min/max (though that's handled through offset() and range())
    static constexpr unsigned min() { return vs::min(); }
    static constexpr unsigned max() { return vs::max(); }

    constexpr bool is_noop() const
    {
        return na::noop == base_type::root_count();
    }

    static constexpr typename h::offset_type offset()
    {
        return h::get_offset();
    }

    using range_type = embr::units::internal::unit_base<
        typename base_type::root_rep,
        typename base_type::period,
        typename base_type::tag_type>;

    static constexpr range_type range()
    {
        return range_type(max());;
    }
};

}}}}

// DEBT: Put this elsewhere
namespace embr { namespace j1939 { namespace spn { inline namespace v1 {

// DEBT: This demands a slot-based spn
template <spns spn_>
struct unit : slot::unit<spn::traits<spn_>::slot>
{
    using traits = spn::traits<spn_>;
    using base_type = slot::unit<traits::slot>;

    template <class ...Args>
    constexpr unit(Args&&...args) :
        base_type(std::forward<Args>(args)...)
    {}

    static constexpr spns spn() { return spn_; }
};

}}}}

#include <estd/internal/macro/pop.h>
