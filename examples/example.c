/**
 * \file            example.c
 * \brief           Platform independent interface for implementing some
 *                  specific function for AVR, linux PC or ARM
 */

/*
 * Copyright (c) 2011 Eugene SAMOYLOV
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
 * Author:          Eugene SAMOYLOV aka Helius <ghelius@gmail.com>
 * Version:         2.5.0-dev
 */

#include <stdio.h>
#include <string.h>
#include "microrl.h"
#include "example_misc.h"

/* Create microrl instance and pointer to it */
microrl_t rl;
microrl_t* prl = &rl;

/**
 * \brief           Program entry point
 */
int main (void/*int argc, char** argv*/) {
    /* Hardware initialization */
    init();

    /* Initialize library with microrl instance and print and execute callbacks */
    microrl_init(prl, print, execute);

#if MICRORL_CFG_USE_COMPLETE
    /* Set callback for auto-completion */
    microrl_set_complete_callback(prl, complete);
#endif /* MICRORL_CFG_USE_COMPLETE */

#if MICRORL_CFG_USE_CTRL_C
    /* Set callback for Ctrl+C handling */
    microrl_set_sigint_callback(prl, sigint);
#endif /* MICRORL_CFG_USE_CTRL_C */

    while (1) {
        /* Put received char from stdin to microrl instance */
        char ch = get_char();
        microrl_processing_input(prl, &ch, 1);
    }
    return 0;
}
