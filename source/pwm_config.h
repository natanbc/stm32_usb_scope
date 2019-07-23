#include "macro_helpers.h"

#if !defined(TEST_WAVE_TIMER)
    #error "PWM timer not defined"
#endif
#if !defined(TEST_WAVE_CHANNEL)
    #error "PWM channel not defined"
#endif

#define TEST_WAVE_PWM_OBJ PPCAT(PWMD, TEST_WAVE_TIMER)


#if TEST_WAVE_TIMER == 1
    #if TEST_WAVE_CHANNEL == 1
        #define TEST_WAVE_PIN PAL_LINE(GPIOA, 8)
    #elif TEST_WAVE_CHANNEL == 2
        #define TEST_WAVE_PIN PAL_LINE(GPIOA, 9)
    #elif TEST_WAVE_CHANNEL == 3
        #define TEST_WAVE_PIN PAL_LINE(GPIOA, 10)
    #elif TEST_WAVE_CHANNEL == 4
        #define TEST_WAVE_PIN PAL_LINE(GPIOA, 11)
    #else
        #error "Invalid PWM channel"
    #endif
#elif TEST_WAVE_TIMER == 2
    #if TEST_WAVE_CHANNEL == 1
        #define TEST_WAVE_PIN PAL_LINE(GPIOA, 0)
    #elif TEST_WAVE_CHANNEL == 2
        #define TEST_WAVE_PIN PAL_LINE(GPIOA, 1)
    #elif TEST_WAVE_CHANNEL == 3
        #define TEST_WAVE_PIN PAL_LINE(GPIOA, 2)
    #elif TEST_WAVE_CHANNEL == 4
        #define TEST_WAVE_PIN PAL_LINE(GPIOA, 3)
    #else
        #error "Invalid PWM channel"
    #endif
#elif TEST_WAVE_TIMER == 3
    #if TEST_WAVE_CHANNEL == 1
        #define TEST_WAVE_PIN PAL_LINE(GPIOA, 6)
    #elif TEST_WAVE_CHANNEL == 2
        #define TEST_WAVE_PIN PAL_LINE(GPIOA, 7)
    #elif TEST_WAVE_CHANNEL == 3
        #define TEST_WAVE_PIN PAL_LINE(GPIOB, 0)
    #elif TEST_WAVE_CHANNEL == 4
        #define TEST_WAVE_PIN PAL_LINE(GPIOB, 1)
    #else
        #error "Invalid PWM channel"
    #endif
#elif TEST_WAVE_TIMER == 4
    #if TEST_WAVE_CHANNEL == 1
        #define TEST_WAVE_PIN PAL_LINE(GPIOB, 6)
    #elif TEST_WAVE_CHANNEL == 2
        #define TEST_WAVE_PIN PAL_LINE(GPIOB, 7)
    #elif TEST_WAVE_CHANNEL == 3
        #define TEST_WAVE_PIN PAL_LINE(GPIOB, 8)
    #elif TEST_WAVE_CHANNEL == 4
        #define TEST_WAVE_PIN PAL_LINE(GPIOB, 9)
    #else
        #error "Invalid PWM channel"
    #endif
#else
    #error "Invalid PWM timer"
#endif
