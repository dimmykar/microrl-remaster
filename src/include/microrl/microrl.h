/**
 * \file            microrl.h
 * \brief           Micro Read Line library
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

#ifndef MICRORL_HDR_H
#define MICRORL_HDR_H

#include <stdint.h>
#include <stddef.h>
#include "microrl_config.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * \defgroup        MICRORL Micro Read Line library
 * \brief           Micro Read Line library
 * \{
 */

/**
 * \brief           Unused variable macro
 * \param[in]       x: Unused variable
 */
#define MICRORL_UNUSED(x)           ((void)(x))

/**
 * \brief           Calculate size of statically allocated array
 * \param[in]       x: Input array
 * \return          Number of array elements
 */
#define MICRORL_ARRAYSIZE(x)        (sizeof(x) / sizeof((x)[0]))

/**
 * \brief           MicroRL result enumeration
 */
typedef enum {
    microrlOK = 0x00,                            /*!< Everything OK */
    microrlERR = 0x01,                           /*!< Common error */
    microrlERRPAR = 0x02,                        /*!< Parameter error */
    microrlERRTKNNUM = 0x03,                     /*!< Too many tokens */
    microrlERRCLFULL = 0x04,                     /*!< Command line is full */
    microrlERRCPLT = 0x05                        /*!< Auto-completion error */
} microrlr_t;

/**
 * \brief           ESC seq internal codes
 */
typedef enum {
    MICRORL_ESC_BRACKET,                        /*!< Encountered '[' character after ESC code */
    MICRORL_ESC_HOME,                           /*!< Encountered 'HOME' code after ESC code */
    MICRORL_ESC_END,                            /*!< Encountered 'END' code after ESC code */
    MICRORL_ESC_DEL                             /*!< Encountered 'DEL' code after ESC code */
} microrl_esc_code_t;

#if MICRORL_CFG_USE_ECHO_OFF || __DOXYGEN__
/**
 * \brief           List of possible echo modes
 */
typedef enum {
    MICRORL_ECHO_ONCE,                          /*!< Echo is disabled until Enter is pressed */
    MICRORL_ECHO_ON,                            /*!< Echo is always enabled */
    MICRORL_ECHO_OFF                            /*!< Echo is always disabled */
} microrl_echo_t;
#endif /* MICRORL_CFG_USE_ECHO_OFF || __DOXYGEN__ */

/* Forward declarations */
struct microrl;
#if MICRORL_CFG_USE_HISTORY
struct microrl_hist_rbuf;
#endif /* MICRORL_CFG_USE_HISTORY */

#if MICRORL_CFG_USE_HISTORY || __DOXYGEN__
/**
 * \brief           History struct, contains internal variable
 *
 * History stores in static ring buffer for memory saving
 *
 */
typedef struct microrl_hist_rbuf {
    char ring_buf[MICRORL_CFG_RING_HISTORY_LEN];/*!< History buffer */
    size_t head;                                /*!< Buffer head position */
    size_t tail;                                /*!< Buffer tail position */
    size_t count;                               /*!< Navigation counter */
} microrl_hist_rbuf_t;
#endif /* MICRORL_CFG_USE_HISTORY || __DOXYGEN__ */

/**
 * \brief           String output callback function
 * \param[in]       mrl: microRL working instance
 * \param[in]       str: String to print
 * \return          The number of characters that would have been written,
 *                      not counting the terminating null character.
 */
typedef int       (*microrl_output_fn)(struct microrl* mrl, const char* str);

/**
 * \brief           Command execute callback function
 * \param[in]       mrl: microRL working instance
 * \param[in]       argc: argument count
 * \param[in]       argv: pointer array to token string
 * \return          '0' on success, '1' otherwise
 */
typedef int       (*microrl_exec_fn)(struct microrl* mrl, int argc, const char* const *argv);

