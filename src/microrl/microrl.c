/**
 * \file            microrl.c
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
 * Version:         2.3.0-dev
 */

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "microrl.h"
#if MICRORL_CFG_USE_LIBC_STDIO
#include <stdio.h>
#endif /* MICRORL_CFG_USE_LIBC_STDIO */

/* GNU Compiler */
#if defined(__GNUC__)
#define __INLINE__                          inline
/* IAR Compiler */
#elif defined(__ICCARM__)
#define __INLINE__                          inline
/* TI Arm Compiler */
#elif defined ( __TI_ARM__ )
#define __INLINE__                          inline
/* Arm Compiler 4/5 */
#elif defined(__CC_ARM)
#define __INLINE__                          __inline
/* Arm Compiler 6.6 LTM (armclang) */
#elif defined (__ARMCC_VERSION) && (__ARMCC_VERSION >= 6010050) && (__ARMCC_VERSION < 6100100)
#define __INLINE__                          __inline
/* Arm Compiler above 6.10.1 (armclang) */
#elif defined (__ARMCC_VERSION) && (__ARMCC_VERSION >= 6100100)
#define __INLINE__                          __inline
#else
#define __INLINE__
#warning "Unknown compiler. 'inline' keyword is not set"
#endif /* defined(__GNUC__) */

#define IS_CONTROL_CHAR(x)                  ((x) <= 31)

/**
 * \brief           List of ANSI escape codes
 */
typedef enum {
    MICRORL_ESQ_ANSI_NUL = 0x00,                    /*!< ^@ Null character */
    MICRORL_ESQ_ANSI_SOH = 0x01,                    /*!< ^A Start of heading, = console interrupt */
    MICRORL_ESQ_ANSI_STX = 0x02,                    /*!< ^B Start of text, maintenance mode on HP console */
    MICRORL_ESQ_ANSI_ETX = 0x03,                    /*!< ^C End of text */
    MICRORL_ESQ_ANSI_EOT = 0x04,                    /*!< ^D End of transmission, not the same as ETB */
    MICRORL_ESQ_ANSI_ENQ = 0x05,                    /*!< ^E Enquiry, goes with ACK; old HP flow control */
    MICRORL_ESQ_ANSI_ACK = 0x06,                    /*!< ^F Acknowledge, clears ENQ logon hand */
    MICRORL_ESQ_ANSI_BEL = 0x07,                    /*!< ^G Bell, rings the bell... */
    MICRORL_ESQ_ANSI_BS = 0x08,                     /*!< ^H Backspace, works on HP terminals/computers */
    MICRORL_ESQ_ANSI_HT = 0x09,                     /*!< ^I Horizontal tab, move to next tab stop */
    MICRORL_ESQ_ANSI_LF = 0x0A,                     /*!< ^J Line Feed */
    MICRORL_ESQ_ANSI_VT = 0x0B,                     /*!< ^K Vertical tab */
    MICRORL_ESQ_ANSI_FF = 0x0C,                     /*!< ^L Form Feed, page eject */
    MICRORL_ESQ_ANSI_CR = 0x0D,                     /*!< ^M Carriage Return */
    MICRORL_ESQ_ANSI_SO = 0x0E,                     /*!< ^N Shift Out, alternate character set */
    MICRORL_ESQ_ANSI_SI = 0x0F,                     /*!< ^O Shift In, resume defaultn character set */
    MICRORL_ESQ_ANSI_DLE = 0x10,                    /*!< ^P Data link escape */
    MICRORL_ESQ_ANSI_DC1 = 0x11,                    /*!< ^Q XON, with XOFF to pause listings; "okay to send". */
    MICRORL_ESQ_ANSI_DC2 = 0x12,                    /*!< ^R Device control 2, block-mode flow control */
    MICRORL_ESQ_ANSI_DC3 = 0x13,                    /*!< ^S XOFF, with XON is TERM=18 flow control */
    MICRORL_ESQ_ANSI_DC4 = 0x14,                    /*!< ^T Device control 4 */
    MICRORL_ESQ_ANSI_NAK = 0x15,                    /*!< ^U Negative acknowledge */
    MICRORL_ESQ_ANSI_SYN = 0x16,                    /*!< ^V Synchronous idle */
    MICRORL_ESQ_ANSI_ETB = 0x17,                    /*!< ^W End transmission block, not the same as EOT */
    MICRORL_ESQ_ANSI_CAN = 0x18,                    /*!< ^X Cancel line, MPE echoes !!! */
    MICRORL_ESQ_ANSI_EM = 0x19,                     /*!< ^Y End of medium, Control-Y interrupt */
    MICRORL_ESQ_ANSI_SUB = 0x1A,                    /*!< ^Z Substitute */
    MICRORL_ESQ_ANSI_ESC = 0x1B,                    /*!< ^[ Escape, next character is not echoed */
    MICRORL_ESQ_ANSI_FS = 0x1C,                     /*!< ^\ File separator */
    MICRORL_ESQ_ANSI_GS = 0x1D,                     /*!< ^] Group separator */
    MICRORL_ESQ_ANSI_RS = 0x1E,                     /*!< ^^ Record separator, block-mode terminator */
    MICRORL_ESQ_ANSI_US = 0x1F,                     /*!< ^_ Unit separator */

    MICRORL_ESQ_ANSI_DEL = 0x7F                     /*!< Delete (not a real control character...) */
} microrl_esq_ansi_t;

/**
 * \brief           History ring buffer memory status
 */
typedef enum {
    MICRORL_HIST_FULL = 0,                          /*!< History ring buffer is full */
    MICRORL_HIST_NOT_FULL                           /*!< History ring buffer is not full or empty */
} microrl_hist_status_t;

