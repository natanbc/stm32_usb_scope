#if !defined(INPUT_CHANNEL)
    #error "ADC input channel not defined"
#endif
#if !defined(INPUT_SAMPLE_RATE)
    #error "ADC sample rate not defined"
#endif

/*
each branch needs to define:

INPUT_PIN -> line
INPUT_SMPR2 -> int

SMPR 1 is only needed for channels > 9, which aren't available
in this board.

SQR 1, 2 and 3 have the same definition regardless of input
because it only changes depending on the number of channels.
Since we only support a single channel, there's only one possible
definition of it.
*/

#if INPUT_CHANNEL == ADC_CHANNEL_IN9
    #define INPUT_PIN           PAL_LINE(GPIOB, 1)
    #define INPUT_SMPR2         ADC_SMPR2_SMP_AN9(INPUT_SAMPLE_RATE)
#elif INPUT_CHANNEL == ADC_CHANNEL_IN8
    #define INPUT_PIN           PAL_LINE(GPIOB, 0)
    #define INPUT_SMPR2         ADC_SMPR2_SMP_AN8(INPUT_SAMPLE_RATE)
#elif INPUT_CHANNEL == ADC_CHANNEL_IN7
    #define INPUT_PIN           PAL_LINE(GPIOA, 7)
    #define INPUT_SMPR2         ADC_SMPR2_SMP_AN7(INPUT_SAMPLE_RATE)
#elif INPUT_CHANNEL == ADC_CHANNEL_IN6
    #define INPUT_PIN           PAL_LINE(GPIOA, 6)
    #define INPUT_SMPR2         ADC_SMPR2_SMP_AN6(INPUT_SAMPLE_RATE)
#elif INPUT_CHANNEL == ADC_CHANNEL_IN5
    #define INPUT_PIN           PAL_LINE(GPIOA, 5)
    #define INPUT_SMPR2         ADC_SMPR2_SMP_AN5(INPUT_SAMPLE_RATE)
#elif INPUT_CHANNEL == ADC_CHANNEL_IN4
    #define INPUT_PIN           PAL_LINE(GPIOA, 4)
    #define INPUT_SMPR2         ADC_SMPR2_SMP_AN4(INPUT_SAMPLE_RATE)
#elif INPUT_CHANNEL == ADC_CHANNEL_IN3
    #define INPUT_PIN           PAL_LINE(GPIOA, 3)
    #define INPUT_SMPR2         ADC_SMPR2_SMP_AN3(INPUT_SAMPLE_RATE)
#elif INPUT_CHANNEL == ADC_CHANNEL_IN2
    #define INPUT_PIN           PAL_LINE(GPIOA, 2)
    #define INPUT_SMPR2         ADC_SMPR2_SMP_AN2(INPUT_SAMPLE_RATE)
#elif INPUT_CHANNEL == ADC_CHANNEL_IN1
    #define INPUT_PIN           PAL_LINE(GPIOA, 1)
    #define INPUT_SMPR2         ADC_SMPR2_SMP_AN1(INPUT_SAMPLE_RATE)
#elif INPUT_CHANNEL == ADC_CHANNEL_IN0
    #define INPUT_PIN           PAL_LINE(GPIOA, 0)
    #define INPUT_SMPR2         ADC_SMPR2_SMP_AN0(INPUT_SAMPLE_RATE)
#else
    #error "Unsupported ADC channel " #INPUT_CHANNEL
#endif

#define INPUT_SMPR1             0
#define INPUT_SQR1              0
#define INPUT_SQR2              0
#define INPUT_SQR3              ADC_SQR3_SQ1_N(INPUT_CHANNEL)
