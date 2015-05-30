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

#include <stdio.h>     /* fopen fclose */
#include <stdlib.h>    /* calloc EXIT_SUCCESS */
#include <sys/types.h> /* stat */
#include <sys/stat.h>  /* stat */
#include <unistd.h>    /* access stat */
#include <string.h>    /* memset */

#include "log.h"
#include "ser.h"
#include "data.h"
#include "checksum.h"
#include "option.h"
#include "server.h"

#define RECV_SIZE 0xff
#define CODE_HEADER 0x55
#define CODE_FOOTER 0xff

/*** シグナル */
volatile sig_atomic_t sig_handled = 0;

/*** ファイル名 */
static char **filename = NULL;
/*** ファイル数 */
static int fcount = 0;
/*** 送受信回数 */
static unsigned long lcount = 0;
/*** 関数 */
static int (*func)(int);

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

static unsigned char recv_buf[RECV_SIZE];

/**
 * 受信送信ループ処理
 *
 * @param[in] fd 	ファイルディスクリプタ
 * @return EXIT_SUCCESS 成功
 */
int
recv_send_from_file(const int fd)
{
    int result = EXIT_FAILURE;
    int retval = 0;
    size_t rlen = 0;
    size_t length = 0;
    struct stat st;
    static FILE *fp = NULL;
    unsigned char *sbuf = NULL;
    unsigned char *p = NULL;
    unsigned char cksum = 0;
    int index = 0;

    dbglog("start");
    dbglog("fcount=%d", fcount);

    do {
        index = fcount - 1;
        do {
            /* 受信 */
            (void)recv_data(fd, recv_buf, sizeof(recv_buf));

            dbglog("filename=%s", filename[index]);
            if (!filename[index] ||
                access(filename[index], R_OK) < 0) {
                outlog("access error");
                goto out;
            }

            (void)memset(&st, 0x00, sizeof(struct stat));
            retval = stat(filename[index], &st);
            length = (size_t)st.st_size;
            if (retval < 0 || length == 0) {
                outlog("stat error: retval=%d length=%zu", retval, length);
                goto out;
            }

            fp = fopen(filename[index], "rb");
            if (!fp)
                goto out;

            sbuf = calloc(sizeof(unsigned char), length + 3);
            if (!sbuf)
                goto out;
            p = sbuf;

            /* ヘッダ設定 */
            *p = (unsigned char)CODE_HEADER;

            /* ファイル読み込み */
            rlen = fread(++p, sizeof(unsigned char), length, fp);
            if (ferror(fp)) {
                outlog("fread error: ferror=%d rlen=%zu length%zu",
                       ferror(fp), rlen, length);
                goto out;
            }
            p += rlen;

            /* チェックサム */
            cksum = add_checksum(sbuf, rlen + 2);
            dbglog("checksum=0x%hhx", cksum);

            /* フッタ設定 */
            *(++p) = (unsigned char)CODE_FOOTER;

            /* 送信 */
            (void)send_data(fd, sbuf, rlen + 3);

            if (sbuf)
                free(sbuf);
            sbuf = NULL;

            retval = fclose(fp);
            if (retval < 0)
                outlog("fclose");
            fp = NULL;

            if (sig_handled)
                break;

        } while (index-- > 0);
    } while (--lcount > 0);

    result = EXIT_SUCCESS;

out:
    if (sbuf)
        free(sbuf);
    sbuf = NULL;

    if (fp) {
        retval = fclose(fp);
        if (retval < 0)
            outlog("fclose");
    }
    fp = NULL;
    return result;
}

/**
 * 受信送信ループ処理
 *
 * @param[in] fd ファイルディスクリプタ
 * @retval EXIT_SUCCESS
 */
int
recv_send_loop(const int fd)
{
    dbglog("start");

    do {
        (void)recv_data(fd, recv_buf, sizeof(recv_buf));

        (void)send_data(fd, send_buf, sizeof(send_buf));

        if (sig_handled)
            break;
    } while (--lcount > 0);

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
    dbglog("start");

    do {
        (void)recv_data(fd, recv_buf, sizeof(recv_buf));

        if (++lcount%2)
            (void)send_data(fd, send_buf, sizeof(send_buf));
        else
            (void)send_data(fd, send_buf, sizeof(send_buf) - 1);

        if (sig_handled)
            break;
    } while (--lcount > 0);

    return EXIT_SUCCESS;
}

/**
 * 受信送信ループ処理
 *
 * @param[in] fd ファイルディスクリプタ
 * @retval EXIT_SUCCESS
 */
int
recv_send_loop3(const int fd)
{
    dbglog("start");

    do {
        (void)recv_data(fd, recv_buf, sizeof(recv_buf));

        if (lcount%2)
            (void)send_data(fd, send_buf, sizeof(send_buf));
        else
            (void)send_data(fd, send_buf2, sizeof(send_buf2));

        if (sig_handled)
            break;
    } while (--lcount > 0);

    return EXIT_SUCCESS;
}

/**
 * 受信送信ループ処理
 *
 * @param[in] fd ファイルディスクリプタ
 * @retval EXIT_SUCCESS
 */
int
recv_send_loop4(const int fd)
{
    dbglog("start");

    do {
        (void)recv_data(fd, recv_buf, sizeof(recv_buf));

        if (lcount%2)
            (void)send_data(fd, send_buf, sizeof(send_buf));
        else
            (void)send_data(fd, send_buf3, sizeof(send_buf3));

        if (sig_handled)
            break;
    } while (--lcount > 0);

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

        if (sig_handled)
            break;
    } while (--lcount > 0);

    return EXIT_SUCCESS;
}

/**
 * 受信ループ処理
 *
 * @param[in] fd ファイルディスクリプタ
 * @retval EXIT_SUCCESS
 */
int
recv_loop(const int fd)
{
    dbglog("start");

    do {
        (void)recv_data(fd, recv_buf, sizeof(recv_buf));

        if (sig_handled)
            break;
    } while (--lcount > 0);

    return EXIT_SUCCESS;
}

/**
 * ファイル名設定
 *
 * @param[in] file ファイル名
 * @return なし
 */
void
set_filename(char **file)
{
    filename = file;
}

/**
 * ファイル数設定
 *
 * @param[in] count ファイル数
 * @return なし
 */
void
set_file_count(int count)
{
    fcount = count;
}

/**
 * ループ回数設定
 *
 * @param[in] count ループ回数
 * @return なし
 */
void
set_loop_count(int count)
{
    lcount = count;
}

/**
 * コールバック関数設定
 *
 * @param[in] count ループ回数
 * @return なし
 */
void
set_callback(void *callback)
{
    func = callback;
}

/**
 * コールバック関数
 *
 * @param[in] fd ファイルディスクリプタ
 * @return なし
 */
int callback(int fd)
{
    return func(fd);
}
