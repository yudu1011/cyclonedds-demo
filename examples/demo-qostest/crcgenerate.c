#include <stdio.h>
#include <stdlib.h>
#include "TestData.h"
#define CRC8_POLY 0x07
// static uint8_t crc8table[256];
// static uint8_t crc = 0x00;

static void generate_crc8_table();
static uint8_t crc8_checktable(TestQoSModule_DataType *sample, uint8_t crc);

static void generate_crc8_table(uint8_t * crc8table, int size = 256) {
    for (int i = 0; i < 256; i++) {
        crc8table[i] = i;
        for (int j = 8; j != 0; j--) {
            if ((crc8table[i] & 0x80) != 0) {
                crc8table[i] = (crc8table[i] << 1) ^ CRC8_POLY;
            } else {
                crc8table[i] <<= 1;
            }
        }
    }
}
static uint8_t crc8_checktable(TestQoSModule_DataType *sample, uint8_t crc, uint8_t * crc8table, int size = 256) {
    crc = crc ^ 0xFF; // CRC初始化
    for (int pos = 0; pos < sample->payload._length; pos++) {
        crc = crc8table[(crc ^ sample->payload._buffer[pos]) & 0xFF];
    }
    return crc; // CRC结果取反
}
int main(){
    static uint8_t * crc8table;
    generate_crc8_table(crc8table);
    for (int i = 0; i < 16; ++i) {
        for (int j = 0; j < 16; ++j) {
                // 按照格式输出 CRC8 表格的值
                printf("%d ",crc8table[i+16*j]);
        }
    } 
}