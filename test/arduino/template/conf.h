#pragma once

#ifndef CONFIG_SERIAL
#ifdef __AVR_ATtiny85__
#define CONFIG_SERIAL 0
#else
#define CONFIG_SERIAL 1
#endif
#endif

#ifndef CONFIG_DIAGNOSTIC_CA
#ifdef __AVR_ATtiny85__
#define CONFIG_DIAGNOSTIC_CA CONFIG_SERIAL
#else
#define CONFIG_DIAGNOSTIC_CA 1
#endif
#endif

#ifndef CONFIG_SERIAL_WAIT
#define CONFIG_SERIAL_WAIT 1
#endif
