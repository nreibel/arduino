#ifndef SRC_CRC_API_CRC_PRV_H_
#define SRC_CRC_API_CRC_PRV_H_

#include "crc_cfg.h"

#if CRC32_ALGO == CRC32_ALGO_CRC32Q
    #define CRC32_POLYNOMIAL 0x814141AB
    #define CRC32_INIT       0
    #define CRC32_XOROUT     0
#elif CRC32_ALGO == CRC32_ALGO_BZIP2
    #define CRC32_POLYNOMIAL 0x04C11DB7
    #define CRC32_INIT       0xFFFFFFFF
    #define CRC32_XOROUT     0xFFFFFFFF
#elif CRC32_ALGO == CRC32_ALGO_MPEG2
    #define CRC32_POLYNOMIAL 0x04C11DB7
    #define CRC32_INIT       0xFFFFFFFF
    #define CRC32_XOROUT     0
#elif CRC32_ALGO == CRC32_ALGO_POSIX
    #define CRC32_POLYNOMIAL 0x04C11DB7
    #define CRC32_INIT       0
    #define CRC32_XOROUT     0xFFFFFFFF
#elif CRC32_ALGO == CRC32_ALGO_XFER
    #define CRC32_POLYNOMIAL 0x000000AF
    #define CRC32_INIT       0
    #define CRC32_XOROUT     0
#else
    #error Unknown algorythm
#endif

#endif /* SRC_CRC_API_CRC_PRV_H_ */
