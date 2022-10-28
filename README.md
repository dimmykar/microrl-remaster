# Micro Read Line library for small and embedded devices with basic VT100 support

This library is a remaster of the [microrl](https://github.com/Helius/microrl) library, which author stopped its development in 2017.

## Description

Microrl library is designed to help implement command line interface in small and embedded devices. Main goal is to write compact, small memory consuming but powerful interfaces, with support navigation through command line with cursor, HOME, END keys, hot key like Ctrl+U and other, history and completion feature.

<img src="/img/demo.png" alt="Window of terminal with microrl library"/>


## Features

  - `microrl_config.h` and `microrl_user_config.h` files
    * Turn on/off feature for add functional/decrease memory via config files.

  - Pass the pointer to `microrl_t` to all callbacks so that the operations can be specific to a particular instance of microrl

  - Hot keys support
    * Backspace, cursor arrows, HOME, END keys
    * Ctrl+U (cut line from cursor to begin)
    * Ctrl+K (cut line from cursor to end)
    * Ctrl+A (like HOME)
    * Ctrl+E (like END)
    * Ctrl+H (like backspace)
    * Ctrl+D (delete one character forward)
    * Ctrl+B (like cursor arrow left) 
    * Ctrl+F (like cursor arrow right)
    * Ctrl+P (like cursor arrow up)
    * Ctrl+N (like cursor arrow down)
    * Ctrl+R (retype prompt and partial command)
    * Ctrl+C (call `sigint()` callback, only for embedded system)

  - History (optional)
    * Static ring buffer history for memory saving. Number of commands saved to history depends from commands length and buffer size (defined in config)

  - Completion (optional)
    * Command completion via completion callback

  - Quoting (optional)
    * Use single `'` or double `"` quotes around a command argument that needs to include space characters

  - Echo control (optional)
    * Use `microrl_set_echo()` function to turn on or turn off echo
    * Could be used to print `*` or other specified character insted of real characters to mask secret input like passwords


## Source code organization

```
src/                             - library sources
src/microrl
  microrl.c                      - microrl routines
src/include/microrl
  microrl.h                      - lib interface and data type
  microrl_config.h               - file with default configs
  microrl_user_config_template.h - customisation config-file template
examples/                        - library usage examples
  avr_misc/                      - avr specific routines for avr example
  esp8266_example/               - esp8266 (platformio) example with echo off feature
  stm32_example/                 - stm32 (STM32CubeIDE) example with full library functionality
  unix_misc/                     - unix specific routines for desktop example
  example.c                      - common part of example, for build demonstrating example for various platform
  example_misc.h                 - interface to platform specific routines for example build (avr, unix, stm32, esp8266)
  Makefile                       - unix example build (gcc)
  Makefile.avr                   - avr example build (avr-gcc)
```


## Install

Requirements: C compiler with support for C99 standard (GNU GCC, Keil, IAR) with standard C library (libc, uClibc or other compatible). Also you have to implement several routines in your own code for library to work.

__NOTE:__ need add `-std=gnu99` arg for gcc

For embed lib to you project, you need to do few simple steps:

a) Include `microrl.h` file to you project.

b) Create `microrl_t` object, and call `microrl_init()` func, with print callback pointer and your routine callback pointer, that will be called if user press Enter in terminal.<br>
Print callback pointer is pointer to function that call by library if it's need to put text to terminal. Text string always is null terminated.
For example on linux PC print callback may be:
```c
/* Print callback for microrl library */
int print(microrl_t* mrl, char* str) {
    return fprintf(stdout, "%s", str);
}
```
Execute callback pointer give a `argc`, `argv` parametrs, like `main()` func in application. All token in `argv` is null terminated. So you can simply walk through `argv` and handle commands.

c) If you want completion support if user press TAB key, call `microrl_set_complete_callback()` and set you callback. It also give `argc` and `argv` arguments, so iterate through it and return set of complete variants.

d) Rename `microrl_user_config_template.h` to `microrl_user_config.h`. Look at `microrl_config.h` file and tune library in `microrl_user_config.h`. To do this, copy the default configs from `microrl_config.h` to `microrl_user_config.h` and change them for you requiring. Then replace `microrl_user_config.h` to libraries confuguration folder of your project.

e) Now you just call `microrl_processing_input()` on each input string (or one char) received from input stream (usart, network, etc).

Example of code:
```c
int main (int argc, char** argv) {
    /* Create microrl instance */
    microrl_t rl;

    /* Initialize library with microrl instance and print and execute callbacks */
    microrl_init(&rl, print, execute);

    /* Set callback for completion (optionally) */
    microrl_set_complete_callback(&rl, complet);

    /* Set callback for Ctrl+C handling (optionally) */
    microrl_set_sigint_callback(&rl, sigint);
    
    while (1) {
        /* Put received char from stdin to microrl instance */
        char ch = get_char();
        microrl_processing_input(&rl, &ch, 1);
    }

    return 0;
}
```

See examples of library usage.



Author: Eugene Samoylov aka Helius (ghelius@gmail.com)<br>
01.09.2011

Remastered by: Dmitry Karasev aka dimmykar (karasevsdmitry@yandex.ru)<br>
27.09.2021
