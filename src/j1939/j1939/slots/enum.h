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
    SAEad06 = 11,           ///< Angle/Direction
    SAEct04 = 12,           ///< Count
    SAEds03 = 13,
    SAEds04 = 14,
    SAEpc01 = 30,
    SAEr02 = 31,
    SAEr01 = 32,
    SAEsa01 = 35,           ///< Source Address
    SAEec02 = 40,
    SAEec03 = 41,           ///< 0-250 A
    SAEpc03 = 44,           ///< Percent
    SAEpc05 = 45,
    SAEpc06 = 46,
    SAEpw03 = 55,
    SAEcd01 = 60,
    SAEtm08 = 61,
    SAEcm01 = 62,
    SAEtm04 = 63,
    SAEtm05 = 64,
    SAEcw01 = 65,
    SAEcy01 = 66,
    SAEtp01 = 67,           ///< -40 to 210 deg C
    SAEtp02 = 68,
    SAEtq01 = 69,
    SAEev01 = 80,
    SAEpr03 = 85,
    SAEtm07 = 99,
    SAEec01 = 104,
    SAEpw02 = 107,
    SAEatad0200 = 108,      ///< ASCII up to 200 bytes, '*' delimited
    SAEatad0005 = 109,      ///< ASCII up to 5 bytes, '*' delimited
    SAEatad1728 = 110,      ///< ASCII up to 1728 bytes, '*' delimited
    SAEpg00 = 116,          ///< PGN
    SAEpc04 = 128,
    SAEds02 = 142,
    SAEec06 = 143,          ///< 0-64.255A
    SAEev02 = 145,
    SAEec04 = 146,          ///< Electrical Current
    SAEpw01 = 147,
    SAEpa01 = 148,          ///< Apparent Power
    SAEnm01 = 159,          ///< NAME
    SAEpc02 = 205,          ///< Percent
    SAEtm13 = 206,
    SAEca01 = 211,          ///< Battery Capacity (1 mAhr/bit)
    SAESP00 = 214,          ///< SPN
    SAEOC00 = 216,
    SAEev03 = 221,
    SAEvm02 = 222,          ///< Volume
    SAEtm14 = 224,
    SAEct09 = 230,          ///< Count
    SAEtm15 = 238,
    SAEtm16 = 241,
    SAEtm17 = 247,
    SAEev04 = 249,
    SAEpc07 = 251,
    SAEpc08 = 256,
    SAEpc10 = 268,
    SAEds15 = 270,
    SAEatad0025 = 271,      ///< ASCII up to 25 bytes, '*' delimited
    SAEtc01 = 274,          ///< Trouble Code
    SAEmd01 = 283,          ///< Manufacturer Defined
    SAEcy02 = 284,
    SAEatad0032 = 291,      ///< ASCII up to 32 bytes, '*' delimited
    SAEsg02 = 298,          ///< Signal Gain
    SAEpw04 = 305,
    SAEtm18 = 319,
};

}}