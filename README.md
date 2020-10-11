# attiny playground

This is a attiny example repo. Every project is represented in a single file. The build tool is a single bash script. Following tools are required.

- `avr-gcc`: To compile our source code.
- `avrdude`: To flash the compiled binary to the chip.

## Hardware setup

Due to personal limitations, I used an Arduino Nano as an ISP Programmer with the provided ArduinoISP example source code of the Arduino IDE. I wired up the following ports (attiny pins are prefixes with `TINY_`):

- `GND` and `RST` with an 100nF capacitor.
- `3.3V` to `TINY_VCC`
- `D10` to `TINY_PB5` (`TINY_RESET`)
- `D11` to `TINY_MOSI`
- `D12` to `TINY_MISO`
- `D13` to `TINY_SCK`

## Build tool

There is a provided build tool in `build.sh` writte by myself with the following commands:

```
This is a attiny project environment.

Command format: './build.sh <COMMAND> <FILE>'

Prepend '<SETTING>=<VALUE>' to the command to overwrite the default settings.
- MMCU: Overwrite the selected chip. Default: attiny25
- PORT: Overwrite the flash port. Default: /dev/ttyUSB0
- FCPU: Overwrite the chip frequency. Default: 1000000
- BAUD: Overwrite the upload speed. Default: 19200

The following commands exist:
- 'c' or 'compile': Compile the src/<FILE>.c into build/*.
- 'f' or 'flash': Flash the build/<FILE>.hex file onto the chip.
- 'e' or 'erase': Delete the build directory.
- 'h' or 'help': Print this message.
```

## Projects

- **LED Flicker**: Connect an LED with PB4 and GND of the ATtiny, compile with `./build.sh c led` and flash with `./build.sh f led` and observe.

## LICENSE

_MIT, just do with it what you'd like to. But respect the copyright._