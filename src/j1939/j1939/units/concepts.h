#pragma once

#if __cpp_concepts
#include <concepts>

namespace embr::units::internal {

// DEBT: Should name this something more like Adjuster
template<class T, typename TInt = int>
concept Adder = requires(T adder)
{
    typename T::int_type;
    { adder(TInt{}) } -> std::same_as<typename T::int_type>;
};

}

#endif

