/**
 * @file  tests/test_checksum.c
 * @brief チェックサムテスト
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

#include <CUnit/CUnit.h>

#include "checksum.h"

static unsigned char data[] = {
    0x01, 0x02, 0x03, 0x04, 0x05, 0x00
};

void
test_001_checksum(void)
{
    size_t size = sizeof(data);
    unsigned char checksum = 0;
    checksum = add_checksum(data, size);
    CU_ASSERT(checksum == 0x01);
    CU_ASSERT(data[size - 1] == 0x01);
    checksum = check_checksum(data, size);
    CU_ASSERT(checksum == 0x00);
}

void
test_002_checksum(void)
{
    size_t size = sizeof(data);
    unsigned char checksum = 0;
    data[size - 1] = 0x02;
    checksum = check_checksum(data, size);
    CU_ASSERT(checksum != 0x00);
}
