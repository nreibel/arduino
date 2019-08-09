#ifndef OS_API_UTILS_H_
#define OS_API_UTILS_H_

#include "types.h"

/**
 * Compare 2 arrays up to the specified length, return value of 0 means no difference
 */
int __memcmp(const uint8_t *str1, const uint8_t *str2, int length);

/**
 * Copy buffer
 */
void __memcpy(const uint8_t *src, uint8_t *dst, int len);

/**
 * Right-align and pad with spaces a string of length len_buffer into a buffer of length len_padded.
 */ 
void __pad_str(const uint8_t *src, int len_src, uint8_t *dst, int len_dst);

/**
 * Print a number nbr of length len into the string buffer at position pos
 */
void __int_to_str(uint8_t *buffer, int nbr, int pos, int len);

#endif /* OS_API_UTILS_H_ */
