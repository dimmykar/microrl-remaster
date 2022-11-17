/**
 * \file            stm32_misc.c
 * \brief           STM32 platform specific implementation routines (for STM32F4, rewrite for your MCU)
 */

/*
 * Copyright (c) 2021 Dmitry KARASEV
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
 * Author:          Dmitry KARASEV <karasevsdmitry@yandex.ru>
 * Version:         2.5.0-dev
 */

#include <string.h>
#include <stdlib.h>
#include "stm32f4xx.h"
#include "stm32f4xx_ll_bus.h"
#include "stm32f4xx_ll_rcc.h"
#include "stm32f4xx_ll_gpio.h"
#include "stm32f4xx_ll_usart.h"
#include "microrl.h"

#define UART_TX_Pin                 LL_GPIO_PIN_10
#define UART_TX_GPIO_Port           GPIOC
#define UART_RX_Pin                 LL_GPIO_PIN_11
#define UART_RX_GPIO_Port           GPIOC

#define _STM32_DEMO_VER             "1.3"

#define _ENDLINE_SEQ                "\r\n"

/* Definition commands word */
#define _CMD_HELP                   "help"
#define _CMD_CLEAR                  "clear"
#define _CMD_SERNUM                 "sernum"
/* Arguments for set/clear */
#define _SCMD_RD                    "?"
#define _SCMD_SAVE                  "save"

#define _NUM_OF_CMD                 3
#define _NUM_OF_SETCLEAR_SCMD       2

#if MICRORL_CFG_USE_ECHO_OFF
#define SESSION_ADMIN_LOGIN        "admin"
#define SESSION_ADMIN_PASSW        "1234"
#endif /* MICRORL_CFG_USE_ECHO_OFF */

/* Available  commands */
char* keyword[] = {_CMD_HELP, _CMD_CLEAR, _CMD_SERNUM};

/* 'read/save' command argements */
char* read_save_key[] = {_SCMD_RD, _SCMD_SAVE};

/* Array for comletion */
char* compl_word[_NUM_OF_CMD + 1];

/* Variable changeable with commands */
uint32_t device_sn = 0;

#if MICRORL_CFG_USE_ECHO_OFF
/* Session status flags */
uint8_t  logged_in = 0;
uint8_t  passw_in = 0;
#endif /* MICRORL_CFG_USE_ECHO_OFF */

/**
 * \brief           Init STM32F4 platform
 */
void init(void) {
    LL_USART_InitTypeDef USART_InitStruct = {0};
    LL_GPIO_InitTypeDef GPIO_InitStruct = {0};

    LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOC);
    LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_UART4);
    __DSB();

    GPIO_InitStruct.Pin = UART_TX_Pin | UART_RX_Pin;
    GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
    GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
    GPIO_InitStruct.Pull = LL_GPIO_PULL_UP;
    GPIO_InitStruct.Alternate = LL_GPIO_AF_8;
    LL_GPIO_Init(GPIOC, &GPIO_InitStruct);

    USART_InitStruct.BaudRate = 115200;
    USART_InitStruct.DataWidth = LL_USART_DATAWIDTH_8B;
    USART_InitStruct.StopBits = LL_USART_STOPBITS_1;
    USART_InitStruct.Parity = LL_USART_PARITY_NONE;
    USART_InitStruct.TransferDirection = LL_USART_DIRECTION_TX_RX;
    USART_InitStruct.HardwareFlowControl = LL_USART_HWCONTROL_NONE;
    USART_InitStruct.OverSampling = LL_USART_OVERSAMPLING_16;
    LL_USART_Init(UART4, &USART_InitStruct);

    LL_USART_DisableIT_CTS(UART4);

    LL_USART_ConfigAsyncMode(UART4);

    LL_USART_Enable(UART4);
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
    uint32_t i = 0;

    while (str[i] != 0) {
        while (!LL_USART_IsActiveFlag_TXE(UART4)) {}
        LL_USART_TransmitData8(UART4, str[i++]);
    }

    return i;
}

/**
 * \brief           Get char user pressed
 * \return          Input character
 */
