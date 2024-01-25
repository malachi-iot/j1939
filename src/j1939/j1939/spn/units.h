#pragma once

#include "enum.h"
#include "fwd.h"
#include "../slots/units.h"

#include <estd/internal/macro/push.h>

namespace embr { namespace j1939 { namespace spn { inline namespace v1 {

template <spns spn_>
struct unit<spn_, 
    estd::enable_if_t<
        estd::is_base_of<
            slot::v1::internal::slot_type_tag,
            spn::traits<spn_>
        >::value
    >
> : slot::unit<spn::traits<spn_>::slot>
{
    using traits = spn::traits<spn_>;
    using base_type = slot::unit<traits::slot>;

    template <class ...Args>
    constexpr unit(Args&&...args) :
        base_type(std::forward<Args>(args)...)
    {}

    static constexpr spns spn() { return spn_; }
};


template <spns spn_>
struct unit<spn_,
    estd::enable_if_t<
        estd::is_base_of<
            // Interestingly, SFINAE prefers this over a floating 'class Enum' up top.
            // since we "fail silently" if enum_type isn't present, this causes no issues
            spn::internal::enum_traits_base<typename spn::traits<spn_>::enum_type>,
            spn::traits<spn_>
        >::value
    >
>
{
    using traits = spn::traits<spn_>;

    using enum_type = typename traits::enum_type;

private:
    enum_type value_;

public:
    explicit constexpr unit(enum_type value) : value_{value} {}
    unit() = default;
    unit(const unit&) = default;
    unit& operator=(const unit&) = default;
    unit& operator=(enum_type copy_from)
    {
        value_ = copy_from;
        return *this;
    }

    constexpr bool operator==(enum_type rhs) const
    {
        return value_ == rhs;
    }

    constexpr operator enum_type() const
    {
        return value_;
    }

    static constexpr spns spn() { return spn_; }

    constexpr bool is_noop() const
    {
        return value_ == enum_type::noop;
    }

    enum_type value() const { return value_; }
};

}}}}

#include <estd/internal/macro/pop.h>
