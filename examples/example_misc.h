/**
 * \file            example_misc.h
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

#ifndef MICRORL_EXAMPLE_MISC_HDR_H
#define MICRORL_EXAMPLE_MISC_HDR_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

void   init(void);
int    print(microrl_t* mrl, const char* str);
char   get_char(void);
int    execute(microrl_t* mrl, int argc, const char* const *argv);
char** complete(microrl_t* mrl, int argc, const char* const *argv);
void   sigint(microrl_t* mrl);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* MICRORL_EXAMPLE_MISC_HDR_H */
