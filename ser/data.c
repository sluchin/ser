/**
 * @file  ser/data.c
 * @brief 送受信処理
 *
 * @author higashi
 * @date 2014-12-28 higashi 新規作成
 *
 * Copyright (C) 2014-2015 Tetsuya Higashi. All Rights Reserved.
 */
/* This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#include <stdlib.h>     /* EXIT_SUCCESS */
#include <string.h>     /* memcpy */
#include <errno.h>      /* errno */
#include <sys/select.h> /* select */
#include <stdbool.h>    /* bool */

#include "log.h"
#include "ser.h"
#include "checksum.h"
#include "data.h"
#include "server.h"

#define CODE_HEADER 0x55
#define CODE_FOOTER 0x0d

struct timespec *ptimeout = NULL;
static size_t max = 0;

static bool search_header(unsigned char *buf, size_t size,
                          int *pos, size_t *length);

/**
 * 送信処理
 *
 * @param[in] fd   ファイルディスクリプタ
 * @param[in] buf  バッファ
 * @param[in] size サイズ
 * @return EXIT_FAILURE エラー
 */
int
send_data(const int fd, const unsigned char *buf, const size_t size)
{
    int retval = 0;    /* 戻り値 */
    size_t length = 0;

    dbglog("start");

    length = size;
    retval = ser_write(fd, buf, &length);
    if (retval < 0)
        return EXIT_FAILURE;

    dbgdump(buf, length, "write: length=%zu", length);

    return EXIT_SUCCESS;
}

/**
 * 受信処理
 *
 * @param[in] fd   ファイルディスクリプタ
 * @param[in] size サイズ
 * @return EXIT_FAILURE エラー
 */
int
recv_data(const int fd, const size_t size)
{
    int retval = 0;    /* 戻り値 */
    size_t length = 0; /* 受信する長さ*/
    int pos = 0;
    unsigned char recv_buf[size];

    dbglog("start");

    max = length = size;

    do {
        dbglog("ser_read: pos=%d, length=%zu", pos, length);
        retval = ser_read_to(fd, (unsigned char *)(recv_buf + pos), &length, ptimeout);
        if (retval < 0)
            break;
        dbgdump(recv_buf, length + pos, "read: length=%zu", length + pos);

        if (recv_buf[0] == CODE_HEADER &&
            recv_buf[sizeof(recv_buf) - 1] == CODE_FOOTER) {
            dbglog("checksum");
            // チェックサム
            retval = check_checksum(recv_buf, sizeof(recv_buf));
            if (retval) {
                outlog("checksum error");
                return EXIT_FAILURE;
            }
            break;
        } else { // ヘッダを検索
            dbglog("search_header");
            if (search_header(recv_buf, sizeof(recv_buf), &pos, &length))
                continue;
            outlog("search error");
            return EXIT_FAILURE;
        }
    } while (!sig_handled);

    return EXIT_SUCCESS;
}

/**
 * ヘッダ検索
 *
 * @param[in]  buf    バッファ
 * @param[in]  size   サイズ
 * @param[out] pos    ヘッダ位置
 * @param[out] length ヘッダからの長さ
 * @return true ヘッダが見つかった
 */
static bool search_header(unsigned char *buf, size_t size,
                          int *pos, size_t *length)
{
    unsigned char tmp[size];

    for (*pos = 1; *pos < size; (*pos)++) {
        if (*(buf + *pos) == CODE_HEADER) {
            *length = size - *pos;
            (void)memset(tmp, 0x00, size);
            (void)memcpy(tmp, buf + *pos, *length);
            (void)memset(buf, 0x00, size);
            (void)memcpy(buf, tmp, *length);
            dbgdump(buf, *length, "search: length=%zu", *length);
            *pos = *length;
            *length = size - *pos;
            return true;
        }
    }
    *length = max;
    *pos = 0;

    return false;
}
