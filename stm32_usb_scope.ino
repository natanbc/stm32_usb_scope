/*.
  (c) Andrew Hull - 2015
  STM32-O-Scope - aka "The Pig Scope" or pigScope released under the GNU GENERAL PUBLIC LICENSE Version 2, June 1991
  https://github.com/pingumacpenguin/STM32-O-Scope
  Adafruit Libraries released under their specific licenses Copyright (c) 2013 Adafruit Industries.  All rights reserved.
*/

/*
 (c) natanbc - 2019
 Modified the code to work only over serial. 
*/

#include <libmaple/dma.h>
// Defined for power and sleep functions pwr.h and scb.h
#include <libmaple/pwr.h>
#include <libmaple/scb.h>

// SerialCommand -> https://github.com/kroimon/Arduino-SerialCommand.git
#include <SerialCommand.h>

#define TEST_WAVE_PIN       PB1     //PB1 PWM 1000 Hz 

// LED - blinks on trigger events - leave this undefined if your board has no controllable LED
// define as PC13 on the "Red/Blue Pill" boards and PD2 on the "Yellow Pill R"
#define BOARD_LED PC13

// Analog input
#define ANALOG_MAX_VALUE 4096

#define CLOCK_MHZ 72000000

// Samples - depends on available RAM 6K is about the limit on an STM32F103C8T6
#define MAX_SAMPLES 1024*6

const int8_t analogInPin = PB0;   // Analog input pin: any of LQFP44 pins (PORT_PIN), 10 (PA0), 11 (PA1), 12 (PA2), 13 (PA3), 14 (PA4), 15 (PA5), 16 (PA6), 17 (PA7), 18 (PB0), 19  (PB1)

// Trigger default position (half of full scale)
const int32_t triggerValue = 2048;

const int16_t retriggerDelay = 0;

// Startup with sweep hold off or on
boolean triggerHeld = 0;

//Trigger stuff
boolean notTriggered;

int8_t triggerType = 0; //0-both 1-negative 2-positive

//Array for trigger points
uint16_t triggerPoints[2];

uint64_t lastUptime = 0;
uint64_t uptimeMicros = 0;
uint32_t lastMicros = 0;

SerialCommand sCmd;

USBSerial serial_out;

// Array for the ADC data
uint32_t dataPoints32[MAX_SAMPLES / 2];
uint16_t *dataPoints = (uint16_t *)&dataPoints32;


// End of DMA indication
volatile static bool dma1_ch1_Active;
#define ADC_CR1_FASTINT 0x70000 // Fast interleave mode DUAL MODE bits 19-16



void setup() {
  serial_out.begin(115200);
  // BOARD_LED blinks on triggering assuming you have an LED on your board. If not simply dont't define it at the start of the sketch.
#if defined BOARD_LED
  pinMode(BOARD_LED, OUTPUT);
  for (int i = 0; i < 2; i++) {
    digitalWrite(BOARD_LED, HIGH);
    delay(500);
    digitalWrite(BOARD_LED, LOW);
    delay(500);
  }
#else
  delay(2000);
#endif
  serial_out.println("#Time(uS), value, ADC Number, diff");

  adc_calibrate(ADC1);
  adc_calibrate(ADC2);
  setADCs(); //Setup ADC peripherals for interleaved continuous mode.

  //
  // Serial command setup
  // Setup callbacks for SerialCommand commands
  sCmd.addCommand("sleep",       sleepMode);               // Experimental - puts system to sleep

  sCmd.addCommand("h",   toggleHold);                      // Turns triggering on/off
  sCmd.addCommand("P",   toggleTestPulseOn);               // Toggle the test pulse pin from high impedence input to square wave output.
  sCmd.addCommand("p",   toggleTestPulseOff);              // Toggle the Test pin from square wave test to high impedence input.

  sCmd.setDefaultHandler(unrecognized);                    // Handler for command that isn't matched  (says "Unknown")
  sCmd.clearBuffer();


  // The test pulse is a square wave of approx 3.3V (i.e. the STM32 supply voltage) at approx 1 kHz
  // "The Arduino has a fixed PWM frequency of 490Hz" - and it appears that this is also true of the STM32F103 using the current STM32F03 libraries as per
  // STM32, Maple and Maple mini port to IDE 1.5.x - http://forum.arduino.cc/index.php?topic=265904.2520
  // therefore if we want a precise test frequency we can't just use the default uncooked 50% duty cycle PWM output.
  timer_set_period(Timer3, 1000);
  toggleTestPulseOn();

  // Set up our sensor pin(s)
  pinMode(analogInPin, INPUT_ANALOG);

  lastMicros = micros();
}

