//CH -> PIN
//0  ->  A0
//1  ->  A1
//...
//6  ->  A6
//7  ->  A7
//8  ->  B0
//9  ->  B1
#define INPUT_CHANNEL            ADC_CHANNEL_IN8
//Valid sample rates:
//1P5    ->    1.5 clock cycles
//7P5    ->    7.5 clock cycles
//13P5   ->   13.5 clock cycles
//28P5   ->   28.5 clock cycles
//41P5   ->   41.5 clock cycles
//55P5   ->   55.5 clock cycles
//71P5   ->   71.5 clock cycles
//239P5  ->  239.5 clock cycles
#define INPUT_SAMPLE_RATE        ADC_SAMPLE_1P5
#define ADC_BUFFER_SIZE          1024

//Use micro USB port to send data. Disables regular
//serial if enabled.
#define SERIAL_USE_USB           TRUE

//DRIVER   TX   RX
//  1      A9   A10
//  2      A2   A3
//  3      B10  B11
//Be sure to enable the selected driver in mcuconf.h
#define SERIAL_DRIVER            1


#define TEST_WAVE_ENABLE         TRUE
//TIMER  CHANNEL  PIN
//  1       1      A8
//  1       2      A9
//  1       3     A10
//  1       4     A11
//  2       1      A0
//  2       2      A1
//  2       3      A2
//  2       4      A3
//  3       1      A6
//  3       2      A7
//  3       3      B0
//  3       4      B1
//  4       1      B6
//  4       2      B7
//  4       3      B8
//  4       4      B9
//Be sure to enable the selected timer in mcuconf.h
#define TEST_WAVE_TIMER          3
#define TEST_WAVE_CHANNEL        4
//frequency of the internal timer. you probably won't need to change this
#define TEST_WAVE_TIM_FREQ       1000000
//frequency of the generated pwm signal
#define TEST_WAVE_PWM_FREQ       200
#define TEST_WAVE_WIDTH          50


#define LED_ENABLE               TRUE
#define LED_PIN                  PAL_LINE(GPIOC, GPIOC_LED)
