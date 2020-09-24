#ifndef SRC_CRC_API_CRC_H_
#define SRC_CRC_API_CRC_H_

#include "types.h"

void CRC32_Init(uint32_t *crc);
void CRC32_Update(uint8_t b, uint32_t *crc);
void CRC32_Final(uint32_t *crc);

#endif /* SRC_CRC_API_CRC_H_ */