void loop() {
  sCmd.readSerial();
  if(!triggerHeld) {
    trigger();
    if(!notTriggered) {
      blinkLED();
      takeSamples();
      uint32_t m = micros();
      lastUptime = uptimeMicros;
      uptimeMicros += (m - lastMicros);
      lastMicros = m;
      serialSamples();
    }
  }
  delay(retriggerDelay);
}

void setADCs() {
  //  const adc_dev *dev = PIN_MAP[analogInPin].adc_device;
  int pinMapADCin = PIN_MAP[analogInPin].adc_channel;
  adc_set_sample_rate(ADC1, ADC_SMPR_1_5); //=0,58uS/sample.  ADC_SMPR_13_5 = 1.08uS - use this one if Rin>10Kohm,
  adc_set_sample_rate(ADC2, ADC_SMPR_1_5);    // if not may get some sporadic noise. see datasheet.

  //  adc_reg_map *regs = dev->regs;
  adc_set_reg_seqlen(ADC1, 1);
  ADC1->regs->SQR3 = pinMapADCin;
  ADC1->regs->CR2 |= ADC_CR2_CONT; // | ADC_CR2_DMA; // Set continuous mode and DMA
  ADC1->regs->CR1 |= ADC_CR1_FASTINT; // Interleaved mode
  ADC1->regs->CR2 |= ADC_CR2_SWSTART;

  ADC2->regs->CR2 |= ADC_CR2_CONT; // ADC 2 continuos
  ADC2->regs->SQR3 = pinMapADCin;
}


// Crude triggering on positive or negative or either change from previous to current sample.
void trigger() {
  notTriggered = true;
  switch (triggerType) {
    case 1:
      triggerNegative();
      break;
    case 2:
      triggerPositive();
      break;
    default:
      triggerBoth();
      break;
  }
}

void triggerBoth() {
  //apparently it works better if it's always triggered
  /*triggerPoints[0] = analogRead(analogInPin);
  while(notTriggered) {
    triggerPoints[1] = analogRead(analogInPin);
    if( ((triggerPoints[1] < triggerValue) && (triggerPoints[0] > triggerValue)) ||
        ((triggerPoints[1] > triggerValue) && (triggerPoints[0] < triggerValue)) ) {
      notTriggered = false;
    }
    triggerPoints[0] = triggerPoints[1];
  }*/
  notTriggered = false;
}

void triggerPositive() {
  triggerPoints[0] = analogRead(analogInPin);
  while(notTriggered) {
    triggerPoints[1] = analogRead(analogInPin);
    if((triggerPoints[1] > triggerValue) && (triggerPoints[0] < triggerValue) ) {
      notTriggered = false;
    }
    triggerPoints[0] = triggerPoints[1];
  }
}

void triggerNegative() {
  triggerPoints[0] = analogRead(analogInPin);
  while(notTriggered) {
    triggerPoints[1] = analogRead(analogInPin);
    if((triggerPoints[1] < triggerValue) && (triggerPoints[0] > triggerValue) ) {
      notTriggered = false;
    }
    triggerPoints[0] = triggerPoints[1];
  }
}

void incEdgeType() {
  triggerType += 1;
  if(triggerType > 2) {
    triggerType = 0;
  }
  serial_out.print("# Trigger type set to ");
  switch(triggerType) {
    case 1:  serial_out.println("negative"); break;
    case 2:  serial_out.println("positive"); break;
    default: serial_out.println("negative/positive"); break;
  }
}

void blinkLED() {
#if defined BOARD_LED
  digitalWrite(BOARD_LED, LOW);
  delay(10);
  digitalWrite(BOARD_LED, HIGH);
#endif

}

// Grab the samples from the ADC
// Theoretically the ADC can not go any faster than this.
//
// According to specs, when using 72Mhz on the MCU main clock,the fastest ADC capture time is 1.17 uS. As we use 2 ADCs we get double the captures,
// so .58 uS, which is the times we get with ADC_SMPR_1_5.
// I think we have reached the speed limit of the chip, now all we can do is improve accuracy.
// See; http://stm32duino.com/viewtopic.php?f=19&t=107&p=1202#p1194

