# STM32 USB scope

Cheap oscilloscope which dumps samples to serial for plotting on a computer.

This software is designed and tested for an STM32F103C6 (Blue Pill) microcontroller.


A work-in-progress program to visualize the samples with better resolution, framerate and tooling
is being developed by [DarkRyu550](https://github.com/DarkRyu550). It's available on the `xosc` folder.

# Needed software

- A C compiler for your machine's architecture (linux comes with the appropriate gcc by default)
- A C compiler that targets `arm-none-eabi`
- [picocom](https://github.com/npat-efault/picocom)
- [ttyplot](https://github.com/tenox7/ttyplot)

# Usage

The provided software is designed to work with bash on linux systems. It might work on other systems
if you change the serial communication program to an equivalent. The C code should be cross platform.

The behaviour of the software can be configured by editing `config.h`.

- Compile the mean.c file (`cd helpers; gcc -O3 mean.c -o mean_c`)
- Install Chibi OS
    * If the path it's installed to isn't `/usr/src/chibios`, edit the makefile and replace the `CHIBIOS  := /usr/src/chibios` with the right path
- Compile the microcontroller code by running `make`
- Flash it via st-link with `make flash`
- Connect to the micro usb port on the board/the serial output configured
- Run `./scope.sh [samples per plot bar]`. You may need to edit it to use the location the microcontroller
  appears in /dev.

# Output format

Data is sent as chunks starting with either `#` or `>` and terminated by a newline (`\n`). Chunks that
start with `#` are comments and should be ignored. Chunks that start with '>' are samples and follow the format:

- `uint16_t` with the value 0x1234. This is provided as a way for the receiver to determine the endianness of the sender.
- `uint32_t` with the amount of samples.
- `length` samples represented as `uint16_t`.

# Test the scope

The following schematic shows how to test the scope using the 1kHz square wave it generates on PB1.

![schematic](https://github.com/natanbc/stm32_usb_scope/raw/master/assets/example-schematic.png)

![top view](https://github.com/natanbc/stm32_usb_scope/raw/master/assets/top-view.jpg)

# Video (ignore se você não for o simões)

https://www.youtube.com/watch?v=lJdXwsVDLeo
