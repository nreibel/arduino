#ifndef SRC_OS_API_BITS_H_
#define SRC_OS_API_BITS_H_

// Check if a is between b and c
#define BETWEEN(a, b, c) ((a >= b) && (a <= c))
#define ABS(v)           ((v) >= 0 ? (v) : -(v))

// Returns a mask with the bit at rank x set
#define BIT(x)               (1ULL << (x))

// Apply mask
#define MASK(x, m)           ((x) & (m))

// (re)set bit b in r, with mask m
#define SET_BIT(r, b)        (r |= BIT(b))
#define RESET_BIT(r, b)      (r &= ~BIT(b))
#define TOGGLE_BIT(r, b)     (r ^= BIT(b))
#define SET_BITS(r, b, m)    (r = MASK(r, ~(m)) | MASK(b, m))

// Split byte into half bytes
#define HIGH_NIBBLE(b)       MASK((b) >> 4, 0xF)
#define LOW_NIBBLE(b)        MASK((b), 0xF)

// Split word into bytes
#define BYTE(b, n)           MASK((b) >> 8*(n), 0xFF)
#define HIGH_BYTE(b)         BYTE((b), 1)
#define LOW_BYTE(b)          BYTE((b), 0)

// (re)set bitmask in r
#define SET_MASK(r, m)       (r |= (m))
#define RESET_MASK(r, m)     (r &= ~(m))
#define TOGGLE_MASK(r, m)    (r ^= (m))

// Test bit of value r at index x
#define IS_SET_BIT(r, x)     (MASK((r), BIT(x)))

#define ADD_BOUNDED(var, inc, min, max) (var = (min) + ((var+(min)+(inc)) % (max-min+1)))
#define SUB_BOUNDED(var, inc, min, max) ADD_BOUNDED(var, -(inc), min, max)
#define ADD_MODULO(var, inc, max)       ADD_BOUNDED(var, inc, 0, (max)-1)
#define SUB_MODULO(var, inc, max)       SUB_BOUNDED(var, inc, 0, (max)-1)
#define INCREMENT_MOD(var, max)         ADD_MODULO(var, 1, max)
#define DECREMENT_MOD(var, max)         SUB_MODULO(var, 1, max)

// Map a value x range [in1,in2] to range [out1,out2]
#define MAP(x, in1, in2, out1, out2) (((x)-(in1))*((out2)-(out1))/((in2)-(in1))+(out1))

#define MAX(a, b)          ((a) > (b) ? (a) : (b))
#define MIN(a, b)          ((a) < (b) ? (a) : (b))
#define LIMIT(x, min, max) ((x) > (min) ? (x) < (max) ? (x) : (max) : (min))

#endif /* SRC_OS_API_BITS_H_ */
