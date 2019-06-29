# STM32 USB scope

Cheap oscilloscope which dumps samples to serial for plotting on a computer.

This software is designed and tested for an STM32F103C6 (Blue Pill) microcontroller. It may or
may not work on other stm32 variations.

A work-in-progress program to visualize the samples with better resolution, framerate and tooling
is being developed by [DarkRyu550](https://github.com/DarkRyu550). It's available on the `xosc` folder.

# Needed software

- A C compiler
- [Arduino IDE](https://www.arduino.cc/en/Main/Software)
- [picocom](https://github.com/npat-efault/picocom)
- [ttyplot](https://github.com/tenox7/ttyplot)

# Usage

The provided software is designed to work with bash on linux systems. It might work on other systems
if you change the serial communication program to an equivalent. The C code should be cross platform.

- Compile the mean.c file (`gcc -O3 mean.c -o mean_c`)


- (optional) Burn the stm32duino bootloader
    * https://wiki.stm32duino.com/index.php?title=Burning_the_bootloader
- Write the stm32_usb_scope.ino file on the microcontroller
- Connect it to your computer
- Run `./scope.sh [samples per plot bar]`. You may need to edit it to use the location the microcontroller
  appears in /dev.

# Output format

Each sample is printed as a single line, with each column representing one of the following values, in order

- Timestamp (in microseconds)
- ADC value
- ADC number (1 or 2)
- Difference to last sample
- Mean of this sample and the last

# Test the scope

The following schematic shows how to test the scope using the 1kHz square wave it generates on PB1.

![schematic](https://github.com/natanbc/stm32_usb_scope/raw/master/assets/example-schematic.png)

![top view](https://github.com/natanbc/stm32_usb_scope/raw/master/assets/top-view.jpg)

# Video (ignore se você não for o simões)

https://www.youtube.com/watch?v=lJdXwsVDLeo
