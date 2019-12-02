#ifndef OS_API_UTILS_H_
#define OS_API_UTILS_H_

#include "types.h"

/**
 * Right-align and pad with spaces a string of length len_buffer into a buffer of length len_padded.
 */ 
void __pad_right(const uint8_t *src, unsigned int len_src, uint8_t *dst, unsigned int len_dst);

/**
 * Print a number nbr of length len into the string buffer at position pos
 */
void __int_to_str(uint8_t *buffer, int nbr, int pos, int len);

#endif /* OS_API_UTILS_H_ */
