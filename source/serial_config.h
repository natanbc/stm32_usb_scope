#include "macro_helpers.h"

#if !defined(SERIAL_DRIVER) && !SERIAL_USE_USB
    #error "Serial driver not defined"
#endif

#if SERIAL_USE_USB
    #define SERIAL SDU1
    #define SERIAL_WRITE_CHAR(_S,_C)    (chnPutTimeout(_S,_C,TIME_INFINITE))
    #define SERIAL_WRITE_DATA(_S,_D,_L) (chnWrite(_S,_D,_L))

    void serial_start(void) {
        sduObjectInit(&SDU1);
        sduStart(&SDU1, &serusbcfg);

        usbDisconnectBus(serusbcfg.usbp);
        chThdSleepMilliseconds(1000);
        usbStart(serusbcfg.usbp, &usbcfg);
        usbConnectBus(serusbcfg.usbp);
    }

    void serial_wait_until_ready(void) {
        while(SDU1.config->usbp->state != USB_ACTIVE) {
            chThdSleepMilliseconds(50);
        }
    }
#else
    #define SERIAL PPCAT(SD, SERIAL_DRIVER)
    #define SERIAL_WRITE_CHAR(_S,_C)    (sdPut(_S,_C))
    #define SERIAL_WRITE_DATA(_S,_D,_L) (sdWrite(_S,_D,_L))

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

    void serial_start(void) {
        palSetLineMode(SERIAL_TX_PIN,    PAL_MODE_STM32_ALTERNATE_OPENDRAIN);
        palSetLineMode(SERIAL_RX_PIN,    PAL_MODE_INPUT);
        sdStart(&SERIAL, NULL);
    }

    void serial_wait_until_ready(void) {}
#endif