/**
 * \brief           Direction of history navigation
 */
typedef enum {
    MICRORL_HIST_DIR_UP = 0,                        /*!< Previous record in history ring buffer */
    MICRORL_HIST_DIR_DOWN                           /*!< Next record in history ring buffer */
} microrl_hist_dir_t;

/**
 * \brief           Split command line to tokens array
 * \param[in]       mrl: \ref microrl_t working instance
 * \param[out]      tkn_arr: Tokens buffer stored split words
 * \param[out]      tkn_count: Number of split tokens
 * \param[in]       limit: Number of command line characters to split
 * \return          \ref microrlOK on success, member of \ref microrlr_t enumeration otherwise
 */
static microrlr_t prv_cmdline_buf_split(microrl_t* mrl, const char** tkn_arr, uint8_t* tkn_count, size_t limit) {
    uint8_t num = 0;
    char *str = mrl->cmdline;

    /* Process complete string */
    while (*str != '\0') {
        while (*str == ' ' && ++str) {}         /* Remove leading whitespaces */
        if (*str == '\0') {
            break;
        }

#if MICRORL_CFG_USE_QUOTING
        if (*str == '"' || *str == '\'') {      /* Check if it starts with quote to handle escapes */
            ++str;
            tkn_arr[num++] = str;               /* Set start of argument after quotes */

            while (*str != '\0') {              /* Process until end of quote */
                if (!((size_t)(str - mrl->cmdline) < limit)) {
                    tkn_arr[--num] = NULL;
                    *tkn_count = num;
                    return microrlOK;
                }

                if (*str == '\\') {
                    ++str;
                    if (*str == '"' || *str == '\'') {
                        ++str;
                    }
                } else if (*str == '"' || *str == '\'') {
                    *str = '\0';
                    ++str;
                    break;
                } else {
                    ++str;
                }
            }
        } else {
#endif /* MICRORL_CFG_USE_QUOTING */
            tkn_arr[num++] = str;               /* Set start of argument directly on character */
            while ((*str != ' ' && *str != '\0')) {
                if (!((size_t)(str - mrl->cmdline) < limit)) {
                    tkn_arr[--num] = NULL;
                    *tkn_count = num;
                    return microrlOK;
                }
#if MICRORL_CFG_USE_QUOTING
                if (*str == '"' || *str == '\'') {   /* Quote should not be here... */
                    *str = '\0';                /* ...add NULL termination to end token */
                }
#endif /* MICRORL_CFG_USE_QUOTING */
                ++str;
            }
            if (*str == '\0') {
                break;
            }
            *str = '\0';
            ++str;
#if MICRORL_CFG_USE_QUOTING
        }
#endif /* MICRORL_CFG_USE_QUOTING */
        if (num == MICRORL_CFG_CMD_TOKEN_NMB && *str != '\0') {     /* Check for number of tokens */
            return microrlERRTKNNUM;
        }
    }

    *tkn_count = num;
    return microrlOK;
}

/**
 * \brief           Insert the passed text at the cursor position
 * \param[in,out]   mrl: \ref microrl_t working instance
 * \param[in]       text: Text to store on the command line
 * \param[in]       len: Length of text to store
 * \return          \ref microrlOK on success, member of \ref microrlr_t enumeration otherwise
 */
static microrlr_t prv_cmdline_buf_insert_text(microrl_t* mrl, const char* text, size_t len) {
    if ((mrl->cmdlen + len) <= (MICRORL_ARRAYSIZE(mrl->cmdline) - 1)) {
#if MICRORL_CFG_USE_ECHO_OFF
        if ((mrl->echo != MICRORL_ECHO_ON) && (mrl->echo_off_pos == -1)) {
            mrl->echo_off_pos = mrl->cmdlen;
        }
#endif /* #if MICRORL_CFG_USE_ECHO_OFF */
        memmove(mrl->cmdline + mrl->cursor + len,
                mrl->cmdline + mrl->cursor,
                mrl->cmdlen - mrl->cursor);
        for (size_t i = 0; i < len; ++i) {
            mrl->cmdline[mrl->cursor + i] = text[i];
        }
        mrl->cursor += len;
        mrl->cmdlen += len;
        memset(&mrl->cmdline[mrl->cmdlen], 0x00, MICRORL_ARRAYSIZE(mrl->cmdline) - 1 - mrl->cmdlen);

        return microrlOK;
    }

    return microrlERRCLFULL;
}

/**
 * \brief           Remove len characters backwards at cursor
 * \param[in,out]   mrl: \ref microrl_t working instance
 * \param[in]       len: Number of chars to remove
 */
static void prv_cmdline_buf_backspace(microrl_t* mrl, size_t len) {
    if (mrl->cursor >= len) {
        memmove(mrl->cmdline + mrl->cursor - len,
                mrl->cmdline + mrl->cursor,
                mrl->cmdlen - mrl->cursor + len);
        mrl->cursor -= len;
        mrl->cmdline[mrl->cmdlen] = '\0';
        mrl->cmdlen -= len;
    }
}

/**
 * \brief           Remove one characters forward at cursor
 * \param[in,out]   mrl: \ref microrl_t working instance
 */
static void prv_cmdline_buf_delete(microrl_t* mrl) {
    if ((mrl->cmdlen > 0) && (mrl->cursor != mrl->cmdlen)) {
        memmove(mrl->cmdline + mrl->cursor,
                mrl->cmdline + mrl->cursor + 1,
                mrl->cmdlen - mrl->cursor + 1);
        mrl->cmdline[mrl->cmdlen] = '\0';
        --mrl->cmdlen;
    }
}

