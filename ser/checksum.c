#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "log.h"
#include "checksum.h"

unsigned char
check_checksum(unsigned char *c, size_t n)
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
