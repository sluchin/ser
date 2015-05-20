/**
 * @file  lib/ser.c
 * @brief シリアル
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

#include <sys/types.h> /* open */
#include <sys/stat.h>  /* open */
#include <fcntl.h>     /* open */
#include <unistd.h>    /* write accdss */
#include <string.h>    /* memset */
#include <errno.h>     /* errno */
#include <signal.h>    /* signal */
#include <termios.h>   /* termios */

#include "log.h"
#include "ser.h"

#ifdef _USE_SELECT
#  define SER_FLAGS (O_RDWR | O_NOCTTY)
#else
#  define SER_FLAGS (O_RDWR | O_NOCTTY | O_NONBLOCK)
#endif /* _USE_SELECT */

static struct termios oldtio;
static struct termios newtio;

static int ser_config(const int fd);
static sigset_t get_sigmask(void);

/**
 * オープン
 *
 * @param[in]  dev デバイス
 * @param[out] fd  ファイルディスクリプタ
 * @retval SER_NG エラー
 */
int
ser_open(const char *dev, int *fd)
{
    int retval = 0;

    if (!dev) {
        outlog("device null");
        return SER_NG;
    } else {
        if (access(dev, R_OK | W_OK) < 0) {
            outlog("access error");
            return SER_NG;
        }
    }

    *fd = open(dev, SER_FLAGS);
    if (*fd < 0) {
        outlog("open error");
        return SER_NG;
    }

    retval = ser_config(*fd);
    if (retval < 0)
        return SER_NG;

    return SER_OK;
}

/**
 * クローズ
 *
 * @param[in,out] fd ファイルディスクリプタ
 * @retval SER_NG エラー
 */
int
ser_close(int *fd)
{
    int retval = 0;

    retval = tcsetattr(*fd, TCSANOW, &oldtio);
    if (retval < 0)
        outlog("tcsetattr error");

    retval = close(*fd);
    if (retval < 0) {
        outlog("close error");
        return SER_NG;
    }

    *fd = -1;
    return SER_OK;
}

/**
 * 設定
 *
 * @param[in] fd ファイルディスクリプタ
 * @retval SER_NG エラー
 */
static int
ser_config(const int fd)
{
    int retval = 0;

    (void)memset(&oldtio, 0x00, sizeof(struct termios));
    (void)memset(&newtio, 0x00, sizeof(struct termios));

    retval = tcgetattr(fd, &oldtio);
    if (retval < 0) {
        outlog("tcgetattr error");
        return SER_NG;
    }

    (void)memcpy(&newtio, &oldtio, sizeof(struct termios));

    newtio.c_iflag = IGNPAR;
    newtio.c_oflag = 0;
    newtio.c_cflag = B9600 | CS8 | CLOCAL | CREAD; // | CRTSCTS;
    newtio.c_lflag = 0;

    newtio.c_cc[VTIME] = 0;
    newtio.c_cc[VMIN] = 1;

    cfsetispeed(&newtio, B9600);
    cfsetospeed(&newtio, B9600);

    retval = tcflush(fd, TCIFLUSH);
    if (retval < 0)
        outlog("tcflash error");

    retval = tcsetattr(fd, TCSANOW, &newtio);
    if (retval < 0) {
        outlog("tcsetattr error");
        return SER_NG;
    }

    return SER_OK;
}

/**
 * データ送信
 *
 * @param[in]     fd     ファイルディスクリプタ
 * @param[in]     sdata  送信データ
 * @param[in,out] length データ長
 * @retval SER_NG エラー
 */
int
ser_write(const int fd, const void *sdata, size_t *length)
{
    ssize_t len = 0; /* write戻り値 */
    size_t left = 0; /* 残りのバイト数 */
    const unsigned char *ptr = NULL; /* ポインタ */

    dbglog("start: fd=%d, sdata=%p, length=%zu", fd, sdata, *length);

    ptr = (unsigned char *)sdata;
    left = *length;
    while (left > 0) {
        len = write(fd, sdata, *length);
        dbglog("write=%zd, ptr=%p, left=%zu", len, ptr, left);
        if (len <= 0) {
            if (errno == EINTR) {
                len = 0;
                outlog("len=%zd, left=%zu, *length=%zu", len, left, *length);
                break;
            } else {
                goto error;
            }
        }
        left -= len;
        ptr += len;
    }
    *length -= left;
    dbglog("write=%zd, fd=%d, ptr=%p, left=%zu, length=%zu",
           len, fd, ptr, left, *length);
    return SER_OK;

error:
    *length -= left;
    outlog("write=%zd, fd=%d, ptr=%p, left=%zu, length=%zu",
           len, fd, ptr, left, *length);
    return SER_NG;
}

/**
 * データ受信
 *
 * @param[in]     fd     ファイルディスクリプタ
 * @param[out]    rdata  受信データ
 * @param[in,out] length データ長
 * @retval SER_TO タイムアウト
 * @retval SER_NG エラー
 */
int
ser_read(const int fd, void *rdata, size_t *length)
{
    ssize_t len = 0;   /* recv戻り値 */
    size_t left = 0;   /* 残りのバイト数 */
    unsigned char *ptr = NULL; /* ポインタ */

    dbglog("start: fd=%d, rdata=%p, length=%zu", fd, rdata, *length);

    ptr = (unsigned char *)rdata;
    left = *length;
    while (left > 0) {
        len = read(fd, ptr, left);
        dbglog("read=%zd, ptr=%p, left=%zu", len, ptr, left);
        if (len < 0) { /* エラー */
            if (errno == EINTR) {
                len = 0;
                break;
            } else {
                goto error;
            }
        } else if (len == 0) { /* 接続先がシャットダウンした */
            outlog("Not connected.");
            goto error;
        } else { /* 正常時 */
            left -= len;
            ptr += len;
        }
    }
    *length -= left;
    dbglog("read=%zd, fd=%d, ptr=%p, left=%zu, length=%zu",
           len, fd, ptr, left, *length);
    return SER_OK;

error:
    *length -= left;
    outlog("read=%zd, fd=%d, ptr=%p, left=%zu, length=%zu",
           len, fd, ptr, left, *length);
    return SER_NG;
}