/**
 * \brief           Reset command line buffer and its position pointers
 * \param[out]      mrl: \ref microrl_t working instance
 */
static __INLINE__ void prv_cmdline_buf_reset(microrl_t* mrl) {
    memset(mrl->cmdline, 0x00, sizeof(mrl->cmdline));
    mrl->cmdlen = 0;
    mrl->cursor = 0;
}

/**
 * \brief           Print prompt string in terminal
 * \param[in]       mrl: \ref microrl_t working instance
 */
static __INLINE__ void prv_terminal_print_prompt(microrl_t* mrl) {
    mrl->out_fn(mrl, MICRORL_CFG_PROMPT_COLOR);
    mrl->out_fn(mrl, mrl->prompt);
    mrl->out_fn(mrl, MICRORL_COLOR_DEFAULT);
}

/**
 * \brief           Clear the last character in the terminal command line
 *                      and move the cursor to its position
 * \param[in]       mrl: \ref microrl_t working instance
 */
static __INLINE__ void prv_terminal_backspace(microrl_t* mrl) {
    mrl->out_fn(mrl, "\033[D \033[D");
}

/**
 * \brief           Print end line symbol defined in \ref MICRORL_CFG_END_LINE config
 * \param[in]       mrl: \ref microrl_t working instance
 */
static __INLINE__ void prv_terminal_newline(microrl_t* mrl) {
    mrl->out_fn(mrl, MICRORL_CFG_END_LINE);
}

/**
 * \brief           Insert ESQ sequence into the passed string to set the cursor
 *                      at the current position + offset (positive or negative)
 *                      in in the terminal command line.
 *                      The passed string must be at least 7 bytes long
 * \param[in]       str: The original string before moving the cursor
 * \param[in]       offset: Positive or negative interval to move cursor
 * \return          The original string after moving the cursor
 */
static char* prv_cursor_generate_move(char* str, int32_t offset) {
    char c = 'C';
    if (offset > 999) {
        offset = 999;
    }
    if (offset < -999) {
        offset = -999;
    }
    if (offset < 0) {
        offset = -offset;
        c = 'D';
    } else if (offset == 0) {
        *str = '\0';
        return str;
    }

#if MICRORL_CFG_USE_LIBC_STDIO
    str += sprintf(str, "\033[%d%c", offset, c);
#else
    *str++ = '\033';
    *str++ = '[';
    char tmp_str[4] = {0};
    size_t i = 0;
    size_t j;
    while (offset > 0) {
        tmp_str[i++] = (offset % 10) + '0';
        offset /= 10;
    }
    for (j = 0; j < i; ++j) {
        *str++ = tmp_str[i - j - 1];
    }
    *str++ = c;
    *str = '\0';
#endif /* MICRORL_CFG_USE_LIBC_STDIO */
    return str;
}

/**
 * \brief           Set cursor at current position + offset (positive or negative)
 *                      in terminal command line
 * \param[in]       mrl: \ref microrl_t working instance
 * \param[in]       offset: Positive or negative interval to move cursor
 */
static void prv_terminal_move_cursor(microrl_t* mrl, int32_t offset) {
    char str[16] = {0};
    if (offset != 0) {
        prv_cursor_generate_move(str, offset);
        mrl->out_fn(mrl, str);
    }
}

/**
 * \brief           Print command line buffer to specified position
 * \param[in]       mrl: \ref microrl_t working instance
 * \param[in]       pos: Start position from which the line will be printed
 * \param[in]       reset: Flag to reset the cursor position
 */
static void prv_terminal_print_line(microrl_t* mrl, int32_t pos, uint8_t reset) {
    char str[MICRORL_CFG_PRINT_BUFFER_LEN] = {0};
    char* j = str;

    if (reset) {
#if MICRORL_CFG_USE_CARRIAGE_RETURN
        *j++ = '\r';
        j = prv_cursor_generate_move(j, strlen(mrl->prompt) + pos);
#else
        j = prv_cursor_generate_move(j, -(MICRORL_ARRAYSIZE(mrl->cmdline) - 1 + strlen(mrl->prompt) + 2));
        j = prv_cursor_generate_move(j, strlen(mrl->prompt) + pos);
#endif /* MICRORL_CFG_USE_CARRIAGE_RETURN */
    }

    for (size_t i = pos; i < mrl->cmdlen; ++i) {
        *j = mrl->cmdline[i];

#if MICRORL_CFG_USE_ECHO_OFF
        if (((int32_t)i >= mrl->echo_off_pos) && (mrl->echo != MICRORL_ECHO_ON)) {
            *j = MICRORL_CFG_ECHO_OFF_MASK;
        }
#endif /* MICRORL_CFG_USE_ECHO_OFF */

        ++j;

        if ((size_t)(j - str) == strlen(str)) {
            *j = '\0';
            mrl->out_fn(mrl, str);
            j = str;
        }
    }

    if ((size_t)(j - str + 3 + 6 + 1) > MICRORL_ARRAYSIZE(str)) {
        *j = '\0';
        mrl->out_fn(mrl, str);
        j = str;
    }

    *j++ = '\033';                              /* Delete all past end of text */
    *j++ = '[';
    *j++ = 'K';
    prv_cursor_generate_move(j, mrl->cursor - mrl->cmdlen);
    mrl->out_fn(mrl, str);
}

#if MICRORL_CFG_USE_HISTORY || __DOXYGEN__

/**
 * \brief           Set the next record start position to the passed index
 * \param[in]       prbuf: Pointer to \ref microrl_hist_rbuf_t structure
 * \param[in,out]   index: Pointer to the current record
 */
