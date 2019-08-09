#include "types.h"

int __memcmp(const uint8_t *str1, const uint8_t *str2, int length)
{
    int i = 0;
    while (str1[i] == str2[i] && i++ < length);
    return length - i;
}

void __pad_str(const uint8_t *buffer, int len_buffer, uint8_t *padded, int len_padded)
{
    while (len_padded > 0 && len_buffer > 0) padded[--len_padded] = buffer[--len_buffer];
    while (len_padded > 0) padded[--len_padded] = ' ';
}

void __int_to_str(uint8_t *buffer, int nbr, int pos, int len)
{
    for (int i = 0 ; i < len ; i++)
    {
        int digit = nbr % 10;
        buffer[pos-i] = digit + '0';
        nbr /= 10;
    }
}

void __memcpy(const uint8_t *src, uint8_t *dst, int len)
{
    while (len-- > 0)
    {
        *(dst++) = *(src++);
    }
}
