#ifndef CRC_H_
#define CRC_H

#include <vector>
#include <QtGlobal>
#define CRC16_INITIAL_VALUE 0x1D0F

uint16_t crc16(uint8_t *pcBlock, uint8_t len);
uint16_t crc16(const std::vector<uint8_t> &block);

#endif /* CRC_H_ */
