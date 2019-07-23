#include <string.h> //memcpy
#include "ch.h"
#include "hal.h"

//PB0 is ADC CH8
#define INPUT_ADC_CH             8
#define INPUT_PIN                PAL_LINE(GPIOB, 0)

//if you want to change this, you also need to change cfg/mcuconf.h to use
//the appropriate USART (by default 1 is enabled and the others disabled)
#define SERIAL                   SD1
#define SERIAL_TX_PIN            PAL_LINE(GPIOA, 9)
#define SERIAL_RX_PIN            PAL_LINE(GPIOA, 10)


#define TEST_WAVE_ENABLE         TRUE
//PB1 is TIM3 CH4
#define TEST_WAVE_PWM            PWMD3
#define TEST_WAVE_CHANNEL        4
#define TEST_WAVE_PIN            PAL_LINE(GPIOB, 1)
//frequency of the internal timer. you probably won't need to change this
#define TEST_WAVE_TIM_FREQ       1000000
//frequency of the generated pwm signal
#define TEST_WAVE_PWM_FREQ       200
//50% duty cycle
#define TEST_WAVE_WIDTH          PWM_PERCENTAGE_TO_WIDTH(&TEST_WAVE_PWM, 5000)


#define LED_ENABLE               TRUE
#define LED_PIN                  PAL_LINE(GPIOC, GPIOC_LED)

int main(void) {
    /*
     * System initializations.
     * - HAL initialization, this also initializes the configured device drivers
     *   and performs the board-specific initializations.
     * - Kernel initialization, the main() function becomes a thread and the
     *   RTOS is active.
     */
    halInit();
    chSysInit();

    
    palSetLineMode(SERIAL_TX_PIN,    PAL_MODE_STM32_ALTERNATE_OPENDRAIN);
    palSetLineMode(SERIAL_RX_PIN,    PAL_MODE_INPUT);
    sdStart(&SERIAL, NULL);

#if TEST_WAVE_ENABLE
    palSetLineMode(TEST_WAVE_PIN,    PAL_MODE_STM32_ALTERNATE_PUSHPULL);
    PWMChannelConfig channels[PWM_CHANNELS];
    for(int i = 0; i < PWM_CHANNELS; i++) {
        channels[i].mode = PWM_OUTPUT_DISABLED;
        channels[i].callback = NULL;
    }
    //channels are 1-based but this is an array
    channels[TEST_WAVE_CHANNEL - 1].mode = PWM_OUTPUT_ACTIVE_HIGH;
    
    PWMConfig pwmcfg;
    //This is slightly complicated, so check https://www.playembedded.org/blog/stm32-tim-chibios-pwm
    //if you're lost.
    //
    //based on the example in secion 2.3:
    //
    //frequency = timer frequency
    //period = TIMx_ARR
    //TEST_WAVE_WIDTH would set TIMx_CCR1
    //
    //
    //period is set based on the relation
    //period = fTIM/fPWM
    pwmcfg.frequency = TEST_WAVE_TIM_FREQ;
    pwmcfg.period = TEST_WAVE_TIM_FREQ / TEST_WAVE_PWM_FREQ;
    pwmcfg.callback = NULL;
    memcpy(pwmcfg.channels, channels, sizeof(channels));
    pwmcfg.cr2 = 0;
    pwmcfg.dier = 0;


    pwmStart(&TEST_WAVE_PWM,         &pwmcfg);

    //channels are 1-based but this function expects them as 0-based
    pwmEnableChannel(&TEST_WAVE_PWM, TEST_WAVE_CHANNEL - 1, TEST_WAVE_WIDTH);
#endif

#if LED_ENABLE
    palSetLineMode(LED_PIN,          PAL_MODE_OUTPUT_OPENDRAIN);
#endif

    
    while (true) {
        sdWrite(&SERIAL, (uint8_t*)"Test\r\n", 6);
#if LED_ENABLE
        palToggleLine(LED_PIN);
        chThdSleepMilliseconds(50);
        palToggleLine(LED_PIN);
        chThdSleepMilliseconds(50);
#endif
    }
}