static __INLINE__ void prv_hist_next_record(microrl_hist_rbuf_t* prbuf, size_t* index) {
    while (prbuf->ring_buf[*index] != '\0') {
        ++(*index);
        if (*index >= MICRORL_ARRAYSIZE(prbuf->ring_buf)) {
            *index -= MICRORL_ARRAYSIZE(prbuf->ring_buf);
        }
    }
    ++(*index);
}

/**
 * \brief           Remove older record from ring buffer
 * \param[in,out]   prbuf: Pointer to \ref microrl_hist_rbuf_t structure
 */
static void prv_hist_erase_older(microrl_hist_rbuf_t* prbuf) {
    size_t new_pos = prbuf->head;
    prv_hist_next_record(prbuf, &new_pos);
    prbuf->head = new_pos;

    /*
     * If after record erasing the head has moved further than the tail,
     * then the history is empty, so it is needed to reset the tail
     */
    if (prbuf->head == prbuf->tail + 1) {
        prbuf->tail = prbuf->head;
        prbuf->ring_buf[prbuf->tail] = '\0';
    }
}

/**
 * \brief           Check space in history buffer for new record
 * \param[in]       prbuf: Pointer to \ref microrl_hist_rbuf_t structure
 * \param[in]       len: Length of new record to save in history
 * \return          Member of \ref microrl_hist_status_t enumeration
 */
static microrl_hist_status_t prv_hist_is_space_for_new(microrl_hist_rbuf_t* prbuf, size_t len) {
    if (prbuf->tail >= prbuf->head) {
        if ((MICRORL_ARRAYSIZE(prbuf->ring_buf) - prbuf->tail + prbuf->head - 1) > len) {
            return MICRORL_HIST_NOT_FULL;
        }
    } else {
        if ((prbuf->head - prbuf->tail - 1) > len) {
            return MICRORL_HIST_NOT_FULL;
        }
    }

    return MICRORL_HIST_FULL;
}

/**
 * \brief           Copy saved record to 'line' and return size of record
 * \param[in]       prbuf: Pointer to \ref microrl_hist_rbuf_t structure
 * \param[out]      line: Line to restore from history
 * \param[in]       dir: Record search direction, member of \ref microrl_hist_dir_t
 * \return          Size of restored line. `0` is returned, if history is empty
 */
static size_t prv_hist_restore_line(microrl_hist_rbuf_t* prbuf, char* line, microrl_hist_dir_t dir) {
    size_t cnt = 0;
    size_t i = prbuf->head;
    while (i - 1 != prbuf->tail) {              /* Count history records */
        prv_hist_next_record(prbuf, &i);
        ++cnt;
    }

    size_t ind = prbuf->head;
    size_t j = 0;
    if (dir == MICRORL_HIST_DIR_UP) {           /* Set navigation counter depending on the direction */
        if (cnt >= prbuf->count) {
            if (cnt != prbuf->count) {
                ++prbuf->count;
            }
        } else {
            return 0;                           /* Impossible state, return empty line */
        }
    } else {
        if (prbuf->count > 0) {
            if (--prbuf->count == 0) {
                return 0;                       /* Empty line */
            }
        } else {
            return 0;                           /* Empty line */
        }
    }

    while ((cnt - j++) != prbuf->count) {       /* Find record for 'prbuf->count' counter */
        prv_hist_next_record(prbuf, &ind);
    }

    size_t rec_len = 0;
    size_t k = ind;
    while (prbuf->ring_buf[k] != '\0') {        /* Calculating the length of the found record */
        ++k;
        if (k >= MICRORL_ARRAYSIZE(prbuf->ring_buf)) {
            k -= MICRORL_ARRAYSIZE(prbuf->ring_buf);
        }
        ++rec_len;
    }

    memset(line, 0x00, MICRORL_CFG_CMDLINE_LEN);    /* Placing the found record on the command line */
    if ((ind + rec_len) < MICRORL_ARRAYSIZE(prbuf->ring_buf)) {
        memcpy(line, prbuf->ring_buf + ind, rec_len);
    } else {
        size_t part0 = MICRORL_ARRAYSIZE(prbuf->ring_buf) - ind;
        memcpy(line, prbuf->ring_buf + ind, part0);
        memcpy(line + part0, prbuf->ring_buf, rec_len - part0);
    }

    return rec_len;
}

/**
 * \brief           Restore record to command line from history buffer
 * \param[in,out]   mrl: \ref microrl_t working instance
 * \param[in]       dir: Member of \ref microrl_hist_dir_t enumeration
 */
static void prv_hist_search(microrl_t* mrl, microrl_hist_dir_t dir) {
    size_t len = prv_hist_restore_line(&mrl->ring_hist, mrl->cmdline, dir);
    memset(&mrl->cmdline[len], 0x00, MICRORL_ARRAYSIZE(mrl->cmdline) - 1 - len);
    mrl->cursor = mrl->cmdlen = len;
    prv_terminal_print_line(mrl, 0, 1);
}

/**
 * \brief           Put record to ring buffer
 * \param[in,out]   prbuf: Pointer to \ref microrl_hist_rbuf_t structure
 * \param[in]       line: Record to save in history
 * \param[in]       len: Record length
 */