/**
 * データ受信(タイムアウトあり)
 *
 * @param[in]     fd     ファイルディスクリプタ
 * @param[out]    rdata  受信データ
 * @param[in,out] length データ長
 * @param[in]     ts     タイムアウト値
 * @retval SER_TO タイムアウト
 * @retval SER_NG エラー
 */
#ifdef _USE_SELECT
int
ser_read_to(const int fd, void *rdata, size_t *length, struct timespec *ts)
{
    ssize_t len = 0;   /* recv戻り値 */
    size_t left = 0;   /* 残りのバイト数 */
    fd_set fds, rfds;  /* selectマスク */
    int ready = 0;     /* pselect戻り値 */
    sigset_t sigmask;  /* シグナルマスク */
    unsigned char *ptr = NULL; /* ポインタ */

    dbglog("start: fd=%d, rdata=%p, length=%zu", fd, rdata, *length);

    /* シグナルマスク取得 */
    sigmask = get_sigmask();

    /* マスクの設定 */
    FD_ZERO(&fds);    /* 初期化 */
    FD_SET(fd, &fds); /* ソケットをマスク */

    ptr = (unsigned char *)rdata;
    left = *length;
    while (left > 0) {
        (void)memcpy(&rfds, &fds, sizeof(fd_set)); /* マスクコピー */
        ready = pselect(fd + 1, &rfds,
                        NULL, NULL, ts, &sigmask);
        if (ready < 0) {
            if (errno == EINTR) /* 割り込み */
                break;
            outlog("select=%d", ready);
            break;
        } else if (ready) {
            if (FD_ISSET(fd, &rfds)) {
                len = read(fd, ptr, left);
                dbglog("read=%zd, ptr=%p, left=%zu", len, ptr, left);
                if (len < 0) { /* エラー */
                    if (errno == EINTR)
                        len = 0;
                    else
                        goto error;
                } else if (len == 0) { /* 接続先がシャットダウンした */
                    outlog("Not connected.");
                    goto error;
                } else { /* 正常時 */
                    left -= len;
                    ptr += len;
                }
            }
        } else { /* タイムアウト */
            outlog("timeout");
            goto timeout;
        }
    }
    *length -= left;
    dbglog("read=%zd, fd=%d, ptr=%p, left=%zu, length=%zu",
           len, fd, ptr, left, *length);
    return SER_OK;

timeout:
    *length -= left;
    dbglog("read=%zd, fd=%d, ptr=%p, left=%zu, length=%zu",
           len, fd, ptr, left, *length);
    return SER_TO;

error:
    *length -= left;
    outlog("read=%zd, fd=%d, ptr=%p, left=%zu, length=%zu",
           len, fd, ptr, left, *length);
    return SER_NG;
}
#else
int
ser_read_to2(const int fd, void *rdata, size_t *length)
{
    ssize_t len = 0;   /* recv戻り値 */
    size_t left = 0;   /* 残りのバイト数 */
    unsigned char *ptr = NULL; /* ポインタ */

    dbglog("start: fd=%d, rdata=%p, length=%zu", fd, rdata, *length);

    ptr = (unsigned char *)rdata;
    left = *length;
    while (left > 0) {
        len = read(fd, ptr, left);
        dbglog("read=%zd, ptr=%p, left=%zu", len, ptr, left);
        if (len < 0) { /* エラー */
            if (errno == EINTR) {
                len = 0;
                break;
            } else if ((errno == EAGAIN) || (errno == EWOULDBLOCK)) {
                /* ノンブロッキングタイムアウト */
                goto timeout;
            } else {
                goto error;
            }
        } else if (len == 0) { /* 接続先がシャットダウンした */
            outlog("Not connected.");
            goto error;
        } else { /* 正常時 */
            left -= len;
            ptr += len;
        }
    }
    *length -= left;
    dbglog("read=%zd, fd=%d, ptr=%p, left=%zu, length=%zu",
           len, fd, ptr, left, *length);
    return SER_OK;

timeout:
    *length -= left;
    dbglog("read=%zd, fd=%d, ptr=%p, left=%zu, length=%zu",
           len, fd, ptr, left, *length);
    return SER_TO;

error:
    *length -= left;
    outlog("read=%zd, fd=%d, ptr=%p, left=%zu, length=%zu",
           len, fd, ptr, left, *length);
    return SER_NG;
}
#endif /* _USE_SELECT */

/**
 * シグナルマスク取得
 *
 * @return シグナルマスク
 */
static sigset_t
get_sigmask(void)
{
    sigset_t sigmask; /* シグナルマスク */

    /* 初期化 */
    if (sigemptyset(&sigmask) < 0)
        outlog("sigemptyset=0x%x", sigmask);
    /* シグナル全て */
    if (sigfillset(&sigmask) < 0)
        outlog("sigfillset=0x%x", sigmask);
    /* SIGINT除く */
    if (sigdelset(&sigmask, SIGINT) < 0)
        outlog("sigdelset=0x%x", sigmask);
    dbglog("sigmask=0x%x", sigmask);

    return sigmask;
}
