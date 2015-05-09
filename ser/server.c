/**
 * @file  ser/server.c
 * @brief ループ処理
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

#include "log.h"
#include "ser.h"
#include "data.h"
#include "server.h"
#include "option.h"

/*** シグナル */
volatile sig_atomic_t sig_handled = 0;

static const unsigned char send_buf[] =
{
    0x55,
    0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
    0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0x10,
    0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18,
    0x19, 0x1a, 0x1b, 0x1c,
    0x49, 0x0d
};

static const unsigned char send_buf2[] =
{
    0x55,
    0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
    0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0x10,
    0x55,
    0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
    0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0x10,
    0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18,
    0x19, 0x1a, 0x1b, 0x1c,
    0x49, 0x0d
};

static const unsigned char send_buf3[] =
{
    0x55,
    0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
    0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0x10,
    0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18,
    0x19, 0x1a, 0x1b, 0x1c,
    0x50, 0x0d
};

/**
 * 受信送信ループ処理
 *
 * @param[in] fd ファイルディスクリプタ
 * @return なし
 */
int
recv_send_loop(const int fd)
{
    dbglog("start");

    do {
        (void)recv_data(fd, sizeof(send_buf));

        (void)send_data(fd, send_buf, sizeof(send_buf));

    } while (!sig_handled && (!count || !(--count <= 0)));

    return EXIT_SUCCESS;
}

/**
 * 受信送信ループ処理
 *
 * @param[in] fd ファイルディスクリプタ
 * @return なし
 */
int
recv_send_loop2(const int fd)
{
    static int counter = 0;

    dbglog("start");

    do {
        (void)recv_data(fd, sizeof(send_buf));

        if (++counter%2)
            (void)send_data(fd, send_buf, sizeof(send_buf));
        else
            (void)send_data(fd, send_buf, sizeof(send_buf) - 1);

    } while (!sig_handled && (!count || !(--count <= 0)));

    return EXIT_SUCCESS;
}

/**
 * 受信送信ループ処理
 *
 * @param[in] fd ファイルディスクリプタ
 * @return なし
 */
int
recv_send_loop3(const int fd)
{
    static int counter = 0;

    dbglog("start");

    do {
        (void)recv_data(fd, sizeof(send_buf));

        if (++counter%2)
            (void)send_data(fd, send_buf, sizeof(send_buf));
        else
            (void)send_data(fd, send_buf2, sizeof(send_buf2));

    } while (!sig_handled && (!count || !(--count <= 0)));

    return EXIT_SUCCESS;
}

/**
 * 受信送信ループ処理
 *
 * @param[in] fd ファイルディスクリプタ
 * @return なし
 */
int
recv_send_loop4(const int fd)
{
    static int counter = 0;

    dbglog("start");

    do {
        (void)recv_data(fd, sizeof(send_buf));

        if (++counter%2)
            (void)send_data(fd, send_buf, sizeof(send_buf));
        else
            (void)send_data(fd, send_buf3, sizeof(send_buf3));

    } while (!sig_handled && (!count || !(--count <= 0)));

    return EXIT_SUCCESS;
}

/**
 * 送信ループ処理
 *
 * @param[in] fd ファイルディスクリプタ
 * @return なし
 */
int
send_loop(const int fd)
{
    dbglog("start");

    do {
        (void)send_data(fd, send_buf, sizeof(send_buf));

    } while (!sig_handled && (!count || !(--count <= 0)));

    return EXIT_SUCCESS;
}

/**
 * 受信ループ処理
 *
 * @param[in] fd ファイルディスクリプタ
 * @return なし
 */
int
recv_loop(const int fd)
{
    dbglog("start");

    do {
        (void)recv_data(fd, sizeof(send_buf));

    } while (!sig_handled && (!count || !(--count <= 0)));

    return EXIT_SUCCESS;
}
