/**
 * @file  tests/test_recv.c
 * @brief シリアル受信テスト
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

#include <stdbool.h> /* bool */
#include <CUnit/CUnit.h>

#include "data.h"

static unsigned char data1[] = {
    0x01, 0x02, 0x55, 0x04, 0x05, 0x00
};

static unsigned char data2[] = {
    0x01, 0x02, 0x03, 0x04, 0x05, 0x00
};

void
test_001_data(void)
{
    int pos = 0;
    size_t length = 0;
    bool result = false;
    result = search_header(data1, sizeof(data1), &pos, &length);
    CU_ASSERT(result == true);
    CU_ASSERT(pos == 4);
    CU_ASSERT(length == 2);

    result = search_header(data2, sizeof(data2), &pos, &length);
    CU_ASSERT(result == false);
}
