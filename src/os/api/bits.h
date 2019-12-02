#ifndef SRC_OS_API_BITS_H_
#define SRC_OS_API_BITS_H_

// Returns a mask with the bit at rank x set
#define BIT(x)               (1 << (x))

// Apply mask
#define MASK(x, m)           ((x) & (m))

// Get bit at position n
#define GET_BIT(x, n)        MASK((x) >> (n), 0x1)

// (re)set bit b in r
#define SET_BIT(r, b)        (r |= BIT(b))
#define RESET_BIT(r, b)      (r &= ~BIT(b))

// Split byte into half bytes
#define HIGH_NIBBLE(b)       MASK((b) >> 4, 0xF)
#define LOW_NIBBLE(b)        MASK((b), 0xF)

// Split word into bytes
#define HIGH_BYTE(b)         MASK((b) >> 8, 0xFF)
#define LOW_BYTE(b)          MASK((b), 0xFF)

// (re)set bitmask in r
#define SET_MASK(r, m)       (r |= (m))
#define RESET_MASK(r, m)     (r &= ~(m))

// Test bit of value r at index x
#define IS_SET_BIT(r, x)     MASK((r), BIT(x))

#define ADD_BOUNDED(var, inc, min, max) (var = (min) + ((var+(min)+(inc)) % (max-min+1)))
#define SUB_BOUNDED(var, inc, min, max) ADD_BOUNDED(var, -(inc), min, max)
#define ADD_MODULO(var, inc, max)       ADD_BOUNDED(var, inc, 0, (max)-1)
#define SUB_MODULO(var, inc, max)       SUB_BOUNDED(var, inc, 0, (max)-1)
#define INCREMENT_MOD(var, max)         ADD_MODULO(var, 1, max)
#define DECREMENT_MOD(var, max)         SUB_MODULO(var, 1, max)

#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define MIN(a, b) ((a) < (b) ? (a) : (b))

#endif /* SRC_OS_API_BITS_H_ */
