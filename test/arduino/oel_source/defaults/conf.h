#pragma once

#ifndef CONFIG_SERIAL_WAIT
#define CONFIG_SERIAL_WAIT 1
#endif

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

#define CONFIG_LEFT_BLINKER_GPIO_INVERTED 0
#define CONFIG_RIGHT_BLINKER_GPIO_INVERTED 0