/**
 * \brief           Auto-complete function prototype
 * \param[in,out]   mrl: \ref microrl_t working instance
 * \param[in]       argc: argument count
 * \param[in]       argv: pointer array to token string
 * \return          NULL-terminated string, contain complete variant split by 'Whitespace'
 *                      If complete token found, it's must contain only one token to be completed
 *                      Empty string if complete not found, and multiple string if there are some token
 */
typedef char**    (*microrl_get_compl_fn)(struct microrl* mrl, int argc, const char* const *argv);

/**
 * \brief           Ctrl+C terminal signal function prototype
 * \param[in,out]   mrl: \ref microrl_t working instance
 */
typedef void      (*microrl_sigint_fn)(struct microrl* mrl);

/**
 * \brief           MicroRL instance
 */
typedef struct microrl {
    microrl_output_fn out_fn;                   /*!< Output function for print operations */
    microrl_exec_fn exec_fn;                    /*!< Command execute callback */
#if MICRORL_CFG_USE_COMPLETE || __DOXYGEN__
    microrl_get_compl_fn get_completion_fn;     /*!< Auto-completion callback */
#endif /* MICRORL_CFG_USE_COMPLETE || __DOXYGEN__ */
#if MICRORL_CFG_USE_CTRL_C || __DOXYGEN__
    microrl_sigint_fn sigint_fn;                /*!< Ctrl+C terminal signal callback */
#endif /* MICRORL_CFG_USE_CTRL_C || __DOXYGEN__ */

    char* prompt_ptr;                           /*!< Pointer to prompt string */
    char cmdline_str[MICRORL_CFG_CMDLINE_LEN + 1];  /*!< Command line input buffer with NULL character */
    size_t cmdlen;                              /*!< Command length in command line buffer */
    size_t cursor;                              /*!< Command line buffer position pointer */
    char last_endl;                             /*!< Either 0 or the CR or LF that just triggered a newline */

#if MICRORL_CFG_USE_ESC_SEQ || __DOXYGEN__
    microrl_esc_code_t esc_code;                /*!< Code of first escape sequence symbol */
    uint8_t escape;                             /*!< Escape sequence caught flag */
#endif /* MICRORL_CFG_USE_ESC_SEQ || __DOXYGEN__ */

#if MICRORL_CFG_USE_HISTORY || __DOXYGEN__
    microrl_hist_rbuf_t ring_hist;              /*!< Ring history object */
#endif /* MICRORL_CFG_USE_HISTORY || __DOXYGEN__ */

#if MICRORL_CFG_USE_ECHO_OFF || __DOXYGEN__
    microrl_echo_t echo;                        /*!< Echo mode */
    int32_t echo_off_pos;                       /*!< Start position to print '*' echo off chars */
#endif /* MICRORL_CFG_USE_ECHO_OFF || __DOXYGEN__ */

    void* userdata_ptr;                         /*!< Generic user data storage */
} microrl_t;

microrlr_t  microrl_init(microrl_t* mrl, microrl_output_fn out_fn, microrl_exec_fn exec_fn);

microrlr_t  microrl_set_execute_callback(microrl_t* mrl, microrl_exec_fn exec_fn);
#if MICRORL_CFG_USE_COMPLETE
microrlr_t  microrl_set_complete_callback(microrl_t* mrl, microrl_get_compl_fn get_completion_fn);
#endif /* MICRORL_CFG_USE_COMPLETE */
#if MICRORL_CFG_USE_CTRL_C
microrlr_t  microrl_set_sigint_callback(microrl_t* mrl, microrl_sigint_fn sigint_fn);
#endif /* MICRORL_CFG_USE_CTRL_C */

microrlr_t  microrl_set_prompt(microrl_t* mrl, char* prompt_str);
#if MICRORL_CFG_USE_ECHO_OFF || __DOXYGEN__
microrlr_t  microrl_set_echo(microrl_t* mrl, microrl_echo_t echo);
#endif /* #if MICRORL_CFG_USE_ECHO_OFF */

microrlr_t  microrl_processing_input(microrl_t* mrl, const void* data_ptr, size_t len);

uint32_t    microrl_get_version(void);

/**
 * \}
 */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif  /* MICRORL_HDR_H */
