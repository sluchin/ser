/**
 * @file  ser/main.c
 * @brief main関数
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

#include <stdlib.h> /* exit EXIT_SUCCESS */
#include <string.h> /* memset */
#include <signal.h> /* sigaction */
#include <termios.h> /* tcflush */
#include <unistd.h>  /* tcflush */

#include "option.h"
#include "log.h"
#include "ser.h"
#include "server.h"

static int fd = -1;

/** シグナルハンドラ設定 */
static void set_sig_handler(void);
/** シグナルハンドラ */
static void sig_handler(int signo);

/**
 * main関数
 *
 * @param[in] argc 引数の数
 * @param[in] argv コマンド引数・オプション引数
 * @return 常にEXIT_SUCCESS
 */
int
main(int argc, char *argv[])
{
    int retval = 0;

    dbglog("start");

    set_progname(argv[0]);

    /* シグナルハンドラ */
    set_sig_handler();

    /* オプション引数 */
    parse_args(argc, argv);

    retval = ser_open(device, &fd);
    if (retval < 0)
        return EXIT_FAILURE;

    retval = tcflush(fd, TCIOFLUSH);
    if (retval < 0)
        outlog("tcflash error");

    /* ループ */
    retval = func(fd);
    if (retval < 0)
        exit(EXIT_FAILURE);

    retval = ser_close(&fd);
    if (retval < 0)
        return EXIT_FAILURE;

    exit(EXIT_SUCCESS);
    return EXIT_SUCCESS;
}

/**
 * シグナルハンドラ設定
 *
 * @return なし
 */
static void
set_sig_handler(void)
{
    struct sigaction sa; /* sigaction構造体 */
    sigset_t sigmask;    /* シグナルマスク */

    /* シグナルマスクの設定 */
    (void)memset(&sa, 0, sizeof(struct sigaction));
    if (sigemptyset(&sigmask) < 0)
        outlog("sigemptyset=0x%x", sigmask);
    if (sigfillset(&sigmask) < 0)
        outlog("sigfillset=0x%x", sigmask);
    dbglog("sigmask=0x%x", sigmask);

    /* シグナル補足 */
    if (sigaction(SIGINT, (struct sigaction *)NULL, &sa) < 0)
        outlog("sigaction=%p, SIGINT", &sa);
    sa.sa_handler = sig_handler;
    sa.sa_mask = sigmask;
    if (sigaction(SIGINT, &sa, (struct sigaction *)NULL) < 0)
        outlog("sigaction=%p, SIGINT", &sa);

    if (sigaction(SIGTERM, (struct sigaction *)NULL, &sa) < 0)
        outlog("sigaction=%p, SIGTERM", &sa);
    sa.sa_handler = sig_handler;
    sa.sa_mask = sigmask;
    if (sigaction(SIGTERM, &sa, (struct sigaction *)NULL) < 0)
        outlog("sigaction=%p, SIGTERM", &sa);

    if (sigaction(SIGQUIT, (struct sigaction *)NULL, &sa) < 0)
        outlog("sigaction=%p, SIGQUIT", &sa);
    sa.sa_handler = sig_handler;
    sa.sa_mask = sigmask;
    if (sigaction(SIGQUIT, &sa, (struct sigaction *)NULL) < 0)
        outlog("sigaction=%p, SIGQUIT", &sa);
}

/**
 * シグナルハンドラ
 *
 * @param[in] signo シグナル
 * @return なし
 */
static void sig_handler(int signo)
{
    sig_handled = 1;
}
