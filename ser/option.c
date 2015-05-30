/**
 * @file  ser/option.c
 * @brief オプション引数の処理
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

#include <stdio.h>  /* fprintf */
#include <stdlib.h> /* strtol EXIT_SUCCESS */
#include <string.h> /* memset */
#include <getopt.h> /* getopt_long */
#include <errno.h>  /* errno */

#include "log.h"
#include "version.h"
#include "server.h"
#include "data.h"
#include "option.h"

/** オプション情報構造体(ロング) */
static struct option longopts[] = {
    { "send",    no_argument,       NULL, 's' },
    { "recv",    no_argument,       NULL, 'r' },
    { "error",   required_argument, NULL, 'e' },
    { "count",   required_argument, NULL, 'c' },
    { "timeout", no_argument,       NULL, 'T' },
    { "device",  required_argument, NULL, 'D' },
    { "help",    no_argument,       NULL, 'h' },
    { "version", no_argument,       NULL, 'V' },
    { NULL,      0,                 NULL, 0   }
};

/** オプション情報文字列(ショート) */
static const char *shortopts = "sre:c:TD:hV";

/** デバイス */
static char *device = NULL;

/** ヘルプの表示 */
static void print_help(const char *progname);
/** バージョン情報表示 */
static void print_version(const char *progname);
/** getoptエラー表示 */
static void parse_error(const int c, const char *msg);

/**
 * オプション引数
 *
 * オプション引数ごとに処理を分岐する.
 * @param[in] argc 引数の数
 * @param[in] argv コマンド引数・オプション引数
 * @return なし
 */
void
parse_args(int argc, char *argv[])
{
    int opt = 0;
    unsigned char e = 0x00;
    char *endptr = NULL;
    const int base = 10;
    bool r_flag = false;
    struct timespec timeout;
    long count = 0;

    set_callback(recv_send_loop);

    while ((opt = getopt_long(argc, argv, shortopts, longopts, NULL)) != EOF) {
        dbglog("opt=%c, optarg=%s", opt, optarg);
        switch (opt) {
        case 's': /* 送信 */
            set_callback(send_loop);
            break;
        case 'r': /* 受信 */
            set_callback(recv_loop);
            r_flag = true;
            break;
        case 'e': /* エラー系送受信 */
            (void)memcpy(&e, optarg, 1);
            switch (e) {
            case '1':
                set_callback(recv_send_loop2);
                break;
            case '2':
                set_callback(recv_send_loop3);
                break;
            case '3':
                set_callback(recv_send_loop4);
                break;
            default:
                print_help(get_progname());
                exit(EXIT_FAILURE);
            }
            break;
        case 'c': /* 回数 */
            count = strtol(optarg, &endptr, base);
            if ((errno != 0) || (*endptr != '\0')) {
                outlog("endptr=%c", *endptr);
                exit(EXIT_FAILURE);
            }
            set_loop_count(count);
        case 'T': /* タイムアウト */
            (void)memset(&timeout, 0x00, sizeof(struct timespec));
            timeout.tv_sec = 1;
            timeout.tv_nsec = 0;
            set_timeout(&timeout);
            break;
        case 'D': /* デバイス指定 */
            device = optarg;
            break;
        case 'h': /* ヘルプ表示 */
            print_help(get_progname());
            exit(EXIT_SUCCESS);
        case 'V': /* バージョン情報表示 */
            print_version(get_progname());
            exit(EXIT_SUCCESS);
        case '?':
        case ':':
            parse_error(opt, NULL);
            exit(EXIT_FAILURE);
        default:
            parse_error(opt, "internal error");
            exit(EXIT_FAILURE);
        }
    }
    if (optind < argc) {
        if (!r_flag) {
            set_file_count(argc - optind);
            set_filename(&argv[optind]);
            set_callback(recv_send_from_file);
            dbglog("fcount=%d filename=%p",
                   argc - optind, &argv[optind]);
        }
    }
}

/**
 * ヘルプ表示
 *
 * ヘルプを表示する.
 * @param[in] progname プログラム名
 * @return なし
 */
static void
print_help(const char *progname)
{
    (void)fprintf(stderr, "Usage: %s [option]... [filename]...\n", progname);
    (void)fprintf(stderr, "  -s, --send             %s",
                  "send serial\n");
    (void)fprintf(stderr, "  -r, --recv             %s",
                  "recv serial\n");
    (void)fprintf(stderr, "  -e, --error [1,2,3]    %s",
                  "error case\n");
    (void)fprintf(stderr, "  -T, --timeout          %s",
                  "timeout\n");
    (void)fprintf(stderr, "  -D, --device           %s",
                  "device\n");
    (void)fprintf(stderr, "  -h, --help             %s",
                  "display this help and exit\n");
    (void)fprintf(stderr, "  -V, --version          %s",
                  "output version information and exit\n");
}

/**
 * バージョン情報表示
 *
 * バージョン情報を表示する.
 * @param[in] progname プログラム名
 * @return なし
 */
static void
print_version(const char *progname)
{
    (void)fprintf(stderr, "%s %s\n", progname, VERSION);
}

/**
 * getopt エラー表示
 *
 * getopt が異常な動作をした場合, エラーを表示する.
 * @param[in] c オプション引数
 * @param[in] msg メッセージ文字列
 * @return なし
 */
static void
parse_error(const int c, const char *msg)
{
    if (msg)
        (void)fprintf(stderr, "getopt[%d]: %s\n", c, msg);
    (void)fprintf(stderr, "Try `getopt --help' for more information\n");
}

/**
 * デバイス名取得
 *
 * @return デバイス名
 */
char *get_device(void)
{
    return device;
}
