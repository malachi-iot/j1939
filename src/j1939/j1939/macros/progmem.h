#pragma once

// FIX: Not fully worked out yet
/*
#ifdef __AVR__
// UNTESTED
#define EMBR_PROGMEM_STRING(name, v)    \
static constexpr const char name[] = v;
#else
#define EMBR_PROGMEM_STRING(name, v)    \
static constexpr const char name[] = v;
#endif
*/
#define EMBR_PROGMEM_STRING(name, v)    \
static constexpr const char* name() { return v; }

#define EMBR_J1939_PGN_TRAIT(abbrev_, name_)    \
EMBR_PROGMEM_STRING(abbrev, abbrev_)    \
EMBR_PROGMEM_STRING(name, name_)

