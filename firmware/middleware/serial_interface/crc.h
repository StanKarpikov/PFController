#ifndef __CRC_H__
#define __CRC_H__

#include <stdint.h>
#define CRC16_INITIAL_VALUE 0x1D0F
uint16_t crc16(unsigned char *pcBlock, unsigned char len);

#endif /* __CRC_H__ */
