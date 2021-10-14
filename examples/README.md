# MicroRL library demo examples


## AVR demo

For AVR demo build, type

```
$make -f Makefile.avr
```

Connect USART to PC (usb-convertor work!) COM-port and open terminal like `minicom` with it COM-port

```
$minicom -c on -D /dev/ttyUSB0 -b 115200
# -c on: turn on color in terminal
# -D /dev/ttyUSB0: your COM-port (virtual if usb-converter is used)
# -b 11520: USART baud rate
```

| oscillator  |  baud-rate |
| -----------:|-----------:|
|    16MHz    |   115200   |
|     8MHz    |   57600    |
|     1MHz    |   14400    |


## Unix demo

For Unix demo build, just type

```
$make
```


## STM32 demo

To run the STM32 demo, import the `examples/stm32_example/STM32CubeIDE` project into the STM32CubeIDE workspace and build one of its Build Configurations. No changes are required to build the project.

Connect USART to PC (usb-convertor work!) COM-port and open terminal like `minicom` with it COM-port

```
$minicom -c on -D /dev/ttyUSB0 -b 115200
# -c on: turn on color in terminal
# -D /dev/ttyUSB0: your COM-port (virtual if usb-converter is used)
# -b 11520: USART baud rate
```


## ESP8266 demo

Read `examples/esp8266_example/lib/readme.txt` to run this example in PlatformIO