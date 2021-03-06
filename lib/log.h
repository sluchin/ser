/**
 * @file lib/log.h
 * @brief ログ出力
 *
 * @author higashi
 * @date 2010-06-22 higashi 新規作成
 *
 * Copyright (C) 2010-2015 Tetsuya Higashi. All Rights Reserved.
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

#ifndef _OUTPUTLOG_H_
#define _OUTPUTLOG_H_

#include <stddef.h> /* size_t */
#include <syslog.h> /* syslog LOG_INFO LOG_PID */

#define SYS_FACILITY  LOG_SYSLOG
#define LOGARGS       get_progname(), __FILE__, __LINE__, __FUNCTION__
#define SYSARGS       LOG_INFO, LOG_PID, LOGARGS

/* エラー時ログメッセージ出力 */
//#define outlog(fmt, ...)        system_log(SYSARGS, fmt, ## __VA_ARGS__)
#define outlog(fmt, ...)        stderr_log(LOGARGS, fmt, ## __VA_ARGS__)
#define outdump(a, b, fmt, ...) dump_log(SYSARGS, a, b, fmt, ## __VA_ARGS__)
/* デバッグ用ログメッセージ */
#ifdef _DEBUG
//#  define dbglog(fmt, ...)        system_dbg_log(SYSARGS, fmt, ## __VA_ARGS__)
#  define dbglog(fmt, ...)        stderr_log(LOGARGS, fmt, ## __VA_ARGS__)
//#  define dbgdump(a, b, fmt, ...) dump_sys(SYSARGS, a, b, fmt, ## __VA_ARGS__)
#  define dbgdump(a, b, fmt, ...) dump_log(a, b, fmt, ## __VA_ARGS__)
#  ifdef HAVE_EXECINFO
#    define dbgtrace()            systrace(SYSARGS)
#  else
#    define dbgtrace()            do { } while (0)
#  endif
#  define dbgterm(fd)             sys_print_termattr(SYSARGS, fd)
#else
#  define dbglog(fmt, ...)        do { } while (0)
#  define stdlog(fmt, ...)        do { } while (0)
#  define dbgdump(a, b, fmt, ...) do { } while (0)
#  define stddump(a, b, fmt, ...) do { } while (0)
#  define dbgtrace()              do { } while (0)
#  define dbgterm(fd)             do { } while (0)
#endif /* _DEBUG */

/** プログラム名設定 */
void set_progname(const char *name);

/** プログラム名取得 */
char *get_progname(void);

/** シスログ出力 */
void system_log(const int level, const int option, const char *pname,
                const char *fname, const int line, const char *func,
                const char *format, ...);

/** シスログ出力(デバッグ用) */
void system_dbg_log(const int level, const int option, const char *pname,
                    const char *fname, const int line, const char *func,
                    const char *format, ...);

/** 標準エラー出力にログ出力 */
void stderr_log(const char *pname, const char *fname,
                const int line, const char *func,
                const char *format, ...);

/** 標準エラー出力にHEXダンプ */
int dump_log(const void *buf, const size_t len, const char *format, ...);

/** シスログにHEXダンプ */
int dump_sys(const int level, const int option, const char *pname,
             const char *fname, const int line, const char *func,
             const void *buf, const size_t len, const char *format, ...);

/** ファイルにバイナリ出力 */
int dump_file(const char *pname, const char *fname, const char *buf,
              const size_t len);

/** バックトレースシスログ出力 */
void systrace(const int level, const int option, const char *pname,
              const char *fname, const int line, const char *func);

/** バックトレース出力 */
void print_trace(void);

#endif /* _OUTPUTLOG_H_ */

