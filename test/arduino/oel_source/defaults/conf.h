#pragma once

#include "../../template/conf.h"

#ifndef CONFIG_GPIO_LEFT_BLINKER_SWITCH
#define CONFIG_GPIO_LEFT_BLINKER_SWITCH 0
#endif

#ifndef CONFIG_GPIO_RIGHT_BLINKER_SWITCH
#define CONFIG_GPIO_RIGHT_BLINKER_SWITCH 0
#endif

// DEBT: This is just on/off so parking light is not considered 
#ifndef CONFIG_GPIO_MAIN_LIGHT_SWITCH
#define CONFIG_GPIO_MAIN_LIGHT_SWITCH 0
#endif

#ifndef CONFIG_GPIO_HAZARD_SWITCH
#define CONFIG_GPIO_HAZARD_SWITCH 0
#endif

// Local dashboard indicator for left blinker
#ifndef CONFIG_GPIO_LEFT_BLINKDER_INDICATOR
#define CONFIG_GPIO_LEFT_BLINKDER_INDICATOR
#endif

// Local dashboard indicator for right blinker
#ifndef CONFIG_GPIO_RIGHT_BLINKDER_INDICATOR
#define CONFIG_GPIO_RIGHT_BLINKDER_INDICATOR
#endif

#define CONFIG_LEFT_BLINKER_GPIO_INVERTED 0
#define CONFIG_RIGHT_BLINKER_GPIO_INVERTED 0
