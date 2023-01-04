#ifndef SRC_CRC_API_CRC_H_
#define SRC_CRC_API_CRC_H_

#include "types.h"
#include "os_cfg.h"

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

typedef const struct {
    unsigned int    length; // In bits
    uint32_t        polynomial;
    uint32_t        initial_value;
    uint32_t        final_xor;
    bool            reflect_input;
    bool            reflect_output;
} crc_config_s;

typedef struct crc_prv_s {
    uint32_t crc;       // Current value
    crc_config_s * cfg; // Config object

} * crc_t;

#if OS_MALLOC
crc_t crc_create(void);
void crc_destroy(crc_t self);
#endif // OS_MALLOC

int crc_init        (crc_t self, crc_config_s * cfg);
int crc_reset       (crc_t self);
int crc_feed_byte   (crc_t self, uint8_t byte);
int crc_feed_bytes  (crc_t self, const void * byte, unsigned int len);
int crc_get_result  (crc_t self, uint32_t * crc);

#endif /* SRC_CRC_API_CRC_H_ */
