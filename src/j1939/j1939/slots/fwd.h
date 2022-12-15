#pragma once

#include "enum.h"

namespace embr { namespace j1939 {

template <slots slot>
struct slot_traits;

template <slots slot>
using slot_type = typename slot_traits<slot>::type;

}}