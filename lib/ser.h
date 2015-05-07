/**
 * @file  lib/ser.h
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

#ifndef SER_H
#define SER_H

#define SER_OK  0
#define SER_NG -1
#define SER_TO -2

int ser_open(const char *dev, int *fd);
int ser_close(int *fd);
int ser_write(const int fd, const void *sdata, size_t *length);
int ser_read(const int fd, void *rdata, size_t *length);
int ser_read_to(const int fd, void *rdata, size_t *length, struct timespec *ts);
int ser_read_to2(const int fd, void *rdata, size_t *length);

#endif /* SER_H */
