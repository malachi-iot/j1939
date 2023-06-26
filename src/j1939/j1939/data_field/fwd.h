#pragma once

#include <estd/array.h>

#include "../pgn/fwd.h"

namespace embr { namespace j1939 {

template<pgns pgn, class TContainer = estd::array<uint8_t,
    pgn::get_descriptor<pgn>().length> >
struct data_field;


namespace layer1 {

template<pgns pgn>
using data_field = embr::j1939::data_field<pgn>;

}

namespace layer2 {

template<pgns pgn>
using data_field = embr::j1939::data_field<pgn,
    estd::legacy::layer2::array<uint8_t, pgn::get_descriptor<pgn>().length> >;

}

namespace experimental {

struct get_helper_tag {};

template <class TEnum>
struct get_helper;

}


}}
