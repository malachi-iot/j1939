#pragma once

#ifndef CONFIG_GPIO_BJM1_BUTTON1_SINK
#define CONFIG_GPIO_BJM1_BUTTON1_SINK 13
//#pragma message "GPIO defaulting to 13"
#endif

#define CONFIG_BUTTON1_INITIAL_STATE 0

#ifndef CONFIG_BUTTON1_LATCHING
#define CONFIG_BUTTON1_LATCHING 1
#endif

#define CONFIG_BUTTON1_INVERTED 0
