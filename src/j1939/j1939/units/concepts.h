#pragma once

#if __cpp_concepts
#include <concepts>

namespace embr::units::internal {

template<class TAdder>
concept Adder =
requires(TAdder adder)
{
    typename TAdder::int_type;
    { adder(int{}) } -> std::same_as<typename TAdder::int_type>;
};

}

#endif

