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
 * Version:         2.0.0-dev
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
 *                  than MICRORL_CFG_CMDLINE_LEN, no characters are added to the command line.
 */
#ifndef MICRORL_CFG_CMDLINE_LEN
#define MICRORL_CFG_CMDLINE_LEN               60
#endif

/**
 * \brief           Command token number, define max token it command line, if number of token 
 *                  typed in command line exceed this value, then prints message about it and
 *                  command line not to be parced and 'execute' callback will not calls.
 *                  Token is word separate by white space, for example 3 token line:
 *                  "IRin> set mode test"
 */
#ifndef MICRORL_CFG_CMD_TOKEN_NMB
#define MICRORL_CFG_CMD_TOKEN_NMB             8
#endif

/**
 * \brief           Define default prompt string here
 */
#ifndef MICRORL_CFG_PROMPT_STRING
#define MICRORL_CFG_PROMPT_STRING             "IRin > "
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
 * \brief           Define it, if you wanna use completion functional, also set completion callback in you code,
 *                  now if user press TAB calls 'copmlitetion' callback. If you no need it, you can just set 
 *                  NULL to callback ptr and do not use it, but for memory saving tune, 
 *                  if you are not going to use it - disable this define.
 */
#ifndef MICRORL_CFG_USE_COMPLETE
#define MICRORL_CFG_USE_COMPLETE              0
#endif

/**
 * \brief           Define it, if you want to allow quoting command arguments to include spaces.
 *                  Quoting protects whitespace, for example 2 quoted tokens:
 *                  IRin> set wifi "Home Net" "this is a secret"
 */
#ifndef MICRORL_CFG_USE_QUOTING
#define MICRORL_CFG_USE_QUOTING               0
#endif

/**
 * \brief           Define it, if you wanna use history. It s work's like bash history, and
 *                  set stored value to cmdline, if UP and DOWN key pressed. Using history add
 *                  memory consuming, depends from _RING_HISTORY_LEN parametr
 */
#ifndef MICRORL_CFG_USE_HISTORY
#define MICRORL_CFG_USE_HISTORY               1
#endif

/**
 * \brief           History ring buffer length, define static buffer size.
 *                  For saving memory, each entered cmdline store to history in ring buffer,
 *                  so we can not say, how many line we can store, it depends from cmdline len,
 *                  but memory using more effective. We not prefer dinamic memory allocation for
 *                  small and embedded devices. Overhead is 2 char on each saved line
 */
#ifndef MICRORL_CFG_RING_HISTORY_LEN
#define MICRORL_CFG_RING_HISTORY_LEN          64
#endif

/**
 * \brief           Size of the buffer used for piecemeal printing of part or all of the command
 *                  line.  Allocated on the stack. Must be at least 16.                 
 */
#ifndef MICRORL_CFG_PRINT_BUFFER_LEN
#define MICRORL_CFG_PRINT_BUFFER_LEN          40
#endif

/**
 * \brief           Enable Handling terminal ESC sequence. If disabling, then cursor arrow, HOME, END will not work,
 *                  use Ctrl+A(B,F,P,N,A,E,H,K,U,C) see README, but decrease code memory.
 */
#ifndef MICRORL_CFG_USE_ESC_SEQ
#define MICRORL_CFG_USE_ESC_SEQ               1
#endif

/**
 * \brief           Use sprintf from you standard complier library, but it gives some overhead.
 *                  If not defined, use my own number conversion code, it's save about 800 byte of
 *                  code size on AVR (avr-gcc build).
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
 * \brief           Enable 'interrupt signal' callback, if user press Ctrl+C
 */
#ifndef MICRORL_CFG_USE_CTRL_C
#define MICRORL_CFG_USE_CTRL_C                0
#endif

/**
 * \brief           Print prompt at 'microrl_init()'. If enable, prompt will print at startup, 
 *                  otherwise first prompt will print after first press Enter in terminal
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
 * \}
 */

#define MICRORL_VERSION_MAJOR               2
#define MICRORL_VERSION_MINOR               0
#define MICRORL_VERSION_PATCH               0

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif  /* MICRORL_HDR_DEFAULT_CONFIG_H */
