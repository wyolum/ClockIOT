//#include <Time.h>
#include <Wire.h>
#include <FastLED.h>
#include <WiFiManager.h>
#include <EEPROMAnything.h>
#include <NTPClient.h>
#define ULTIM8x16
#include <MatrixMaps.h>
//#include <credentials.h>

#include "klok.h"
#include "textures.h"
#include "logic.h"
#include "Faceplate.h"
#include "get_time.h"

#include "dutch_v1.h"
#include "french_v1.h"
#include "german_v3.h"
#include "hebrew_v1.h"
#include "hungarian_v2.h"
#include "irish_v1.h"
#include "italian_v1.h"
#include "english_v3.h"
#include "spanish_v1.h"

#include "config.h"

//const bool ON = true;
//const bool OFF = !ON;

// How many leds are in the strip?
const uint8_t N_BOARD = 2;
const uint8_t NUM_LEDS = 64 * N_BOARD;
bool mask[NUM_LEDS];
bool wipe[NUM_LEDS];
CRGB leds[NUM_LEDS];

#define DATA_PIN     MOSI
#define CLK_PIN      SCK
#define COLOR_ORDER BGR
#define LED_TYPE APA102
#define MILLI_AMPS 1000  // IMPORTANT: set the max milli-Amps of your power supply (4A = 4000mA)

//********************************************************************************
// Displays
typedef void (*Init)();
typedef void (*Background)();
typedef void (*Transition)(uint16_t last_time_inc, uint16_t time_inc);

NTPClock ntp_clock;
DS3231Clock ds3231_clock;

WiFiManager wifiManager;
WiFiUDP ntpUDP;
Faceplate faceplates[] = {
  english_v3,
  spanish_v1
};
uint8_t num_faceplates = 2;

NTPClient timeClient(ntpUDP, "us.pool.ntp.org", 0, 60000);
Klok klok(faceplates[0], timeClient);

typedef struct{
  Init       init;
  Background background;
  Transition transition;
  String     name;
  int        id;
} Display;

typedef Display Displays[];

void TheMatrix(uint16_t last_time_inc, uint16_t time_inc);
void wipe_around(bool val){
}
void wipe_around_transition(uint16_t last_time_inc, uint16_t time_inc);
void word_drop(uint16_t last_time_inc, uint16_t time_inc);

void next_time(uint16_t last_time_inc, uint16_t time_inc){
}

Display PlainDisplay = {blend_to_rainbow, rainbow, next_time, String("Plain"), 0};
/*
Display WordDropDisplay = {blend_to_rainbow, rainbow, word_drop, String("Word Drop"), 1};
Display WipeAroundDisplay = {blend_to_rainbow, rainbow, wipe_around_transition, String("Wipe Around"), 2};
Display TheMatrixDisplay = {blend_to_blue, fill_blue, TheMatrix, String("The Matrix"), 3};
*/

//--------------------------------------------------------------------------------
//uint32_t current_time;
void blend_to_rainbow(){
  int i;
  CHSV newcolor;
  uint32_t current_time = Now();
  //current_time = Now();
  int count = ((current_time % 300) * 255) / 300;
  
  newcolor.val = 255;
  newcolor.sat = 255;
  for(int ii=0; ii<NUM_LEDS; ii++){
    for( int row = 0; row < MatrixHeight; row++) {
      for( int col = 0; col < MatrixWidth; col++) {
	i = XY(col, row);
	if(mask[i]){
	  newcolor.hue =  (count + (MatrixWidth * row + col) * 2) % 256;
	  nblend(leds[XY(col, row)], newcolor, 1);
	}
      }
    }
    FastLED.show();
    delay(1);
  }
}

void blend_to_color(CRGB color){
  for(int kk=0; kk<128; kk++){
    for(int ii=0; ii<NUM_LEDS; ii++){
      if(mask[ii]){
	nblend(leds[ii], color, 1);
      }
    }
    FastLED.show();
    delay(1);
  }
}

void blend_to_red(){
  blend_to_color(CRGB::Red);
}

void blend_to_green(){
  blend_to_color(CRGB::Green);
}

void blend_to_blue(){
  blend_to_color(CRGB::Blue);
}

void fill_red(){
  fill_solid(leds, NUM_LEDS, CRGB::Red);
}
void fill_green(){
  fill_solid(leds, NUM_LEDS, CRGB::Green);
}
void fill_blue(){
  fill_solid(leds, NUM_LEDS, CRGB::Blue);
}
void noop(){
}

void rainbow() {
  int i, dx, dy;
  CHSV hsv;
  float dist;
  
  hsv.hue = 0;
  hsv.val = 255;
  hsv.sat = 240;
  uint32_t current_time = Now();
  int count = ((current_time % 300) * 255) / 300;
  for( int row = 0; row < MatrixHeight; row++) {
    for( int col = 0; col < MatrixWidth; col++) {
      dy = (row - 4) * 2;
      dx = col - 8;
      dist = sqrt(dx * dx + dy * dy);
      i = XY(col, row);
      //hsv.hue =  ((int)(dist * 16) - count) % 256;
      hsv.hue =  (count + (MatrixWidth * row + col) * 2) % 256;
      leds[i] = hsv;
    }
  }
  // Show the leds (only one of which is set to white, from above)
  //delay(100);
}
// end Displays
//********************************************************************************

