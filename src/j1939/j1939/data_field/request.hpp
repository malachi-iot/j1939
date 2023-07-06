#include "base.hpp"

namespace embr { namespace j1939 {

namespace spn {

template <>
struct type_traits<spns::parameter_group_number_rqst> : internal::type_traits_base<uint32_t>
{

};

template<>
constexpr descriptor get_descriptor<spns::parameter_group_number_rqst>()
{
    return descriptor{1, 1, 24};
}

}

template <class TContainer>
struct data_field<pgns::request, TContainer> :
    internal::data_field_base<TContainer>
{
    struct d
    {
        //static constexpr spn::descriptor pgn()
        //{ return spn::descriptor{}}
    };

    typedef internal::data_field_base<TContainer> base_type;

    ESTD_CPP_FORWARDING_CTOR(data_field)

    uint32_t pgn() const
    {
        return base_type::template get<spns::parameter_group_number_rqst>();
    }

    void pgn(uint32_t v)
    {
        return base_type::template set<spns::parameter_group_number_rqst>(v);
    }
};

}}