static void prv_hist_save_line(microrl_hist_rbuf_t* prbuf, char* line, size_t len) {
    if (len > (MICRORL_ARRAYSIZE(prbuf->ring_buf) - 1)) {
        return;
    }

    /* Don't save the same line as the last record */
    char last_record[MICRORL_CFG_CMDLINE_LEN + 1];
    prv_hist_restore_line(prbuf, last_record, MICRORL_HIST_DIR_UP);
    if (strcmp(line, last_record) == 0) {
        prbuf->count = 0;
        return;
    }

    while (prv_hist_is_space_for_new(prbuf, len) == MICRORL_HIST_FULL) {    /* Freeing up space for saving in the ring buffer */
        prv_hist_erase_older(prbuf);
    }

    if (len < (MICRORL_ARRAYSIZE(prbuf->ring_buf) - prbuf->tail - 1)) {     /* Store record */
        memcpy(prbuf->ring_buf + prbuf->tail + 1, line, len);
    } else {
        size_t part_len = MICRORL_ARRAYSIZE(prbuf->ring_buf) - prbuf->tail - 1;
        memcpy(prbuf->ring_buf + prbuf->tail + 1, line, part_len);
        memcpy(prbuf->ring_buf, line + part_len, len - part_len);
    }

    if (prbuf->head == prbuf->tail) {           /* Update position pointer and navigation counter */
        ++prbuf->head;
    }
    prbuf->tail = prbuf->tail + len + 1;
    if (prbuf->tail >= MICRORL_ARRAYSIZE(prbuf->ring_buf)) {
        prbuf->tail -= MICRORL_ARRAYSIZE(prbuf->ring_buf);
    }
    prbuf->ring_buf[prbuf->tail] = 0;
    prbuf->count = 0;
}

#endif /* MICRORL_CFG_USE_HISTORY || __DOXYGEN__ */

#if MICRORL_CFG_USE_ESC_SEQ || __DOXYGEN__
/**
 * \brief           Handle ANSI escape code sequences
 * \param[in,out]   mrl: \ref microrl_t working instance
 * \param[in]       ch: Input character
 * \return          '1' if full escape sequence is processed, '0' otherwise
 */
static uint8_t prv_escape_process(microrl_t* mrl, char ch) {
    if (ch == '[') {
        mrl->esc_code = MICRORL_ESC_BRACKET;
        return 0;
    } else if (mrl->esc_code == MICRORL_ESC_BRACKET) {
        if (ch == 'A') {
#if MICRORL_CFG_USE_HISTORY

#if MICRORL_CFG_USE_ECHO_OFF
            if (mrl->echo != MICRORL_ECHO_ON) {
                return 1;
            }
#endif /* MICRORL_CFG_USE_ECHO_OFF */

            prv_hist_search(mrl, MICRORL_HIST_DIR_UP);

#endif /* MICRORL_CFG_USE_HISTORY */

            return 1;
        } else if (ch == 'B') {
#if MICRORL_CFG_USE_HISTORY

#if MICRORL_CFG_USE_ECHO_OFF
            if (mrl->echo != MICRORL_ECHO_ON) {
                return 1;
            }
#endif /* MICRORL_CFG_USE_ECHO_OFF */

            prv_hist_search(mrl, MICRORL_HIST_DIR_DOWN);

#endif /* MICRORL_CFG_USE_HISTORY */

            return 1;
        } else if (ch == 'C') {
            if (mrl->cursor < mrl->cmdlen) {
                prv_terminal_move_cursor(mrl, 1);
                ++mrl->cursor;
            }
            return 1;
        } else if (ch == 'D') {
            if (mrl->cursor > 0) {
                prv_terminal_move_cursor(mrl, -1);
                --mrl->cursor;
            }
            return 1;
        } else if (ch == '7') {
            mrl->esc_code = MICRORL_ESC_HOME;
            return 0;
        } else if (ch == '8') {
            mrl->esc_code = MICRORL_ESC_END;
            return 0;
        }
    } else if (ch == '~') {
        if (mrl->esc_code == MICRORL_ESC_HOME) {
            prv_terminal_move_cursor(mrl, -mrl->cursor);
            mrl->cursor = 0;
            return 1;
        } else if (mrl->esc_code == MICRORL_ESC_END) {
            prv_terminal_move_cursor(mrl, mrl->cmdlen - mrl->cursor);
            mrl->cursor = mrl->cmdlen;
            return 1;
        }
    }

    return 1;                                   /* Unknown escape sequence, stop processing */
}
#endif /* MICRORL_CFG_USE_ESC_SEQ || __DOXYGEN__ */

/**
 * \brief           Processing input string from command line and calling execute_fn() callback
 * \param[in,out]   mrl: \ref microrl_t working instance
 * \return          \ref microrlOK on success, member of \ref microrlr_t enumeration otherwise
 */
static microrlr_t prv_handle_newline(microrl_t* mrl) {
    const char* tkn_arr[MICRORL_CFG_CMD_TOKEN_NMB] = {0};
    uint8_t tkn_count = 0;
    microrlr_t status = microrlOK;

    prv_terminal_newline(mrl);

#if MICRORL_CFG_USE_HISTORY
    if (mrl->cmdlen > 0) {
#if MICRORL_CFG_USE_ECHO_OFF
        if (mrl->echo == MICRORL_ECHO_ON) {
#endif /* MICRORL_CFG_USE_ECHO_OFF */
            prv_hist_save_line(&mrl->ring_hist, mrl->cmdline, mrl->cmdlen);
#if MICRORL_CFG_USE_ECHO_OFF
        }
#endif /* MICRORL_CFG_USE_ECHO_OFF */
    }
#endif /* MICRORL_CFG_USE_HISTORY */

#if MICRORL_CFG_USE_ECHO_OFF
    if (mrl->echo == MICRORL_ECHO_ONCE && mrl->cmdline[mrl->echo_off_pos] != '\0') {
        microrl_set_echo(mrl, MICRORL_ECHO_ON);
        mrl->echo_off_pos = -1;
    }
#endif /* MICRORL_CFG_USE_ECHO_OFF */

    status = prv_cmdline_buf_split(mrl, tkn_arr, &tkn_count, mrl->cmdlen);
    if (status == microrlOK) {
#if MICRORL_CFG_USE_COMMAND_HOOKS
        int exec_status = 0;

        MICRORL_PRE_COMMAND_HOOK(mrl, tkn_count, tkn_arr);

        exec_status = mrl->exec_fn(mrl, tkn_count, tkn_arr);

        MICRORL_POST_COMMAND_HOOK(mrl, exec_status, tkn_count, tkn_arr);
#else
        mrl->exec_fn(mrl, tkn_count, tkn_arr);
#endif /* MICRORL_CFG_USE_COMMAND_HOOKS */
    } else {
        mrl->out_fn(mrl, "ERROR: too many tokens");
        prv_terminal_newline(mrl);
    }

    prv_terminal_print_prompt(mrl);
    prv_cmdline_buf_reset(mrl);

    return status;
}

