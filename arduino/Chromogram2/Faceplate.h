#ifndef FACEPLATE_H
#define FACEPLATE_H
#include <stdint.h>

class Faceplate{
  uint8_t *words;
  uint32_t *minute_leds;
  uint32_t *minutes_hack;
  uint8_t width;
  uint8_t height;
  uint8_t num_leds;
  uint8_t n_minute_led;
  uint8_t n_minute_hack;
  uint16_t (*XY_p)(uint8_t col, uint8_t row);
    
 public:
  char *name;
  Faceplate();
  Faceplate(uint8_t *words,
	    uint8_t *displays,
	    uint32_t *minute_leds,
	    uint32_t *minutes_hack,
	    char *name);
  uint8_t *displays;
  void setup(uint8_t width, uint8_t height, uint16_t (*XY_p)(uint8_t col, uint8_t row));
  void maskTime(uint32_t tm, bool* mask);
  void setMask(bool* mask, uint8_t row, uint8_t col, bool val);
  void getword(int i, uint8_t* out);
};

#endif