void takeSamples() {
  // This loop uses dual interleaved mode to get the best performance out of the ADCs
  //

  dma_init(DMA1);
  dma_attach_interrupt(DMA1, DMA_CH1, DMA1_CH1_Event);

  adc_dma_enable(ADC1);
  dma_setup_transfer(DMA1, DMA_CH1, &ADC1->regs->DR, DMA_SIZE_32BITS,
                     dataPoints32, DMA_SIZE_32BITS, (DMA_MINC_MODE | DMA_TRNS_CMPLT));// Receive buffer DMA
  dma_set_num_transfers(DMA1, DMA_CH1, MAX_SAMPLES / 2);
  dma1_ch1_Active = 1;
  //  regs->CR2 |= ADC_CR2_SWSTART; //moved to setADC
  dma_enable(DMA1, DMA_CH1); // Enable the channel and start the transfer.
  //adc_calibrate(ADC1);
  //adc_calibrate(ADC2);
  while (dma1_ch1_Active);

  dma_disable(DMA1, DMA_CH1); //End of trasfer, disable DMA and Continuous mode.
  // regs->CR2 &= ~ADC_CR2_CONT;

}

void serialSamples() {
  uint64_t uptimeDiff = uptimeMicros - lastUptime;
  serial_out.print("# Last uptime = ");
  serial_out.print(lastUptime);
  serial_out.print(", uptime diff = ");
  serial_out.println(uptimeDiff);
  // Send *all* of the samples to the serial port.
  for(int16_t j = 1; j < MAX_SAMPLES; j++ ) {
    // Time from trigger in microseconds
    serial_out.print(lastUptime + (uptimeDiff / (MAX_SAMPLES))*j);
    serial_out.print(" ");
    // raw ADC data
    serial_out.print(dataPoints[j]);
    serial_out.print(" ");
    serial_out.print(j % 2 + 1);
    serial_out.print(" ");
    serial_out.print(dataPoints[j] - dataPoints[j - 1]);
    serial_out.print(" ");
    serial_out.print(dataPoints[j] - ((dataPoints[j] - dataPoints[j - 1]) / 2));
    serial_out.print("\n");
  }
  serial_out.print("\n");
}

void toggleHold() {
  triggerHeld = !triggerHeld;
  if(triggerHeld) {
    serial_out.println("# Toggle Hold on");
  } else {
    serial_out.println("# Toggle Hold off");
  }
}

void unrecognized(const char *command) {
  serial_out.print("# Unknown Command.[");
  serial_out.print(command);
  serial_out.println("]");
}

void toggleTestPulseOn() {
  pinMode(TEST_WAVE_PIN, OUTPUT);
  analogWrite(TEST_WAVE_PIN, 75);
}

void toggleTestPulseOff() {
  pinMode(TEST_WAVE_PIN, INPUT);
  serial_out.println("# Test Pulse Off.");
}

uint16 timer_set_period(HardwareTimer timer, uint32 microseconds) {
  if (!microseconds) {
    timer.setPrescaleFactor(1);
    timer.setOverflow(1);
    return timer.getOverflow();
  }

  uint32 cycles = microseconds * (CLOCK_MHZ / 1000000); // 72 cycles per microsecond

  uint16 ps = (uint16)((cycles >> 16) + 1);
  timer.setPrescaleFactor(ps);
  timer.setOverflow((cycles / ps) - 1 );
  return timer.getOverflow();
}

void adc_dma_enable(const adc_dev * dev) {
  bb_peri_set_bit(&dev->regs->CR2, ADC_CR2_DMA_BIT, 1);
}

void adc_dma_disable(const adc_dev * dev) {
  bb_peri_set_bit(&dev->regs->CR2, ADC_CR2_DMA_BIT, 0);
}

static void DMA1_CH1_Event() {
  dma1_ch1_Active = 0;
}

void sleepMode() {
  serial_out.println("# Nighty night!");
  // Set PDDS and LPDS bits for standby mode, and set Clear WUF flag (required per datasheet):
  PWR_BASE->CR |= PWR_CR_CWUF;
  PWR_BASE->CR |= PWR_CR_PDDS;

  // set sleepdeep in the system control register
  SCB_BASE->SCR |= SCB_SCR_SLEEPDEEP;

  // Now go into stop mode, wake up on interrupt
  // disableClocks();
  asm("wfi");
}