#if MICRORL_CFG_USE_COMPLETE || __DOXYGEN__

/**
 * \brief           Calculate total length of all completion tokens
 * \param[in]       arr: Completion tokens array
 * \return          Total length of all completion tokens
 */
static size_t prv_complite_total_len(char** arr) {
    size_t i;
    char* shortest = arr[0];
    size_t shortlen = strlen(shortest);

    for (i = 0; arr[i] != NULL; ++i) {
        if (strlen(arr[i]) < shortlen) {
            shortest = arr[i];
            shortlen = strlen(shortest);
        }
    }

    for (i = 0; i < shortlen; ++i) {
        for (size_t j = 0; arr[j] != 0; ++j) {
            if (shortest[i] != arr[j][i]) {
                return i;
            }
        }
    }

    return i;
}

/**
 * \brief           Auto-complete activities to complete input in
 *                      command line
 * \param[in,out]   mrl: \ref microrl_t working instance
 * \return          \ref microrlOK on success, member of \ref microrlr_t enumeration otherwise
 */
static microrlr_t prv_microrl_complite_get_input(microrl_t* mrl) {
    uint8_t tkn_count = 0;
    const char* tkn_arr[MICRORL_CFG_CMD_TOKEN_NMB] = {0};
    char** compl_token;

    if (prv_cmdline_buf_split(mrl, tkn_arr, &tkn_count, mrl->cursor) != microrlOK) {
        return microrlERRCPLT;
    }

    if (mrl->cmdline[mrl->cursor - 1] == '\0') {
        tkn_arr[tkn_count++] = "";
    }
    compl_token = mrl->get_completion_fn(mrl, tkn_count, tkn_arr);
    if (compl_token[0] != NULL) {
        size_t i = 0;
        size_t len;
        size_t pos = mrl->cursor;

        if (compl_token[1] == NULL) {
            len = strlen(compl_token[0]);
        } else {
            len = prv_complite_total_len(compl_token);
            prv_terminal_newline(mrl);
            while (compl_token[i] != NULL) {
                mrl->out_fn(mrl, compl_token[i]);
                mrl->out_fn(mrl, " ");
                ++i;
            }
            prv_terminal_newline(mrl);
            prv_terminal_print_prompt(mrl);
            pos = 0;
        }

        if (len != 0) {
            prv_cmdline_buf_insert_text(mrl, compl_token[0] + strlen(tkn_arr[tkn_count - 1]),
                                        len - strlen(tkn_arr[tkn_count - 1]));
        }

        /* Insert end space if completion is performed */
        if (compl_token[1] == NULL) {
            prv_cmdline_buf_insert_text(mrl, " ", 1);
        }

        /* Restore whitespaces replaced with '0' when command line buffer was split */
        if (tkn_count != 0) {
            for (size_t i = 0; i < (size_t)(tkn_count - 1); ++i) {
                memset((char*)tkn_arr[i] + strlen(tkn_arr[i]), ' ', 1);
            }
        }

        prv_terminal_print_line(mrl, pos, 0);
    } else {
        return microrlERRCPLT;
    }

    return microrlOK;
}

#endif /* MICRORL_CFG_USE_COMPLETE || __DOXYGEN__ */

/**
 * \brief           Initialize microRL instance
 * \param[in,out]   mrl: microRL working instance
 * \param[in]       out_fn: Output function used for print operation
 * \brief           exec_fn: Command execute callback function
 * \return          \ref microrlOK on success, member of \ref microrlr_t enumeration otherwise
 */
microrlr_t microrl_init(microrl_t* mrl, microrl_output_fn out_fn, microrl_exec_fn exec_fn) {
    if (mrl == NULL || out_fn == NULL || exec_fn == NULL) {
        return microrlERRPAR;
    }

    memset(mrl, 0x00, sizeof(microrl_t));
    mrl->out_fn = out_fn;
    mrl->exec_fn = exec_fn;
    mrl->prompt = MICRORL_CFG_PROMPT_STRING;

#if MICRORL_CFG_PROMPT_ON_INIT
    prv_terminal_print_prompt(mrl);
#endif /* MICRORL_CFG_PROMPT_ON_INIT */

#if MICRORL_CFG_USE_ECHO_OFF
    mrl->echo = MICRORL_ECHO_ON;
    mrl->echo_off_pos = -1;
#endif /* MICRORL_CFG_USE_ECHO_OFF */

    return microrlOK;
}

