#pragma once

#include "../data_field/fwd.h"

namespace embr { namespace j1939 {

template <class TContainer>
struct NAME;

#if __cpp_concepts
#endif

namespace layer1 {

using NAME = j1939::NAME<estd::array<uint8_t, 8> >;

}

}}