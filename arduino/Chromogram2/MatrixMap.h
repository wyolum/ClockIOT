#ifndef MATRIXMAP_H
#define MATRIXMAP_H

#include <Arduino.h>

const uint8_t MatrixWidth = 32;
const uint8_t MatrixHeight = 8;
const uint16_t NUM_LEDS = MatrixWidth * MatrixHeight;

const extern uint16_t MatrixMap[MatrixHeight][MatrixWidth];

#endif
