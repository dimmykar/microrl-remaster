/**
 * \file            microrl_config.h
 * \brief           MicroRL library default configurations
 */

/*
 * Portion Copyright (c) 2011 Eugene SAMOYLOV
 * Portion Copyright (c) 2021 Dmitry KARASEV
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * This file is part of MicroRL - Micro Read Line library for small and embedded devices.
 *
 * Authors:         Eugene SAMOYLOV aka Helius <ghelius@gmail.com>,
 *                  Dmitry KARASEV <karasevsdmitry@yandex.ru>
 * Version:         2.5.0-dev
 */

#ifndef MICRORL_HDR_DEFAULT_CONFIG_H
#define MICRORL_HDR_DEFAULT_CONFIG_H

/* Uncomment to ignore user configs (or set macro in compiler flags) */
/* #define MICRORL_IGNORE_USER_CONFIGS */

/* Include application options */
#ifndef MICRORL_IGNORE_USER_CONFIGS
#include "microrl_user_config.h"
#endif /* MICRORL_IGNORE_USER_CONFIGS */

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * \defgroup        MICRORL_CFG Configuration
 * \brief           MicroRL configurations
 * \{
 */

/**
 * \brief           Command line length that specifies the size of the command line buffer.
 *                  Set the maximum number of characters. If the user inputs more characters
 *                  than MICRORL_CFG_CMDLINE_LEN, no characters are added on the command line.
 */
#ifndef MICRORL_CFG_CMDLINE_LEN
#define MICRORL_CFG_CMDLINE_LEN               60
#endif

/**
 * \brief           Number of tokens in the command. Set the maximum number of tokens on the command line.
 *                  If the number of tokens typed in the command line exceeds this value, then a message is printed
 *                  about this, the command line will not be parsed and the 'execute' callback will not be called.
 *                  Token is a word, that separate by whitespace, for example, line with 3 tokens:
 *                  "> set mode test"
 */
#ifndef MICRORL_CFG_CMD_TOKEN_NMB
#define MICRORL_CFG_CMD_TOKEN_NMB             8
#endif

/**
 * \brief           Define default prompt string here
 */
#ifndef MICRORL_CFG_PROMPT_STRING
#define MICRORL_CFG_PROMPT_STRING             "> "
#endif

#define MICRORL_COLOR_RED                     "\033[31m"
#define MICRORL_COLOR_GREEN                   "\033[32m"
#define MICRORL_COLOR_YELLOW                  "\033[33m"
#define MICRORL_COLOR_BLUE                    "\033[34m"
#define MICRORL_COLOR_PURPLE                  "\033[35m"
#define MICRORL_COLOR_MAGENTA                 "\033[36m"
#define MICRORL_COLOR_WHITE                   "\033[37m"
#define MICRORL_COLOR_DEFAULT                 "\033[0m"

/**
 * \brief           Use colors escape code, for highlight you prompt, if your terminal supports color
 */
#ifndef MICRORL_CFG_PROMPT_COLOR
#define MICRORL_CFG_PROMPT_COLOR              MICRORL_COLOR_GREEN
#endif

/**
 * \brief           Enable it, if you want to use completion functional, also set completion callback in you code.
 *                  Completion functional calls 'copmletion' callback if user press 'TAB'.
 */
#ifndef MICRORL_CFG_USE_COMPLETE
#define MICRORL_CFG_USE_COMPLETE              0
#endif

/**
 * \brief           Enable it, if you want to allow quoting command arguments to include spaces.
 *                  Quoting protects whitespace, for example, 2 quoted tokens:
 *                  "> set wifi "Home Net" "secret password"
 */
#ifndef MICRORL_CFG_USE_QUOTING
#define MICRORL_CFG_USE_QUOTING               0
#endif

/**
 * \brief           Enable it, if you want to use "echo off" feature.
 *                  "Echo off" is used for typing the secret input data, like passwords.
 *                  When the feature is enabled, there are 2 type of echo off: ONCE and OFF.
 *                  Use \ref MICRORL_ECHO_ONCE to disable echo for password input, echo mode
 *                  will enabled after user press 'Enter'.
 *                  Use \ref MICRORL_ECHO_ON or \ref MICRORL_ECHO_OFF to turn on or off the
 *                  echo manually.
 *                  At library initialization echo is ON
 */
#ifndef MICRORL_CFG_USE_ECHO_OFF
#define MICRORL_CFG_USE_ECHO_OFF              0
#endif

/**
 * \brief           Set the character, which will mask the secret input when echo is off
 *                  You can set it to '\0' to disable output of secret data to the terminal
 */
#ifndef MICRORL_CFG_ECHO_OFF_MASK
#define MICRORL_CFG_ECHO_OFF_MASK             '*'
#endif

/**
 * \brief           Enable it, if you want to use history. It works like bash history, and
 *                  sets stored value to command line, if 'UP' or 'DOWN' key is pressed.
 *                  Using of history increases memory consumption and depends on the
 *                  MICRORL_CFG_RING_HISTORY_LEN parameter
 */
#ifndef MICRORL_CFG_USE_HISTORY
#define MICRORL_CFG_USE_HISTORY               1
#endif

/**
 * \brief           History ring buffer length. Defines static buffer size.
 *                  To save memory, each command typed is stored in history ring buffer.
 *                  So we can not say, how many line we can store, it depends from command line length,
 *                  but memory using more effective. We not prefer dinamic memory allocation for small and
 *                  embedded devices. Overhead is 1 char on each saved record (command + terminating zero)
 */
#ifndef MICRORL_CFG_RING_HISTORY_LEN
#define MICRORL_CFG_RING_HISTORY_LEN          64
#endif

/**
 * \brief           Size of the buffer used for piecemeal printing of part or all of the command
 *                  line buffer. Allocated on the stack. Must be at least 16.
 */
#ifndef MICRORL_CFG_PRINT_BUFFER_LEN
#define MICRORL_CFG_PRINT_BUFFER_LEN          40
#endif

/**
 * \brief           Enable if for handling terminal ESC sequences. If disabled, then cursor arrow,
 *                  HOME, END will not work. Use Ctrl+A(B,F,P,N,A,E,H,K,U,C). See README.md for more info.
 *                  This functionality increases the code memory.
 */
#ifndef MICRORL_CFG_USE_ESC_SEQ
#define MICRORL_CFG_USE_ESC_SEQ               1
#endif

/**
 * \brief           Enable it for use 'sprintf()' implementation from your compiler's standard library, but
 *                  this adds some overhead. If not enabled, that uses my own number conversion code,
 *                  which save about 800 byte of code size on AVR (avr-gcc build).
 *                  Try to build with and without, and compare total code size for tune library.
 */
#ifndef MICRORL_CFG_USE_LIBC_STDIO
#define MICRORL_CFG_USE_LIBC_STDIO            0
#endif

/**
 * \brief           Use a single carriage return character to move the cursor to the left margin
 *                  rather than moving left by a large number. This reduces the number of
 *                  characters sent to the terminal, but should be left undefined if the terminal
 *                  will also simulate a linefeed when it receives the carriage return.
 */
#ifndef MICRORL_CFG_USE_CARRIAGE_RETURN
#define MICRORL_CFG_USE_CARRIAGE_RETURN       1
#endif

/**
 * \brief           Enable it and add an 'interrupt signal' callback to invoke it when the user presses Ctrl+C
 */
#ifndef MICRORL_CFG_USE_CTRL_C
#define MICRORL_CFG_USE_CTRL_C                0
#endif

/**
 * \brief           Print prompt at 'microrl_init()'. If enable, prompt will print at startup,
 *                  otherwise first prompt will print after first press 'Enter' in terminal
 * \note            Enable it, if you call 'microrl_init()' after your communication subsystem
 *                  already initialize and ready to print message
 */
#ifndef MICRORL_CFG_PROMPT_ON_INIT
#define MICRORL_CFG_PROMPT_ON_INIT            1
#endif

/**
 * \brief           Newline symbol printed in terminal
 *
 * The symbol must be "\r", "\n", "\r\n" or "\n\r"
 */
#ifndef MICRORL_CFG_END_LINE
#define MICRORL_CFG_END_LINE                  "\r\n"
#endif

/**
 * \brief           Enable it to use user-defined pre- and post- command execute callbacks (hooks)
 */
#ifndef MICRORL_CFG_USE_COMMAND_HOOKS
#define MICRORL_CFG_USE_COMMAND_HOOKS         0
#endif

/**
 * \brief           Optional user implemented function called before command execution callback
 *                      Not called if \ref MICRORL_CFG_USE_COMMAND_HOOKS is set to 0
 * \param[in]       mrl: Pointer to microRL working instance
 * \param[in]       argc: Number of arguments in command line
 * \param[in]       argv: Pointer to argument list
 */
#ifndef MICRORL_PRE_COMMAND_HOOK
#define MICRORL_PRE_COMMAND_HOOK(mrl, argc, argv)
#endif

/**
 * \brief           Optional user implemented function called after command execution callback
 *                      Not called if \ref MICRORL_CFG_USE_COMMAND_HOOKS is set to 0
 * \param[in]       mrl: Pointer to microRL working instance
 * \param[in]       res: Return value of the command execution callback
 * \param[in]       argc: Number of arguments in command line
 * \param[in]       argv: Pointer to argument list
 */
#ifndef MICRORL_POST_COMMAND_HOOK
#define MICRORL_POST_COMMAND_HOOK(mrl, res, argc, argv)
#endif

/**
 * \brief           Set compiler specific keywords notation for inline functions
 *                      In GGC the 'inline' is used, but in KEIL-MDK the '__inline' is used
 * \note            You can set just 'static' option to not use inline functions
 */
#ifndef MICRORL_CFG_STATIC_INLINE
#define MICRORL_CFG_STATIC_INLINE             static inline
#endif
/**
 * \}
 */

#define MICRORL_VERSION_MAJOR                 2
#define MICRORL_VERSION_MINOR                 5
#define MICRORL_VERSION_PATCH                 0

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif  /* MICRORL_HDR_DEFAULT_CONFIG_H */
