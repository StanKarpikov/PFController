#include "crc.h"

uint16_t crc16(uint8_t *pcBlock, uint8_t len)
{
    uint16_t crc = CRC16_INITIAL_VALUE;  // initial value
    uint16_t polynomial = 0x1021;        // 0001 0000 0010 0001  (0, 5, 12)

    for (int b = 0; b < len; b++)
    {
        for (int i = 0; i < 8; i++)
        {
            quint8 bit = ((pcBlock[b] >> (7 - i) & 1) == 1);
            quint8 c15 = ((crc >> 15 & 1) == 1);
            crc <<= 1;
            if (c15 ^ bit) crc ^= polynomial;
        }
    }

    crc &= 0xffff;
    return crc;
}

uint16_t crc16(const std::vector<uint8_t> &block)
{
    uint16_t crc = CRC16_INITIAL_VALUE;  // initial value
    uint16_t polynomial = 0x1021;        // 0001 0000 0010 0001  (0, 5, 12)

    for (int b = 0; b < block.size(); b++)
    {
        for (int i = 0; i < 8; i++)
        {
            quint8 bit = ((block[b] >> (7 - i) & 1) == 1);
            quint8 c15 = ((crc >> 15 & 1) == 1);
            crc <<= 1;
            if (c15 ^ bit) crc ^= polynomial;
        }
    }

    crc &= 0xffff;
    return crc;
}
