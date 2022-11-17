/**
 * \file            microrl_user_config.h
 * \brief           MicroRL library user configurations
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
 * Authors:         Dmitry KARASEV <karasevsdmitry@yandex.ru>
 * Version:         2.5.0-dev
 */

#ifndef MICRORL_HDR_USER_CONFIG_H
#define MICRORL_HDR_USER_CONFIG_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/*
 * Open "microrl_config.h" and copy & replace
 * here settings you want to change values
 */
#define MICRORL_CFG_CMDLINE_LEN               40
#define MICRORL_CFG_CMD_TOKEN_NMB             5
#define MICRORL_CFG_PROMPT_STRING             "> "
#define MICRORL_CFG_PROMPT_COLOR              "\033[32m"
#define MICRORL_CFG_USE_COMPLETE              1
#define MICRORL_CFG_USE_QUOTING               1
#define MICRORL_CFG_USE_ECHO_OFF              1
#define MICRORL_CFG_ECHO_OFF_MASK             '*'
#define MICRORL_CFG_USE_HISTORY               1
#define MICRORL_CFG_RING_HISTORY_LEN          64
#define MICRORL_CFG_PRINT_BUFFER_LEN          40
#define MICRORL_CFG_USE_ESC_SEQ               1
#define MICRORL_CFG_USE_LIBC_STDIO            0
#define MICRORL_CFG_USE_CARRIAGE_RETURN       1
#define MICRORL_CFG_USE_CTRL_C                1
#define MICRORL_CFG_PROMPT_ON_INIT            1
#define MICRORL_CFG_END_LINE                  "\r\n"

#define MICRORL_CFG_USE_COMMAND_HOOKS         1
#define MICRORL_PRE_COMMAND_HOOK(mrl, argc, argv)             /* Igrnore before command execution hook */
#define MICRORL_POST_COMMAND_HOOK(mrl, res, argc, argv)       do {                                                                      \
                                                                  extern void post_exec_hook(microrl_t*, int, int, const char* const *);\
                                                                  post_exec_hook((mrl), (res), (argc), (argv));                         \
                                                              } while (0)

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif  /* MICRORL_HDR_USER_CONFIG_H */
