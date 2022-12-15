#ifndef SRC_CRC_API_CRC_H_
#define SRC_CRC_API_CRC_H_

#include "types.h"

#define CRC32_CRC32Q_POLYNOMIAL 0x814141AB
#define CRC32_CRC32Q_INIT       0
#define CRC32_CRC32Q_XOROUT     0

#define CRC32_BZIP2_POLYNOMIAL  0x04C11DB7
#define CRC32_BZIP2_INIT        0xFFFFFFFF
#define CRC32_BZIP2_XOROUT      0xFFFFFFFF

#define CRC32_MPEG2_POLYNOMIAL  0x04C11DB7
#define CRC32_MPEG2_INIT        0xFFFFFFFF
#define CRC32_MPEG2_XOROUT      0

#define CRC32_POSIX_POLYNOMIAL  0x04C11DB7
#define CRC32_POSIX_INIT        0
#define CRC32_POSIX_XOROUT      0xFFFFFFFF

#define CRC32_XFER_POLYNOMIAL   0x000000AF
#define CRC32_XFER_INIT         0
#define CRC32_XFER_XOROUT       0

enum {
    CRC_OK,
    CRC_ERR_PARAM,
    CRC_ERR_INIT,
};

typedef struct crc_data_s {
    uint32_t        crc;    // Current value
    unsigned int    len;    // CRC length in bits
    uint32_t        poly;   // Polynomial
    uint32_t        init;   // Initial value
    uint32_t        xorout; // Final XOR value
    bool            refin;  // Reflect input data
    bool            refout; // Reflect final CRC
} * crc_data_t;

int crc_init       (crc_data_t self, unsigned int len, uint32_t poly, uint32_t init, uint32_t xorout, bool refin, bool refout);
int crc_reset      (crc_data_t self);
int crc_feed_byte  (crc_data_t self, uint8_t byte);
int crc_feed_bytes (crc_data_t self, const void * byte, unsigned int len);
int crc_final      (crc_data_t self, uint32_t * crc);

#endif /* SRC_CRC_API_CRC_H_ */
