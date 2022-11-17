/**
 * \file            avr_misc.c
 * \brief           AVR platform specific implementation routines (for Atmega8, rewrite for your MC)
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

#include <string.h>
#include <stdlib.h>
#include <avr/io.h>
#include "microrl.h"

#define _AVR_DEMO_VER       "1.0"

#define _ENDLINE_SEQ        "\n\r"

/* Definition commands word */
#define _CMD_HELP           "help"
#define _CMD_CLEAR          "clear"
#define _CMD_CLR            "clear_port"
#define _CMD_SET            "set_port"
/* Arguments for set/clear */
#define _SCMD_PB            "port_b"
#define _SCMD_PD            "port_d"

#define _NUM_OF_CMD              4
#define _NUM_OF_SETCLEAR_SCMD    2

/* Available  commands */
char* keyword[] = {_CMD_HELP, _CMD_CLEAR, _CMD_SET, _CMD_CLR};

/* 'set/clear' command argements */
char* set_clear_key[] = {_SCMD_PB, _SCMD_PD};

/* Array for comletion */
char* compl_word[_NUM_OF_CMD + 1];


/**
 * \brief           Init AVR platform
 */
void init(void) {
    UBRRL = 8; /* 19200 bps on 16MHz */
    UCSRC = (1 << URSEL) | (1 << UCSZ1) | (1 << UCSZ0);
    UCSRB = (1 << RXEN) | (1 << TXEN);
    DDRB = 0xFF;
    DDRD = 0xFF;
}

/**
 * \brief           Print to IO stream callback for MicroRL library
 * \param[in]       mrl: \ref microrl_t working instance
 * \param[in]       str: Output string
 * \return          The number of characters that would have been written,
 *                      not counting the terminating null character.
 */
int print(microrl_t* mrl, const char* str) {
    MICRORL_UNUSED(mrl);
    int i = 0;

    while (str[i] != 0) {
        while (!(UCSRA & (1 << UDRE)));
        UDR = str[i++];
    }

    return i;
}

/**
 * \brief           Get char user pressed
 * \return          Input character
 */
char get_char(void) {
    while (!(UCSRA & (1 << RXC)));
    return UDR;
}

/**
 * \brief           Сonverting the numeric MicroRL version to string
 * \param[out]      ver_str: 6-byte version string
 */
void get_version(char* ver_str) {
    uint32_t ver = microrl_get_version();

    ver_str[0] = (char)((ver >> 16) & 0x000000FF) + '0';
    ver_str[1] = '.';
    ver_str[2] = (char)((ver >> 8) & 0x000000FF) + '0';
    ver_str[3] = '.';
    ver_str[4] = (char)(ver & 0x000000FF) + '0';
}

/**
 * \brief           HELP command callback
 * \param[in]       mrl: \ref microrl_t working instance
 */
void print_help(microrl_t* mrl) {
    print(mrl, "Use TAB key for completion"_ENDLINE_SEQ"Command:"_ENDLINE_SEQ);
    print(mrl, "\tclear               - clear screen"_ENDLINE_SEQ);
    print(mrl, "\tset_port port pin   - set 1 port[pin] value, support only 'port_b' and 'port_d'"_ENDLINE_SEQ);
    print(mrl, "\tclear_port port pin - set 0 port[pin] value, support only 'port_b' and 'port_d'"_ENDLINE_SEQ);
}

/**
 * \brief           SET PIN VALUE OF PORT command callback
 * \param[in]       mrl: \ref microrl_t working instance
 * \param[in]       port: MCU GPIO port
 * \param[in]       pin: MCU GPIO pin
 * \param[in]       val: '0' to pulldown pin, '1' to pullup pin
 */
void set_port_val(microrl_t* mrl, unsigned char* port, int pin, int val) {
    if ((*port == PORTD) && (pin < 2) && (pin > 7)) {
        print(mrl, "only 2..7 pin avialable for PORTD"_ENDLINE_SEQ);
        return;
    }

    if ((*port == PORTB) && (pin > 5)) {
        print(mrl, "only 0..5 pin avialable for PORTB"_ENDLINE_SEQ);
        return;
    }

    if (val) {
        (*port) |= 1 << pin;
    } else {
        (*port) &= ~(1 << pin);
    }
}

/**
 * \brief           Execute callback for MicroRL library
 *
 * Do what you want here, but don't write to argv!!! read only!!
 *
 * \param[in]       mrl: \ref microrl_t working instance
 * \param[in]       argc: argument count
 * \param[in]       argv: pointer array to token string
 * \return          '0' on success, '1' otherwise
 */
