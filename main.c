#include <string.h> //memcpy
#include "ch.h"
#include "hal.h"
#include "chprintf.h"

#define INPUT_CHANNEL            ADC_CHANNEL_IN8
#define INPUT_SAMPLE_RATE        ADC_SAMPLE_1P5

#include "adc_config.h"

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

#define NUM_BUFFERS              2
static msg_t buffers_queue[NUM_BUFFERS];
static mailbox_t filled_buffers;

//only one channel for now, might extend in the future
//use a define so no magic constants get forgotten if
//multiple channels are eventually added.
#define ADC_CHANNEL_COUNT        1
#define ADC_BUFFER_SIZE          1024

static const ADCConfig adccfg = {};
//buffer is an interleaved array (ch1, ch2, ..., chN, ch1, ch2, ..., chN, ...)
static adcsample_t sample_buf[ADC_CHANNEL_COUNT * ADC_BUFFER_SIZE];

static void adc_callback(ADCDriver* adcp) {
    adcsample_t* dataStart;
    if(adcIsBufferComplete(adcp)) {
        dataStart = &sample_buf[ADC_BUFFER_SIZE/2];
    } else {
        dataStart = sample_buf;
    }
    chSysLockFromISR();
    chMBPostI(&filled_buffers, (msg_t)dataStart);
    chSysUnlockFromISR();
}

static void adc_errcallback(ADCDriver* adcp, adcerror_t err) {
    (void)adcp;
    (void)err; //only value in the enum is ADC_ERR_DMAFAILURE
    chSysLockFromISR();
    chMBPostI(&filled_buffers, (msg_t)NULL);
    chSysUnlockFromISR();
}

static const ADCConversionGroup adccg = {
    TRUE,                                //buffer is circular, once it's full go back to start.
    ADC_CHANNEL_COUNT,
    adc_callback,                        //called twice per sampling, with half of the buffer.
                                         //this way we can always send half of the data while
                                         //the other half is written to.
    adc_errcallback,                     //error callback
    0, 0,                                //cr1, cr2
    INPUT_SMPR1, INPUT_SMPR2,            //SMPR 1 & 2
    ADC_SQR1_NUM_CH(ADC_CHANNEL_COUNT) |
        INPUT_SQR1,                      //SQR 1 + channel count
    INPUT_SQR2, INPUT_SQR3               //SQR 2 & 3
};

static THD_WORKING_AREA(waDataSender, 128);
static THD_FUNCTION(DataSenderThread, arg) {
    (void)arg;
    bool restartSampling = false;
    while(true) {
        adcsample_t* pbuf;
        chprintf((BaseSequentialStream*)&SERIAL,"# Fetching buffer...\n");
        chMBFetchTimeout(&filled_buffers, (msg_t*)&pbuf, TIME_INFINITE);
        if(pbuf == NULL) {
            chprintf((BaseSequentialStream*)&SERIAL, "# ERROR\n");
            chSysHalt("Error reading data from ADC");
            break;
        }
        chprintf((BaseSequentialStream*)&SERIAL, "# Got ADC response\n");
        for(int i = 0; i < ADC_BUFFER_SIZE/2; i++) {
            chprintf((BaseSequentialStream*)&SERIAL, "%d\n", pbuf[i]);
        }
        if(restartSampling) {
            restartSampling = false;
            adcStartConversion(&ADCD1, &adccg, &sample_buf[0], ADC_BUFFER_SIZE);
        }
        restartSampling = true;
    }
}

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

    chMBObjectInit(&filled_buffers, buffers_queue, NUM_BUFFERS);
    
    palSetLineMode(SERIAL_TX_PIN,    PAL_MODE_STM32_ALTERNATE_OPENDRAIN);
    palSetLineMode(SERIAL_RX_PIN,    PAL_MODE_INPUT);
    sdStart(&SERIAL, NULL);

    palSetLineMode(INPUT_PIN,        PAL_MODE_INPUT_ANALOG);
    adcObjectInit(&ADCD1);
    adcStart(&ADCD1, &adccfg);

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
    //based on the example in section 2.3:
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
#if STM32_PWM_USE_ADVANCED
    pwmcfg.bdtr = 0;
#endif
    pwmcfg.dier = 0;


    pwmStart(&TEST_WAVE_PWM,         &pwmcfg);

    //channels are 1-based but this function expects them as 0-based
    pwmEnableChannel(&TEST_WAVE_PWM, TEST_WAVE_CHANNEL - 1, TEST_WAVE_WIDTH);
#endif

#if LED_ENABLE
    palSetLineMode(LED_PIN,          PAL_MODE_OUTPUT_OPENDRAIN);
#endif

    chThdCreateStatic(waDataSender, sizeof(waDataSender), NORMALPRIO + 1, DataSenderThread, NULL);
    adcStartConversion(&ADCD1, &adccg, &sample_buf[0], ADC_BUFFER_SIZE);
    while (true) {
#if LED_ENABLE
        palToggleLine(LED_PIN);
        chThdSleepMilliseconds(50);
        palToggleLine(LED_PIN);
        chThdSleepMilliseconds(50);
#endif
    }
}
