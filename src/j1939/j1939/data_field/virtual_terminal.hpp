/**
 *  Public information on ISO 11783 in general is way more scarce than the
 *  already-scarce j1939.  Educated guess indicates that vt12 and vt21 pgns
 *  are ASCII carrying USB-CDC like messages.
 *
 *  Virtual Terminal in particular is covered by ISO 11783-6
 *
 * References:
 *
 * 1. RESERVED
 * 2. http://www.tfzr.rs/Content/files/0/Lista_PGN-ova.xls
 */
#pragma once

#include "base.hpp"

// We can infer from [2] "variable" length that tp_dt is at play meaning possibly
// regular data_field_base application should be displaced by not-quite-ready
// streaming style interface.

// Also, there was an offhanded mention somewhere that ISO 11783 has its own
// mechanism for handling frames over 8 bytes

namespace embr { namespace j1939 {

template <class TContainer>
struct data_field<pgns::vt12, TContainer> : internal::data_field_base<TContainer>
{

};


template <class TContainer>
struct data_field<pgns::vt21, TContainer> : internal::data_field_base<TContainer>
{

};

}}