char get_char(void) {
    while (!LL_USART_IsActiveFlag_RXNE(UART4)) {}
    return (char)LL_USART_ReceiveData8(UART4);
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
    char ver_str[6] = {0};
    get_version(ver_str);

    print(mrl, "MicroRL v");
    print(mrl, ver_str);
    print(mrl, " library DEMO v");
    print(mrl, _STM32_DEMO_VER);
    print(mrl, _ENDLINE_SEQ);

#if MICRORL_CFG_USE_ECHO_OFF
    if (!logged_in) {
        print(mrl, "\tlogin YOUR_LOGIN      - 'admin' in this example"_ENDLINE_SEQ);
        print(mrl, "If login is correct, you will be asked to enter password."_ENDLINE_SEQ);
    }
#endif /* MICRORL_CFG_USE_ECHO_OFF */

#if MICRORL_CFG_USE_COMPLETE
    print(mrl, "Use TAB key for completion"_ENDLINE_SEQ);
#endif /* MICRORL_CFG_USE_COMPLETE */

    print(mrl, "List of commands:"_ENDLINE_SEQ);
    print(mrl, "\tclear               - clear screen"_ENDLINE_SEQ);
    print(mrl, "\tsernum ?            - read serial number value"_ENDLINE_SEQ);
    print(mrl, "\tsernum VALUE        - set serial number value"_ENDLINE_SEQ);
    print(mrl, "\tsernum save         - save serial number value to flash"_ENDLINE_SEQ);
}

/**
 * \brief           CLEAR command callback
 * \param[in]       mrl: \ref microrl_t working instance
 */
void clear_screen(microrl_t* mrl) {
    print(mrl, "\033[2J");    /* ESC seq for clear entire screen */
    print(mrl, "\033[H");     /* ESC seq for move cursor at left-top corner */
}

/**
 * \brief           Makes `unsigned 32-bit` value from ascii char array
 * \param[in]       str: Input string with value to convert
 * \param[out]      val: `unsigned 32-bit` data to be converted
 */
void str_to_u32(char* str, uint32_t* val) {
    uint32_t temp = 0;

    for (uint8_t i = 0; str[i] >= 0x30 && str[i] <= 0x39; ++i) {
        temp = temp + (str[i] & 0x0F);
        temp = temp * 10;
    }
    temp = temp / 10;

    *val = temp;
}

/**
 * \brief           Makes ascii char array from `unsigned 32-bit` value
 * \param[in]       val: `unsigned 32-bit` data to be converted
 * \param[out]      str: Minimum `11-bytes` long array to write value to
 */
void u32_to_str(uint32_t* val, char* str) {
    uint32_t v = *val;
    size_t s = 0;
    char t;

    size_t n;
    for (n = 0; v > 0; v /= 10) {
        str[s + n++] = "0123456789"[v % 10];
    }

    /* Reverse a string */
    for (size_t i = 0; i < n / 2; ++i) {
        t = str[s + i];
        str[s + i] = str[s + n - i - 1];
        str[s + n - i - 1] = t;
    }

    if (val == NULL) {
        str[n++] = '0';  /* Handle special case */
    }
}

/**
 * \brief           SERNUM ? command callback
 * \param[in]       mrl: \ref microrl_t working instance
 * \param[in]       val: new serial number value
 */
void read_sernum(microrl_t* mrl) {
    char sn_str[11] = {0};
    uint32_t sn = device_sn;
    u32_to_str(&sn, sn_str);

    print(mrl, "\tS/N ");
    print(mrl, sn_str);
    print(mrl, _ENDLINE_SEQ);
}

/**
 * \brief           SERNUM VALUE command callback
 * \param[in]       mrl: \ref microrl_t working instance
 * \param[in]       val: new serial number value
 */
void set_sernum(microrl_t* mrl, char* str_val) {
    uint32_t sn = 0;

    str_to_u32(str_val, &sn);
    if (sn != 0) {
        device_sn = sn;

        print(mrl, "\tset S/N ");
        print(mrl, str_val);
        print(mrl, _ENDLINE_SEQ);
        return;
    }

    print(mrl, "\tS/N not set"_ENDLINE_SEQ);
}

/**
 * \brief           SERNUM SAVE command callback
 * \param[in]       mrl: \ref microrl_t working instance
 */
void save_sernum(microrl_t* mrl) {
    /* To simplify the code, no implementation of writing SN to FLASH OTP memory is provided here */
    print(mrl, "\tS/N save done"_ENDLINE_SEQ);
}

/**
 * \brief           Command execute callback for MicroRL library, used after log in
 *
 * Do what you want here, but don't write to argv!!! read only!!
 *
 * \param[in]       mrl: \ref microrl_t working instance
 * \param[in]       argc: argument count
 * \param[in]       argv: pointer array to token string
 * \return          '0' on success, '1' otherwise
 */