/**
 * \brief           Set pointer to command execute callback, that called when user press 'Enter'
 * \param[in,out]   mrl: \ref microrl_t working instance
 * \param[in]       exec_fn: Command execute callback
 * \return          \ref microrlOK on success, member of \ref microrlr_t enumeration otherwise
 */
microrlr_t microrl_set_execute_callback(microrl_t* mrl, microrl_exec_fn exec_fn) {
    if (mrl == NULL || exec_fn == NULL) {
        return microrlERRPAR;
    }

    mrl->exec_fn = exec_fn;

    return microrlOK;
}

#if MICRORL_CFG_USE_COMPLETE || __DOXYGEN__
/**
 * \brief           Set pointer to input complition callback, that called when user press 'Tab'
 * \param[in,out]   mrl: \ref microrl_t working instance
 * \param[in]       get_completion: Auto-complete input string callback
 * \return          \ref microrlOK on success, member of \ref microrlr_t enumeration otherwise
 */
microrlr_t microrl_set_complete_callback(microrl_t* mrl, microrl_get_compl_fn get_completion) {
    if (mrl == NULL || get_completion == NULL) {
        return microrlERRPAR;
    }

    mrl->get_completion_fn = get_completion;

    return microrlOK;
}
#endif /* MICRORL_CFG_USE_COMPLETE || __DOXYGEN__ */

#if MICRORL_CFG_USE_CTRL_C || __DOXYGEN__
/**
 * \brief           Set callback for Ctrl+C terminal signal
 * \param[in,out]   mrl: \ref microrl_t working instance
 * \param[in]       sigint: Ctrl+C terminal signal callback
 * \return          \ref microrlOK on success, member of \ref microrlr_t enumeration otherwise
 */
microrlr_t microrl_set_sigint_callback(microrl_t* mrl, microrl_sigint_fn sigint) {
    if (mrl == NULL || sigint == NULL) {
        return microrlERRPAR;
    }

    mrl->sigint_fn = sigint;

    return microrlOK;
}
#endif /* MICRORL_CFG_USE_CTRL_C || __DOXYGEN__ */

/**
 * \brief           Set prompt string
 * \param[in,out]   mrl: \ref microrl_t working instance
 * \param[in]       prompt: pointer to prompt string to set
 * \return          \ref microrlOK on success, member of \ref microrlr_t enumeration otherwise
 */
microrlr_t  microrl_set_prompt(microrl_t* mrl, char* prompt) {
    if (mrl == NULL || prompt == NULL) {
        return microrlERRPAR;
    }

    mrl->prompt = prompt;
    
    return microrlOK;
}

#if MICRORL_CFG_USE_ECHO_OFF || __DOXYGEN__
/**
 * \brief           Set echo mode used to mask user input
 *
 * Use \ref MICRORL_ECHO_ONCE to disable echo for password input, echo mode will enabled after user press 'Enter'.
 * Use \ref MICRORL_ECHO_ON or \ref MICRORL_ECHO_OFF to turn on or off the echo manually.
 *
 * \param[in,out]   mrl: \ref microrl_t working instance
 * \param[in]       echo: Member of \ref microrl_echo_t enumeration
 * \return          \ref microrlOK on success, member of \ref microrlr_t enumeration otherwise
 */
microrlr_t microrl_set_echo(microrl_t* mrl, microrl_echo_t echo) {
    if (mrl == NULL) {
        return microrlERRPAR;
    }

    mrl->echo = echo;

    return microrlOK;
}
#endif /* MICRORL_CFG_USE_ECHO_OFF || __DOXYGEN__ */

/**
 * \brief           Processing command line input
 * \param[in]       mrl: \ref microrl_t working instance
 * \param[in]       in_data: Input data to process
 * \param[in]       len: Length of data for input
 * \return          \ref microrlOK on success, member of \ref microrlr_t enumeration otherwise
 */