uint8_t logo_rgb[] = {
  0x11,0x00,0x29,0x00,0x25,0x00,0x23,0x00,0x25,0x00,0x29,0x00,0x31,0x00,0xe0,0x01,
  0x00,0x03,0x80,0x04,0x80,0x04,0x80,0x04,0x80,0x04,0x00,0x03,0x00,0x00,0x00,0x00,
  0x11,0x00,0x09,0x88,0x05,0x48,0x03,0x28,0x05,0x18,0x09,0x28,0x11,0x48,0x00,0x88
};


struct config_t{
  int timezone;
  long alarm;
  int mode;
  uint8_t brightness;
  uint8_t display_idx;
} configuration;

void display_bitmap_rgb(uint8_t* bitmap){
  uint8_t n = 16;
  uint8_t h = 8;
  uint8_t w = 16;
  uint8_t xy[2];
  uint8_t r, g, b;
  
  int i, j;
  int led_idx, byte_idx;

  //struct CRGB color;
  int x, y;
  
  for(i=0; i<n; i++){
    r = bitmap[i + 0 * w];
    g = bitmap[i + 1 * w];
    b = bitmap[i + 2 * w];
    //   012345678pabcdef
    // 0 0000000011111111
    // 1 2222222233333333
    // 2 4444444455555555
    // 3 6666666677777777
    // 4 8888888899999999
    // 5 aaaaaaaabbbbbbbb
    // 6 ccccccccdddddddd
    // 7 eeeeeeeeffffffff
    for(j=0; j<8; j++){
      x = (i * 8 + j) % 16;
      y = i / 2;
      led_idx = XY(x, y);
      leds[led_idx].red   = 255 * ((r >> j) & 1);
      leds[led_idx].green = 255 * ((g >> j) & 1);
      leds[led_idx].blue  = 255 * ((b >> j) & 1);
      if(leds[led_idx].red || leds[led_idx].green || leds[led_idx].blue){
	mask[led_idx] = true;
      }
    }
  }
}

void apply_mask(bool* mask){
  uint16_t b, k;
  for(uint16_t i=0; i < NUM_LEDS; i++){
    if(!mask[i]){
      leds[i] = CRGB::Black;
    }
  }
}

void fillMask(bool* mask, bool b){
  fillMask(mask, b, 0, NUM_LEDS);
}

void fillMask(bool* mask, bool b, int start, int stop){
  for(int i = start; i < stop && i < NUM_LEDS; i++){
    mask[i] = b;
  }
}

void loadSettings(){
  EEPROM_readAnything(0, configuration);  
}

void saveSettings(){
  EEPROM_writeAnything(0, configuration);
  EEPROM.commit();
}

uint16_t XY( uint8_t x, uint8_t y){
  uint16_t out = 0;
  if(FLIP_DISPLAY){
    x = MatrixWidth - x - 1;
    y = MatrixHeight - y - 1;
  }
  if(x < MatrixWidth && y < MatrixHeight){
    out = MatrixMap[y][x];
  }
  return out;
}

void led_setup(){
  FastLED.addLeds<LED_TYPE, DATA_PIN, CLK_PIN, COLOR_ORDER>(leds, NUM_LEDS);
  FastLED.setDither(true);
  FastLED.setCorrection(TypicalLEDStrip);
  FastLED.setMaxPowerInVoltsAndMilliamps(5, MILLI_AMPS);
  fill_solid(leds, NUM_LEDS, CRGB::Black);
  FastLED.show();

  FastLED.setBrightness(10);
  //FastLED.setBrightness(configuration.brightness);
  wipe_around(ON);
  display_bitmap_rgb(logo_rgb);
  wipe_around(OFF);
  display_bitmap_rgb(logo_rgb);
  FastLED.show();
}

void setup(){
  Wire.begin();
  Serial.begin(115200);
  delay(200);
  Serial.println("setup() starting");
  //Serial.println(year(0));
  loadSettings();
  led_setup();

  for(int ii = 0; ii < num_faceplates; ii++){
    faceplates[ii].setup(MatrixWidth, MatrixHeight, XY);
  }

  
#ifdef USE_CREDENTIALS
  WiFi.begin(ssid, password);

  while ( WiFi.status() != WL_CONNECTED ) {
    delay ( 500 );
    Serial.print ( "." );
  }
#else
  wifiManager.autoConnect("KLOK");
  Serial.println("yay connected");
#endif
  ntp_clock.setup(&timeClient);
  //timeClient.begin();
  //timeClient.setTimeOffset(-240 * 60);
  ds3231_clock.setup();
  ds3231_clock.now();
  Serial.println("setup() complete");
}

uint32_t count;
uint32_t Now(){
  timeClient.update();
  return ntp_clock.now();
}

void loop(){
  uint8_t word[3];
  uint32_t current_time = Now();

  fillMask(mask, OFF);
  faceplates[0].maskTime(current_time, mask);
  rainbow(leds, current_time, XY);
  apply_mask(mask);
  FastLED.show();
  Serial.print(timeClient.getHours());
  Serial.print(":");
  Serial.print(timeClient.getMinutes());
  Serial.print(":");
  Serial.print(timeClient.getSeconds());
  Serial.println("");
  Serial.println(ds3231_clock.hours());
  Serial.println(ds3231_clock.minutes());
  Serial.println(ds3231_clock.seconds());
  delay(1000);
}
