#include <assert.h>
#include <string.h>
#include "types.h"

void __pad_right(const char *src, unsigned int src_len, char *dst, unsigned int dst_len)
{
    assert(dst_len >= src_len);
    memcpy(dst+dst_len-src_len, src, src_len);
    memset(dst, ' ', dst_len-src_len);
}

void __int_to_str(char *buffer, int nbr, int pos, int len)
{
    for (int i = 1 ; i <= len ; i++)
    {
        int digit = nbr % 10;
        buffer[pos+len-i] = digit + '0';
        nbr /= 10;
    }
}
