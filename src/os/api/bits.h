#ifndef SRC_OS_API_BITS_H_
#define SRC_OS_API_BITS_H_

// Returns a mask with the bit at rank x set
#define BIT(x)               (1 << (x))

// Get bit at position n
#define GET_BIT(x, n)        (((x) >> n) & 1)

// Set bit b in r
#define SET_BIT(r, b)        (r |= BIT(b))

#define HIGH_NIBBLE(b)       (((b) >> 4) & 0xF)
#define LOW_NIBBLE(b)        ((b) & 0xF)

// Reset bit b in r
#define RESET_BIT(r, b)      (r &= ~BIT(b))

// Set bits of m in r
#define SET_MASK(r, m)       (r |= (m))

// Reset bits of m in r
#define RESET_MASK(r, m)     (r &= ~(m))

// Test bit of value r at index x
#define IS_SET_BIT(r, x)     ((r) & BIT(x))

#endif /* SRC_OS_API_BITS_H_ */
