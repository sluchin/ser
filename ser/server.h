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

#ifndef SERVER_H
#define SERVER_H

#include <signal.h>  /* sig_atomic_t */

int recv_send_from_file(const int fd);
int recv_send_loop(const int fd);
int recv_send_loop2(const int fd);
int recv_send_loop3(const int fd);
int recv_send_loop4(const int fd);
int send_loop(const int fd);
int recv_loop(const int fd);
void set_filename(char **file);
void set_file_count(int count);
void set_loop_count(int count);
void set_callback(void *callback);
int callback(int fd);

/*** シグナル */
extern volatile sig_atomic_t sig_handled;

#endif
