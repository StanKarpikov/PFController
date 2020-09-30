#ifndef CRC_H_
#define CRC_H

#include <vector>
#include <QtGlobal>
#define CRC16_INITIAL_VALUE 0x1D0F

quint16 crc16(unsigned char *pcBlock, unsigned char len);
quint16 crc16(const std::vector<unsigned char> &block);

#endif /* CRC_H_ */
