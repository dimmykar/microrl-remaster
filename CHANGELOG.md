# Changelog

## Develop




## v2.5.0

1.  Move `inline` keyword setting to config file
2.  Add configuration parameter for prompt coloring


## v2.4.2

1. Fix possible stuck when calculating records number in history
2. Minor code improvements

## v2.4.1

1. Fix compilation error when MICRORL_CFG_USE_ECHO_OFF=0 [#6](https://github.com/dimmykar/microrl-remaster/issues/6)


## v2.4.0

1.  Add DELETE key support
2.  Add Alt+Backspace support
3.  Add new ESC codes for HOME and END keys
4.  New variable naming has been applied. The backward compatibility of the `userdata` attribute of the `microrl_t` structure is broken, the rest of the attributes should be private for the developer. The API is not changed
5.  Rework history navigation


## v2.3.1

1.  Code refactoring to improve readability
2.  Skip completion (TAB) if echo is OFF
3.  Fix skipping saving the same command as last record in history
    - Previously, if the last record was restored from the history by pressing UP key and executed, then it was still saved


## v2.3.0

1.  Add config `MICRORL_CFG_USE_ECHO_OFF` for enable/disable "echo off" feature in library. By default the feature is disabled
2.  Add config `MICRORL_CFG_ECHO_OFF_MASK` for set the echo off masking character. By default the input is masked by `*`
3.  Add single quotes `'` support in addition to double quotes `"`
4.  Fix restoring the echo in `ECHO_ONCE` mode. Now if command line is empty in ECHO_ONCE mode and the user is pressed Enter, the echo will not switch to `ECHO_ON` mode
5.  Now the same command as the last record in history isn't saved


## v2.2.3

1.  Add missing including of `stddef.h` for `size_t` type to prevent compiling errors [#1](https://github.com/dimmykar/microrl-remaster/issues/1)
2.  Fix wrong signed-unsigned types comparsions and other possible warnings while compiling [#1](https://github.com/dimmykar/microrl-remaster/issues/1)
3.  Fix record erasing from history in case of all existing records erased
4.  Fix restoring of empty line in case of restoring the first previous record and pressing `DOWN` key to back to empty line


## v2.2.2

1.  Fix possible command line buffer overflow
2.  Fix echo off feature: previously this feature only worked with `MICRORL_ECHO_ONCE`, now it also works with `MICRORL_ECHO_OFF`


## v2.2.1

1.  Build of Unix and AVR demo have been fixed
    - Added TAB separators in targets of Makefiles replaced with spaces before
    - `microrl_user_config.h` from STM32 example replaced to `examples` folder. Now it is common for all examples, but `post_exec_hook()` demonstration is implemented only in STM32 example
    - Other fixes for successful build of Unix and AVR demos


## v2.2.0

1.  The `../src` folder has been reorganized to make it easier to use the library in various SDKs, to which library is added
2.  The `../src/include/microrl/microrl_user_config.h` file has been renamed to `microrl_user_config_template.h` for to avoid conflicts with used `microrl_user_config.h` file at project
3.  Added optional hooks called before and after command execution callback
    - Config `MICRORL_CFG_EXECUTE_STATUS_LOGGING` has been removed. Its functionality has been moved to `post_exec_hook()` in the stm32 example for command hooks functionality demonstration.
    - Added config `MICRORL_CFG_USE_COMMAND_HOOKS`, which enables using of `MICRORL_PRE_COMMAND_HOOK(mrl, argc, argv)` and `MICRORL_POST_COMMAND_HOOK(mrl, res, argc, argv)` hooks in microrl private code
4.  Added support of multiple compilers for inline keyword
5.  Fixed incorrect behavior when displaying autocomplete options
6.  Fixed incorrect determination of whether the command line buffer is full


## v2.1.1

1.  Added pre-configured STM32CubeIDE project with library demonstration example for STM32F415RG MCU


## v2.1.0

1.  Set execute callback function `microrlr_t microrl_set_execute_callback(microrl_t* mrl, microrl_exec_fn exec_fn)` from [v1.5.1-dev](https://github.com/Helius/microrl/commit/d044bf4300be57b1d6b298a794d5af7c51cf4de8) is restored
    - This function may be useful to switch multiple execution callbacks: for example, one for entering a login and password, another for execution commands after log in
2.  Updated STM32 example with two execute callbacks
3.  Added optional command status logging
    - Enable `MICRORL_CFG_EXECUTE_STATUS_LOGGING` to output command exiting status, if it is not equal 0


## v2.0.0

Changes since [v1.5.1-dev](https://github.com/Helius/microrl/commit/d044bf4300be57b1d6b298a794d5af7c51cf4de8):

1.  Library APIs changes:
    - Library initialization function
      - Setting of execute callback is replaced to `microrlr_t microrl_init(microrl_t* mrl, microrl_output_fn out_fn, microrl_exec_fn exec_fn)` initialization function
      - Execute callback setting function `microrl_set_execute_callback()` is removed
    - Complete and Singint callbacks setting functions return new result type `microrlr_t`
    - String output callback function `void print(microrl_t* mrl, const char* str)` replaced with `int out_fn(microrl_t* mrl, const char* str)`
    - Input processing function `void microrl_insert_char(microrl_t* mrl, int ch)` is replaced with `microrlr_t microrl_processing_input(microrl_t* mrl, const void* in_data, size_t len)`
      - New function accepts an array of input data for processing and its length instead of a single character, which can be useful for processing, for example, DMA transfers with ring buffer
    - Added `microrlr_t  microrl_set_prompt(microrl_t* mrl, char* prompt)` function to set prompt string
    - Added `uint32_t microrl_get_version(void)` function to get version of MicroRL library
2.  `bool` type defines is removed to prevent compiler warnings
    - Added result enumeration type `microrlr_t` instead
3.  New library formatting style
    - Descriptions of types, functions and defines are done in DOXYGEN style
    - Active use of enumeration types to make code easier to understand
    - Library APIs return result `microrlr_t` type
      - Added checks for passing NULL pointers to all APIs
    - Added file headers with license to all library files
    - The project follows the [Tilen Majerle formatting style](https://github.com/MaJerle/c-code-style)
      - But function declarations are placed on one line
    - Using platform-independent `int` types
4.  Library configurations file changes
    - `config.h` file is renamed to `microrl_config.h`. This file contains MicroRL library default configurations
    - `microrl_user_config.h` file with user defined configs is created. Copy to this file the configurations that you want to configure from the `microrl_config.h` file and replace it to your project. This way you can easily update the library without affecting your customized configurations
    - Changed the naming system of configs for forced reconfiguration of the library in accordance with the new changes
      - Some old configs have been removed, others have changed their meaning
5.  MicroRL instance changes
    - Command line buffer size changed to `_COMMAND_LINE_LEN + 1` in `microrl_t`. Config `_COMMAND_LINE_LEN` now contains the size of the command line buffer without a terminating zero
6.  MicroRL adaptation for embedding in C++ programs (`Stephen Casner @slcasner` commits are integrated)
    - Added a `void* userdata` member to `microrl_t` struct that can be used by applications to store a C++ object pointer or other context info
    - Passed the pointer to `microrl_t` in all callbacks so that the operations can be specific to a particular instance of microrl
    - Added `extern "C"` C++ guards to header files
7.  Reduced and consolidated print operations (`Stephen Casner @slcasner` commit are integrated)<br>
    When used over a network connection, each `print()` call in microrl may be transmitted as a separate packet, so the many small print operations make output slow. This commit optimizes some common cases to avoid unnecessary prints and combines others into a buffer to be printed together.
    - When adding characters to the end of a command, now just the single character is output. This avoids cursor jumping. Similarly, when backspacing at the end of the line, just the simple sequence back-one, space, back-one is sufficient.
    - The biggest number of back-to-back print operations occurred in `terminal_print_line()` where each character was printed separately. Now the characters are packed into a temporary buffer first so they can be printed in one or a few operations, depending upon the command length and buffer size. Positioning sequences are also consolidated into the buffer.
    - Some calls to `terminal_reset_cursor()` that were changed to call `terminal_move_cursor()` instead since the latter outputs fewer characters and avoids extra jumping of the cursor on slow lines. The code to reset the cursor position was then incorporated into `terminal_print_line()` where those positioning operations can be consolidated into a print buffer. Since the reset is not always necessary, an argument controls whether it is done.
    - A new define `_USE_CARRIAGE_RETURN` controls using a carriate return to reposition the cursor to the left margin rather than moving left by a large number. This takes fewer characters. This optimization can be turned off if the terminal simulates receiving a linefeed when it receives the carriage return.
    - In `terminal_print_line()`, the delete-to-end-of-line escape sequence is moved to be output after all of the command text. This avoids a flash of the text going away and then being repainted.
    - The code to create repositioning sequences was extracted into a separate function `generate_move_cursor()` so it can be shared between `terminal_move_cursor()` and `terminal_print_line()`.
    - After reorganizing the code to generate repositioning strings there was only one call to `u16bit_to_str()`, so it was merged inline.
    - `strcpy()` calls of just a few characters are replaced by depositing the characters individually.  This removes a dependency and may use fewer instructions.
    - To allow `Ctrl+U` to backspace across all of the characters to the left of the curso in one motion, `microrl_backspace()` now takes a parameter for the number of spaces to move rather than always moving one.
8.  Implemented Echo control feature (method by `Maksim Chichikalov @m-chichikalov`)
    - When echo is disabled, `'*'` is printed to terminal instead of real characters for command buffer
    - Use `MICRORL_ECHO_ONCE` to disable echo for password input, echo mode will enabled after user press `'Enter'`.
    - Use `MICRORL_ECHO_ON` or `MICRORL_ECHO_OFF` to turn on or off the echo manually.
9.  Added smart newline handling: triggers once on any of CR, LF, CRLF or LFCR (`Tom Collins @tomlogic` commit is integrated)
10. Added `Ctrl+D` feature (delete character forward)
11. Refactoring the command line buffer tokenization function
    - Removed the mechanism for replacing whitespaces with zeros
    - Added optional Quoting feature to allow command args with spaces
12. History refactoring
    - It is now possible to use the history ring buffer larger than 256 bytes
      - Stored records in the buffer no longer contain the record length in first byte. Only null-terminated strings are stored in the buffer. This is done at the cost of a slight increase in the resource requirements of the target system.
13. Added an explanations into README.md with Markdown formatting
14. Added new examples
    - STM32 example with full library functionality
    - ESP8266 platformio example by `Maksim Chichikalov @m-chichikalov` with echo off feature
15. Found bugs were fixed, and, of course, new ones were added ;)