#if MICRORL_CFG_USE_ECHO_OFF
int execute_main(microrl_t* mrl, int argc, const char* const *argv) {
#else
int execute(microrl_t* mrl, int argc, const char* const *argv) {
#endif /* MICRORL_CFG_USE_ECHO_OFF || __DOXYGEN__ */
    size_t i = 0;

    /* Just iterate through argv word and compare it with your commands */
    while (i < argc) {
        if (strcmp(argv[i], _CMD_HELP) == 0) {
            print_help(mrl);
            return 0;
        } else if (strcmp(argv[i], _CMD_CLEAR) == 0) {
            clear_screen(mrl);
            return 0;
        } else if (strcmp(argv[i], _CMD_SERNUM) == 0) {
            if (++i < argc) {
                if (strcmp(argv[i], _SCMD_RD) == 0) {
                    read_sernum(mrl);
                } else if (strcmp(argv[i], _SCMD_SAVE) == 0) {
                    save_sernum(mrl);
                } else {
                    set_sernum(mrl, (char*)argv[i]);
                }
                return 0;
            } else {
                print(mrl, "Read or specify serial number, use Tab"_ENDLINE_SEQ);
                return 1;
            }
        } else {
            print(mrl, "\tCommand: '");
            print(mrl, (char*)argv[i]);
            print(mrl, "' not found."_ENDLINE_SEQ);
            return 1;
        }
        ++i;
    }

    return 0;
}

#if MICRORL_CFG_USE_ECHO_OFF || __DOXYGEN__
/**
 * \brief           Log in execute callback for MicroRL library
 *
 * Do what you want here, but don't write to argv!!! read only!!
 *
 * \param[in]       mrl: \ref microrl_t working instance
 * \param[in]       argc: argument count
 * \param[in]       argv: pointer array to token string
 * \return          '0' on success, '1' otherwise
 */
int execute(microrl_t* mrl, int argc, const char* const *argv) {
    size_t i = 0;

    /* Just iterate through argv word and compare it with your commands */
    while (i < argc) {
        if (strcmp(argv[i], "login") == 0) {
            if (++i < argc) {
                if (strcmp (argv[i], SESSION_ADMIN_LOGIN) == 0) {
                    print(mrl, "\tEnter your password:"_ENDLINE_SEQ);
                    microrl_set_echo(mrl, MICRORL_ECHO_ONCE);
                    passw_in = 1;
                    return 0;
                } else {
                    print(mrl, "\tWrong login name. Try again."_ENDLINE_SEQ);
                    return 1;
                }
            } else {
                print(mrl, "\tEnter your login after 'login' command."_ENDLINE_SEQ);
                return 0;
            }
        } else if (passw_in == 1) {
            if (strcmp(argv[i], SESSION_ADMIN_PASSW) == 0) {
                print(mrl, "\tSuccess! You are logged in"_ENDLINE_SEQ);
                passw_in = 0;
                logged_in = 1;
                microrl_set_execute_callback(mrl, execute_main);
                return 0;
            } else {
                print(mrl, "\tWrong password. Try log in again."_ENDLINE_SEQ);
                passw_in = 0;
                return 1;
            }
        } else if (strcmp(argv[i], _CMD_HELP) == 0) {
            print_help(mrl);
        } else {
            print(mrl, "\tType 'help' to list commands"_ENDLINE_SEQ);
            return 1;
        }
        ++i;
    }

    return 0;
}
#endif /* MICRORL_CFG_USE_ECHO_OFF || __DOXYGEN__ */

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
        for (int i = 0; i < _NUM_OF_CMD; ++i) {
            /* If token is matched (text is part of our token starting from 0 char) */
            if (strstr(keyword[i], bit) == keyword[i]) {
                /* Add it to completion set */
                compl_word[j++] = keyword[i];
            }
        }
    }  else if ((argc > 1) && (strcmp(argv[0], _CMD_SERNUM) == 0)) {   /* If command needs subcommands */
        /* Iterate through subcommand */
        for (int i = 0; i < _NUM_OF_SETCLEAR_SCMD; ++i) {
            if (strstr(read_save_key[i], argv[argc - 1]) == read_save_key[i]) {
                compl_word[j++] = read_save_key[i];
            }
        }
    } else {    /* If there is no token in cmdline, just print all available token */
        for (; j < _NUM_OF_CMD; ++j) {
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
    MICRORL_UNUSED(argc);
    MICRORL_UNUSED(argv);

    if (res != 0) {
        char str[11] = {0};
#if MICRORL_CFG_USE_LIBC_STDIO
        snprintf(str, 11, "%d", res);
#else
        u32_to_str((uint32_t*)&res, str);
#endif /* MICRORL_CFG_USE_LIBC_STDIO */
        print(mrl, "Command exited with status ");
        print(mrl, str);
        print(mrl, _ENDLINE_SEQ);
    }
}
#endif /* MICRORL_CFG_USE_COMMAND_HOOKS */
