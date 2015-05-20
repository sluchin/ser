/**
 * @file  ser/checksum.c
 * @brief チェックサム
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

#include "log.h"
#include "checksum.h"

unsigned char
checksum(unsigned char *c, size_t n)
{
    unsigned char checksum = 0;
    int i;
    for (i = 0; i < (n - 1); i++)
        checksum ^= c[i];
    if (checksum)
        outlog("check sum error");
    return checksum;
}

unsigned char
add_checksum(unsigned char *c, size_t n)
{
    unsigned char checksum = 0;
    unsigned char *p = c;

    int i;
    for (i = 0; i < n; i++)
        checksum ^= *(p++);

    *(--p) = checksum;
    return checksum;
}
