#include "macro_helpers.h"

#if !defined(SERIAL_DRIVER)
    #error "Serial driver not defined"
#endif

#define SERIAL PPCAT(SD, SERIAL_DRIVER)

#if SERIAL_DRIVER == 1
    #define SERIAL_TX_PIN           PAL_LINE(GPIOA, 9)
    #define SERIAL_RX_PIN           PAL_LINE(GPIOA, 10)
#elif SERIAL_DRIVER == 2
    #define SERIAL_TX_PIN           PAL_LINE(GPIOA, 2)
    #define SERIAL_RX_PIN           PAL_LINE(GPIOA, 3)
#elif SERIAL_DRIVER == 3
    #define SERIAL_TX_PIN           PAL_LINE(GPIOB, 10)
    #define SERIAL_RX_PIN           PAL_LINE(GPIOB, 11)
#else
    #error "Invalid serial driver"
#endif
