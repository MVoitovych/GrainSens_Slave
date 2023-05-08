#include "math.h"

//#define BROADCAST_COMMAND 0xFF
//#define SET_ADDR_COMMAND 0xFE
//#define ANSWER_COMMAND 0xFD
//#define READ_TEMP_COMMAND 0x01
//#define SLEEP_COMMAND 0x02
//#define ERROR_COMMAND 0x11
//#define STOP_BYTE 0xAA

#define BROADCAST_COMMAND 'B'
#define SET_ADDR_COMMAND 'S'
#define ANSWER_COMMAND 'A'
#define READ_TEMP_COMMAND 'D'
#define SLEEP_COMMAND 0x02
#define ERROR_COMMAND 0x11
#define STOP_BYTE 0xAA

#define ADC_MAX_VALUE 4095

uint8_t getAddr(uint16_t ADC_value, uint8_t total_amount) {
  return (uint8_t)roundf((float )ADC_value * (float)total_amount /
                        (float)ADC_MAX_VALUE);
}