/**
 * References:
 *
 * 1. J1939-21 (DEC2006)
 * 2. J1939-71 (MAR2011)
 * 3. J1939-81 (draft MAY2003)
 */
#pragma once

#include "fwd.h"
#include "function.h"
#include "industry_groups.h"
#include "vehicle_systems.h"

#include "../data_field/base.hpp"

#include "layer0.h"
#include "ostream.h"

namespace embr { namespace j1939 {

// [1], [2]
// [3] 4.1.1
// SAEnm01
template <class TContainer>
struct NAME : internal::data_field_base<TContainer>
{
    typedef internal::data_field_base<TContainer> base_type;

    // DEBT: Auto initializing to 0xFF which is likely not what we want
    // DEBT: As a consequence of above, reserved bit is not 0 as it should be as per [3] 4.1.1.6
    ESTD_CPP_FORWARDING_CTOR(NAME)

    struct d
    {
        static constexpr spn::descriptor arbitrary_address_capable()
        { return spn::descriptor{8, 8, 1}; }

        static constexpr spn::descriptor industry_group()
        { return spn::descriptor{8, 5, 3}; }

        static constexpr spn::descriptor vehicle_system_instance()
        { return spn::descriptor{8, 1, 4}; }

        static constexpr spn::descriptor manufacturer_code()
        { return spn::descriptor{3, 6, 11}; }

        static constexpr spn::descriptor identity_number()
        { return spn::descriptor{1, 1, 21}; }

        static constexpr spn::descriptor ecu_instance()
        { return spn::descriptor{5, 1, 5}; }

        static constexpr spn::descriptor function()
        { return spn::descriptor{6, 1, 8}; }

        static constexpr spn::descriptor function_instance()
        { return spn::descriptor{5, 4, 5}; }
    };

    inline bool arbitrary_address_capable() const
    {
        // DEBT: underlying 'material' doesn't handle bool gracefully yet
        //return base_type::template get<bool>(d::arbitrary_address_capable());
        return (bool)base_type::template get<unsigned>(d::arbitrary_address_capable());
    }

    inline void arbitrary_address_capable(bool v)
    {
        // DEBT: Casting to uint8_t
        base_type::set(d::arbitrary_address_capable(), (uint8_t)v);
    }

    inline void arbitrary_address_capable(word<1> v)
    {
        base_type::set(d::arbitrary_address_capable(), v);
    }

    inline const vehicle_systems
    vehicle_system() const
    {
        return (vehicle_systems) base_type::template get<uint8_t>(spn::descriptor{7, 2, 7});
    }

    inline void vehicle_system(word<7> v)
    {
        return base_type::template set(spn::descriptor{7, 2, 7}, v);
    }

    inline word<11> manufacturer_code() const
    {
        return base_type::template get<uint16_t>(d::manufacturer_code());
    }

    inline void manufacturer_code(word<11> v)
    {
        base_type::set(d::manufacturer_code(), v);
    }

    inline word<21> identity_number() const
    {
        return base_type::template get<uint32_t>(d::identity_number());
    }

    inline void identity_number(word<21> v)
    {
        base_type::set(d::identity_number(), v);
    }

    EMBR_J1939_PROPERTY_INLINE2(vehicle_system_instance)
    EMBR_J1939_PROPERTY_INLINE2(function_instance)
    EMBR_J1939_PROPERTY_INLINE2(industry_group)
    EMBR_J1939_PROPERTY_INLINE2(ecu_instance)

    EMBR_J1939_PROPERTY_INLINE(reserved, (spn::descriptor{7, 1, 1}))
    EMBR_J1939_PROPERTY_INLINE(function, (spn::descriptor{6, 1, 8}))


    // EXPERIMENTAL
    template <class TEnum>//, class THelper = j1939::experimental::get_helper<TEnum>,
        //estd::enable_if_t<estd::is_base_of<embr::j1939::experimental::get_helper_tag, THelper>::value, bool> = true>
    inline TEnum get_enum() const
    {
        return j1939::experimental::get_helper<TEnum>::get(*this);
    }

    // EXPERIMENTAL
    template <class TEnum>
    inline void set_enum(TEnum v)
    {
        j1939::experimental::get_helper<TEnum>::set(*this, v);
    }
};

// DEBT: There's got to be a more streamlined way of doing this.
namespace experimental {

// DEBT: Would be better to do descriptor_##name and deduce word in setter
// NOTE: This is a lot of convention to rely on, maybe too much
#define EMBR_J1939_GET_SET_ENUM(name, e) \
template <> struct get_helper<e> : get_helper_tag \
{                                        \
    template <class TContainer> \
    inline static e get(const NAME<TContainer>& n)   \
    {   \
        return (e) n.name().cvalue();     \
    }   \
                                         \
    template <class TContainer> \
    inline static void set(NAME<TContainer>& n, e v)   \
    {                                    \
        typedef decltype(n.name()) word_type;                                 \
        typedef typename word_type::type value_type;                                 \
        n.name((value_type)v);     \
    }   \
};





template <>
struct get_helper<function_fields> : get_helper_tag
{
    template <class TContainer>
    static function_fields get(const NAME<TContainer>& n)
    {
        return (function_fields) n.function().cvalue();
    }
};


template <>
struct get_helper<industry_groups> : get_helper_tag
{
    template <class TContainer>
    static industry_groups get(const NAME<TContainer>& n)
    {
        return (industry_groups) n.industry_group().cvalue();
    }
};


template <>
struct get_helper<vehicle_systems> : get_helper_tag
{
    template <class TContainer>
    constexpr static vehicle_systems get(const NAME<TContainer>& n)
    {
        //return (vehicle_systems) n.vehicle_system().cvalue();
        return (vehicle_systems) n.vehicle_system();
    }
};

}

}

template <class TContainer>
inline void set(j1939::NAME<TContainer>& n, j1939::function_fields v)
{
    n.function((uint8_t)v);
}

template <class TContainer>
inline void set(j1939::NAME<TContainer>& n, j1939::industry_groups v)
{
    n.industry_group((uint32_t)v);
}

template <class TContainer>
inline void set(j1939::NAME<TContainer>& n, j1939::vehicle_systems v)
{
    n.vehicle_system((uint8_t)v);
}

template <class TEnum, class TContainer>
inline TEnum get(const j1939::NAME<TContainer>& n)
{
    return j1939::experimental::get_helper<TEnum>::get(n);
}

}