int execute(microrl_t* mrl, int argc, const char* const *argv) {
    int i = 0;
    /* Just iterate through argv word and compare it with your commands */
    while (i < argc) {
        if (strcmp(argv[i], _CMD_HELP) == 0) {
            char ver_str[6] = {0};
            get_version(ver_str);

            print(mrl, "microrl v");
            print(mrl, ver_str);
            print(mrl, " library AVR DEMO v");
            print(mrl, _AVR_DEMO_VER);
            print(mrl, _ENDLINE_SEQ);
            print_help(mrl);        /* Print help */
        } else if (strcmp(argv[i], _CMD_CLEAR) == 0) {
            print(mrl, "\033[2J");    /* ESC seq for clear entire screen */
            print(mrl, "\033[H");     /* ESC seq for move cursor at left-top corner */
        } else if ((strcmp(argv[i], _CMD_SET) == 0) ||
                   (strcmp(argv[i], _CMD_CLR) == 0)) {
            if (++i < argc) {
                int val = strcmp(argv[i - 1], _CMD_CLR);
                unsigned char* port = NULL;
                int pin = 0;
                if (strcmp(argv[i], _SCMD_PD) == 0) {
                    port = (unsigned char*)&PORTD;
                } else if (strcmp(argv[i], _SCMD_PB) == 0) {
                    port = (unsigned char*)&PORTB;
                } else {
                    print(mrl, "only '");
                    print(mrl, _SCMD_PB);
                    print(mrl, "' and '");
                    print(mrl, _SCMD_PD);
                    print(mrl, "' support"_ENDLINE_SEQ);
                    return 1;
                }
                if (++i < argc) {
                    pin = atoi(argv[i]);
                    set_port_val(mrl, port, pin, val);
                    return 0;
                } else {
                    print(mrl, "specify pin number, use Tab"_ENDLINE_SEQ);
                    return 1;
                }
            }
        } else {
            print(mrl, "command: '");
            print(mrl, (char*)argv[i]);
            print(mrl, "' Not found."_ENDLINE_SEQ);
        }
        i++;
    }
    return 0;
}

#if MICRORL_CFG_USE_COMPLETE || __DOXYGEN__
/**
 * \brief           Completion callback for MicroRL library
 * \param[in,out]   mrl: \ref microrl_t working instance
 * \param[in]       argc: argument count
 * \param[in]       argv: pointer array to token string
 * \return          NULL-terminated string, contain complete variant split by 'Whitespace'
 */
char ** complete(microrl_t* mrl, int argc, const char* const *argv) {
    MICRORL_UNUSED(mrl);
    int j = 0;

    compl_word[0] = NULL;

    /* If there is token in cmdline */
    if (argc == 1) {
        /* Get last entered token */
        char* bit = (char*)argv[argc - 1];
        /* Iterate through our available token and match it */
        for (int i = 0; i < _NUM_OF_CMD; i++) {
            /* If token is matched (text is part of our token starting from 0 char) */
            if (strstr(keyword[i], bit) == keyword[i]) {
                /* Add it to completion set */
                compl_word[j++] = keyword[i];
            }
        }
    }  else if ((argc > 1) && ((strcmp(argv[0], _CMD_SET) == 0) ||
                             (strcmp(argv[0], _CMD_CLR) == 0))) { /* If command needs subcommands */
        /* Iterate through subcommand */
        for (int i = 0; i < _NUM_OF_SETCLEAR_SCMD; i++) {
            if (strstr(set_clear_key[i], argv[argc - 1]) == set_clear_key[i]) {
                compl_word[j++] = set_clear_key[i];
            }
        }
    } else { /* If there is no token in cmdline, just print all available token */
        for (; j < _NUM_OF_CMD; j++) {
            compl_word[j] = keyword[j];
        }
    }

    /* Note! Last ptr in array always must be NULL!!! */
    compl_word[j] = NULL;

    /* Return set of variants */
    return compl_word;
}
#endif /* MICRORL_CFG_USE_COMPLETE || __DOXYGEN__ */

#if MICRORL_CFG_USE_CTRL_C || __DOXYGEN__
/**
 * \brief           Ctrl+C terminal signal function
 * \param[in]       mrl: \ref microrl_t working instance
 */
void sigint(microrl_t* mrl) {
    print(mrl, "^C is caught!"_ENDLINE_SEQ);
}
#endif /* MICRORL_CFG_USE_CTRL_C || __DOXYGEN__ */

#if MICRORL_CFG_USE_COMMAND_HOOKS
/**
 * \brief           Hook called after command execution callback
 * \param[in,out]   mrl: \ref microrl_t working instance
 * \param[in]       res: Return value of the command execution callback
 * \param[in]       argc: Number of arguments in command line
 * \param[in]       argv: Pointer to argument list
 */
void post_exec_hook(microrl_t* mrl, int res, int argc, const char* const *argv) {
    MICRORL_UNUSED(mrl);
    MICRORL_UNUSED(res);
    MICRORL_UNUSED(argc);
    MICRORL_UNUSED(argv);
}
#endif /* MICRORL_CFG_USE_COMMAND_HOOKS */
