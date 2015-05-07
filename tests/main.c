/**
 * @file  tests/main.c
 * @brief テスト
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

#include <stdlib.h> /* EXIT_SUCCESS */
#include <CUnit/Console.h>

#include "cunit.h"

int main(void)
{
    CU_pSuite checksum;
    CU_pSuite data;

    CU_initialize_registry();
    checksum = CU_add_suite("checksum", NULL, NULL);
    data = CU_add_suite("data", NULL, NULL);

    CU_add_test(checksum, "test_001", test_001_checksum);
    CU_add_test(checksum, "test_002", test_002_checksum);

    CU_add_test(data, "test_001", test_001_data);

    CU_console_run_tests();
    CU_cleanup_registry();

    return EXIT_SUCCESS;
}
