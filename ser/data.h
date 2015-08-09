/**
 * @file  ser/data.h
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

#ifndef DATA_H
#define DATA_H

#include <stdbool.h>    /* bool */

#define CODE_HEADER 0x55
#define CODE_FOOTER 0xff
#define SIZE_HEADER sizeof(CODE_HEADER)
#define SIZE_FOOTER sizeof(CODE_FOOTER)
#define SIZE_RECV 0xff

int send_data(const int fd, const unsigned char *buf, const size_t size);
int recv_data(const int fd, unsigned char *rbuf, const size_t size);
void set_timeout(struct timespec *ts);

#endif