microrlr_t microrl_processing_input(microrl_t* mrl, const void* in_data, size_t len) {
    if (mrl == NULL || in_data == NULL || len == 0) {
        return microrlERRPAR;
    }

    char* in = (char*)in_data;

    while (len-- != 0) {
        char ch = *in++;

#if MICRORL_CFG_USE_ESC_SEQ
        if (mrl->escape) {
            if (prv_escape_process(mrl, ch)) {
                mrl->escape = 0;
            }
        } else {
#endif /* MICRORL_CFG_USE_ESC_SEQ */
            if ((ch == MICRORL_ESQ_ANSI_CR) || (ch == MICRORL_ESQ_ANSI_LF)) {
                /*
                 * Only trigger a newline if `ch` doen't follow its companion's
                 * triggering a newline.
                 */
                if (mrl->last_endl == (ch == MICRORL_ESQ_ANSI_CR ? MICRORL_ESQ_ANSI_LF : MICRORL_ESQ_ANSI_CR)) {
                    mrl->last_endl = 0;         /* Ignore char, but clear newline state */
                } else {
                    mrl->last_endl = ch;
                    if (prv_handle_newline(mrl) != microrlOK) {
                        return microrlERRTKNNUM;
                    }
                }
                continue;
            }
            mrl->last_endl = 0;

            switch(ch) {
                case MICRORL_ESQ_ANSI_HT: {
#if MICRORL_CFG_USE_COMPLETE
                    if (mrl->get_completion_fn == NULL) {
                        return microrlERRPAR;
                    }
                    if (prv_microrl_complite_get_input(mrl) != microrlOK) {
                        return microrlERRCPLT;
                    }
#endif /* MICRORL_CFG_USE_COMPLETE */
                    break;
                }
                case MICRORL_ESQ_ANSI_ESC: {
#if MICRORL_CFG_USE_ESC_SEQ
                  mrl->escape = 1;
#endif /* MICRORL_CFG_USE_ESC_SEQ */
                    break;
                }
                case MICRORL_ESQ_ANSI_NAK: { /* ^U */
                    if (mrl->cursor > 0) {
                        prv_cmdline_buf_backspace(mrl, mrl->cursor);
                    }
                    prv_terminal_print_line(mrl, 0, 1);
                    break;
                }
                case MICRORL_ESQ_ANSI_VT: { /* ^K */
                    mrl->out_fn(mrl, "\033[K");
                    mrl->cmdlen = mrl->cursor;
                    break;
                }
                case MICRORL_ESQ_ANSI_ENQ: { /* ^E */
                    prv_terminal_move_cursor(mrl, mrl->cmdlen - mrl->cursor);
                    mrl->cursor = mrl->cmdlen;
                    break;
                }
                case MICRORL_ESQ_ANSI_SOH: { /* ^A */
                    prv_terminal_move_cursor(mrl, -mrl->cursor);
                    mrl->cursor = 0;
                    break;
                }
                case MICRORL_ESQ_ANSI_ACK: { /* ^F */
                    if (mrl->cursor < mrl->cmdlen) {
                        prv_terminal_move_cursor(mrl, 1);
                        ++mrl->cursor;
                    }
                    break;
                }
                case MICRORL_ESQ_ANSI_STX: { /* ^B */
                    if (mrl->cursor != 0) {
                        prv_terminal_move_cursor(mrl, -1);
                        --mrl->cursor;
                    }
                    break;
                }
                case MICRORL_ESQ_ANSI_DLE: { /* ^P */
#if MICRORL_CFG_USE_HISTORY

#if MICRORL_CFG_USE_ECHO_OFF
                if (mrl->echo != MICRORL_ECHO_ON) {
                    break;
                }
#endif /* MICRORL_CFG_USE_ECHO_OFF */

                prv_hist_search(mrl, MICRORL_HIST_DIR_UP);
#endif /* MICRORL_CFG_USE_HISTORY */

                    break;
                }
                case MICRORL_ESQ_ANSI_SO: { /* ^N */
#if MICRORL_CFG_USE_HISTORY
#if MICRORL_CFG_USE_ECHO_OFF
                if (mrl->echo != MICRORL_ECHO_ON) {
                    break;
                }
#endif /* MICRORL_CFG_USE_ECHO_OFF */

                prv_hist_search(mrl, MICRORL_HIST_DIR_DOWN);
#endif /* MICRORL_CFG_USE_HISTORY */

                    break;
                }
                case MICRORL_ESQ_ANSI_DEL:  /* Backspace */
                case MICRORL_ESQ_ANSI_BS: { /* ^H */
                    if (mrl->cursor > 0) {
                        prv_cmdline_buf_backspace(mrl, 1);
                        if (mrl->cursor == mrl->cmdlen) {
                            prv_terminal_backspace(mrl);
                        } else {
                            prv_terminal_print_line(mrl, mrl->cursor, 1);
                        }
                    }
                    break;
                }
                case MICRORL_ESQ_ANSI_EOT: { /* ^D */
                    prv_cmdline_buf_delete(mrl);
                    prv_terminal_print_line(mrl, mrl->cursor, 0);
                    break;
                }
                case MICRORL_ESQ_ANSI_DC2: { /* ^R */
                    prv_terminal_newline(mrl);
                    prv_terminal_print_prompt(mrl);
                    prv_terminal_print_line(mrl, 0, 0);
                    break;
                }
                case MICRORL_ESQ_ANSI_ETX: {
#if MICRORL_CFG_USE_CTRL_C
                    if (mrl->sigint_fn == NULL) {
                        return microrlERRPAR;
                    }
                    mrl->sigint_fn(mrl);
#endif /* MICRORL_CFG_USE_CTRL_C */
                    break;
                }
                default: {
                    if (((ch == ' ') && (mrl->cmdlen == 0)) || IS_CONTROL_CHAR(ch)) {   /* Skip spaces in first command line symbol or escapes */
                        break;
                    }
                    if (prv_cmdline_buf_insert_text(mrl, &ch, 1) == microrlOK) {
                        if (mrl->cursor == mrl->cmdlen) {
                            char nch[] = {0, 0};
                            nch[0] = ch;
#if MICRORL_CFG_USE_ECHO_OFF
                            if (((int32_t)mrl->cursor >= mrl->echo_off_pos) && (mrl->echo != MICRORL_ECHO_ON)) {
                                nch[0] = MICRORL_CFG_ECHO_OFF_MASK;
                            }
#endif /* MICRORL_CFG_USE_ECHO_OFF */
                            mrl->out_fn(mrl, nch);
                        } else {
                            prv_terminal_print_line(mrl, mrl->cursor - 1, 0);
                        }
                    } else {
                        return microrlERRCLFULL;
                    }
                }
            }
#if MICRORL_CFG_USE_ESC_SEQ
        }
#endif /* MICRORL_CFG_USE_ESC_SEQ */
    }
    
    return microrlOK;
}

/**
 * \brief           Get current version number of the MicroRL library.
 *                      Semantic versioning is used for numbering
 * \see             Semantic versioning: https://semver.org
 * \return          MicroRL library version number combined into an uint32_t:
 *                      - [23:16] Major version
 *                      - [15:8]  Minor version
 *                      - [7:0]   Patch version
 */
uint32_t microrl_get_version(void) {
    return ((MICRORL_VERSION_MAJOR << 16) | (MICRORL_VERSION_MINOR << 8) | (MICRORL_VERSION_PATCH));
}
