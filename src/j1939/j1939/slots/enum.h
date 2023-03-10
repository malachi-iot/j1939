/**
 * References:
 *
 * 1. J1939-71 (MAR2011)
 */
#pragma once

namespace embr { namespace j1939 {

enum class slots
{
    SAEpr11 = 1,
    SAEpr13 = 2,
    SAEtm11 = 3,
    SAEtm10 = 4,
    SAEtm12 = 5,
    SAEtm06 = 6,
    SAEad01 = 7,
    SAEds03 = 13,
    SAEds04 = 14,
    SAEr02 = 31,
    SAEr01 = 32,
    SAEsa01 = 35,           ///< Source Address
    SAEec02 = 40,
    SAEpw03 = 55,
    SAEcd01 = 60,
    SAEtm08 = 61,
    SAEcm01 = 62,
    SAEtm04 = 63,
    SAEtm05 = 64,
    SAEcw01 = 65,
    SAEcy01 = 66,
    SAEtp01 = 67,
    SAEev01 = 80,
    SAEtm07 = 99,
    SAEec01 = 104,
    SAEpw02 = 107,
    SAEpg00 = 116,          ///< PGN
    SAEds02 = 142,
    SAEpw01 = 147,
    SAEnm01 = 159,          ///< NAME
    SAEtm13 = 206,
    SAESP00 = 214,
    SAEtm14 = 224,
    SAEtm15 = 238,
    SAEtm16 = 241,
    SAEtm17 = 247,
    SAEev04 = 249,
    SAEds15 = 270,
    SAEcy02 = 284,
    SAEpw04 = 305,
    SAEtm18 = 319,
};

}}