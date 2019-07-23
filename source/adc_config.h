#if !defined(INPUT_CHANNEL)
    #error "ADC input channel not defined"
#endif
#if !defined(INPUT_SAMPLE_RATE)
    #error "ADC sample rate not defined"
#endif

/*
each branch needs to define:

INPUT_PIN -> line

INPUT_SMPR1, INPUT_SMPR2,
INPUT_SQR1, INPUT_SQR2
-> ints

INPUT_SQR3 has the same definition regardless of input
because it only changes depending on the number of channels.
Since we only support a single channel, there's only one possible
definition of it.
*/

//TODO: other inputs

#if INPUT_CHANNEL == ADC_CHANNEL_IN8
    #define INPUT_PIN           PAL_LINE(GPIOB, 0)
    #define INPUT_SMPR1         0
    #define INPUT_SMPR2         ADC_SMPR2_SMP_AN8(INPUT_SAMPLE_RATE)
    #define INPUT_SQR1          0
    #define INPUT_SQR2          0
#else
    #error "Unsupported ADC channel " #INPUT_CHANNEL
#endif

#define INPUT_SQR3              ADC_SQR3_SQ1_N(INPUT_CHANNEL)
