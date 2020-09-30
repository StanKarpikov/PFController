#include "crc.h"

quint16 crc16(unsigned char *pcBlock, unsigned char len) {
    quint16 crc = CRC16_INITIAL_VALUE;          // initial value
    quint16 polynomial = 0x1021;   // 0001 0000 0010 0001  (0, 5, 12)

    for (int b=0;b< len;b++) {
        for (int i = 0; i < 8; i++) {
            quint8 bit = ((pcBlock[b]   >> (7-i) & 1) == 1);
            quint8 c15 = ((crc >> 15    & 1) == 1);
            crc <<= 1;
            if (c15 ^ bit) crc ^= polynomial;
        }
    }

    crc &= 0xffff;
    return crc;
}

quint16 crc16(const std::vector<unsigned char> &block) {
    quint16 crc = CRC16_INITIAL_VALUE;          // initial value
    quint16 polynomial = 0x1021;   // 0001 0000 0010 0001  (0, 5, 12)

    for (int b=0;b<block.size();b++) {
        for (int i = 0; i < 8; i++) {
            quint8 bit = ((block[b]   >> (7-i) & 1) == 1);
            quint8 c15 = ((crc >> 15    & 1) == 1);
            crc <<= 1;
            if (c15 ^ bit) crc ^= polynomial;
        }
    }

    crc &= 0xffff;
    return crc;
}
