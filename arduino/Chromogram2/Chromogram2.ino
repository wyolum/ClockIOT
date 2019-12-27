#include <Time.h>
#include <Wire.h>
#include <WiFiManager.h>
#include <FastLED.h>
#include <PubSubClient.h>
#include <EEPROM.h>
#include <EEPROMAnything.h>
#include <NTPClient.h>
#include <WebSocketsServer.h>

#define CHROMOGRAM
#include <HTTPClient.h>
#include <credentials.h>

#include "MatrixMap.h"
#include "klok.h"
#include "textures.h"
#include "logic.h"
#include "Faceplate.h"
#include "get_time.h"

#include "english_v3.h"

#include "config.h"
struct config_t{
  int timezone;
  uint8_t brightness;
  uint8_t display_idx;
  bool factory_reset;
  bool use_wifi;
  bool use_ip_timezone;
  byte mqtt_ip[4];
  bool flip_display;
  uint32_t last_tz_lookup; // look up tz info every Sunday at 3:00 AM
  uint8_t solid_color_rgb[3];
  bool use_ntp_time;
  bool wifi_reset;
  uint8_t faceplate_idx;
} config;

bool setup_complete = false;

void print_config(){
  Serial.println("config:");
  Serial.print("    timezone:"); Serial.println(config.timezone);
  Serial.print("    brightness:"); Serial.println(config.brightness);
  Serial.print("    display_idx:"); Serial.println(config.display_idx);
  Serial.print("    factory_reset:"); Serial.println(config.factory_reset);
  Serial.print("    use_wifi:"); Serial.println(config.use_wifi);
  Serial.print("    use_ip_timezone:"); Serial.println(config.use_ip_timezone);
  Serial.print("    mqtt_ip:");
  for(int ii = 0; ii < 4; ii++){
    if (ii > 0){
      Serial.print(".");
    }
    Serial.print(config.mqtt_ip[ii]);
  }
  Serial.println();
  Serial.print("    flip_display:"); Serial.println(config.flip_display);
  Serial.print("    last_tz_lookup:"); Serial.println(config.last_tz_lookup);
  Serial.print("    solid_color_rgb:");
  for(int ii = 0; ii < 3; ii++){
    if (ii > 0){
      Serial.print(".");
    }
    Serial.print(config.solid_color_rgb[ii]);
  }
  Serial.println();
  Serial.print("    use_ntp_time:"); Serial.println(config.use_ntp_time);
  Serial.print("    wifi_reset:"); Serial.println(config.wifi_reset);
  Serial.print("    faceplate_idx:"); Serial.println(config.faceplate_idx);
}
bool force_update = false;

const bool ON = true;
const bool OFF = !ON;

// How many leds are in the strip?

bool mask[NUM_LEDS];
bool wipe[NUM_LEDS];
CRGB leds[NUM_LEDS];

WiFiClient espClient;
PubSubClient mqtt_client(espClient);
void  interact_loop();

#ifdef CLOCKIOT
#define DATA_PIN     4
#define CLK_PIN      16
#else
#define DATA_PIN     MOSI
#define CLK_PIN      SCK
#endif

#define COLOR_ORDER BGR
#define LED_TYPE APA102
#define MILLI_AMPS 500  // IMPORTANT: set the max milli-Amps of your power supply (4A = 4000mA)

uint32_t last_time;

//********************************************************************************
// Displays

void my_show(){
  FastLED.show();
  if(setup_complete){
    interact_loop();
  }
}

typedef void (*Init)();
typedef void (*DisplayTime)(uint32_t last_tm, uint32_t tm);

NTPClock ntp_clock;
DS3231Clock ds3231_clock;
DoomsdayClock doomsday_clock;

WiFiManager wifiManager;
WiFiUDP ntpUDP;
Faceplate Faceplates[] = {
  english_v3, // in the morning etc.
};

uint8_t N_FACEPLATE = 1;

uint8_t DEFAULT_FACEPLATE_IDX = 0;

NTPClient timeClient(ntpUDP, "us.pool.ntp.org", 0, 60000); //Default
//NTPClient timeClient(ntpUDP, "cn.pool.ntp.org", 0, 60000); //for china?
//NTPClient timeClient(ntpUDP, "XXX.pool.ntp.org", 0, 60000);//no NTP TEST
Klok klok(Faceplates[0], timeClient);

// return json value for name identified (or "not found")
String jsonLookup(String s, String name){
  String out;
  int start, stop;
  
  name = String("\"") + name + String("\""); // add bounding quotes

  start = s.indexOf(name);
  if(start < 0){
    out = String("not found");
  }
  else{
    start +=  name.length() + 2;
    stop = s.indexOf('"', start);
    out = s.substring(start, stop);
  }
  return out;
}

void set_timezone_from_ip(){

  HTTPClient http;
  String payload;
  
  Serial.print("[HTTP] begin...\n");
  // configure traged server and url
  //http.begin("https://www.howsmyssl.com/a/check", ca); //HTTPS
  // http.begin("http://example.com/index.html"); //HTTP

  //http.begin("https://timezoneapi.io/api/ip");// no longer works!
  String url = String("https://www.wyolum.com/utc_offset/utc_offset.py") +
    String("?refresh=") + String(millis()) +
    String("&localip=") +
    String(WiFi.localIP()[0]) + String('.') + 
    String(WiFi.localIP()[1]) + String('.') + 
    String(WiFi.localIP()[2]) + String('.') + 
    String(WiFi.localIP()[3]) + String('&') +
    String("macaddress=") + WiFi.macAddress() + String('&') + 
    String("dev_type=ClockIOT");
  Serial.println(url);
  http.begin(url);
  
  Serial.print("[HTTP] GET...\n");
  // start connection and send HTTP header
  int httpCode = http.GET();
  
  // httpCode will be negative on error
  //httpCode = -1; // force error
  if(httpCode < 0){
    http.end();
    url = String("https://ipapi.co/json/?key=");
    Serial.print("Using backup url:");
    Serial.println(url + "<SECRET KEY>");
    http.begin(url + ipapikey);
  
    Serial.print("[HTTP] GET...\n");
    // start connection and send HTTP header
    httpCode = http.GET();
  }
  payload = http.getString();
  if(httpCode > 0) {
    // HTTP header has been send and Server response header has been handled
    Serial.printf("[HTTP] GET... code: %d\n", httpCode);
    
    // file found at server
    //String findme = String("offset_seconds");
    if(httpCode == HTTP_CODE_OK) {
      Serial.print("payload:");
      Serial.println(payload);
      payload.replace(" ", "");
      String offset_str = jsonLookup(payload, String("utc_offset"));
      int hours = offset_str.substring(0, 3).toInt();
      int minutes = offset_str.substring(3, 5).toInt();
      if(hours < 0){
	minutes *= -1;
      }
      int offset = hours * 3600 + minutes * 60;

      String utc_str =jsonLookup(payload, String("utc"));
      Serial.print("  UTC:"); Serial.println(utc_str);
      if(!utc_str.equals("not found")){
	uint32_t local = utc_str.toInt() + offset;
	Serial.print("Local:");Serial.println(local);
	ds3231_clock.set(local);
      }
      if(doomsday_clock.master->initialized){
	Serial.println("NTP is clock alive!");
      }
      else{
	Serial.println("No NTP, fall back to DS3231");
      }
      Serial.println();
      if(config.use_ip_timezone){
	Serial.print("timezone_offset String:");
	Serial.println(offset_str);
	Serial.print("timezone_offset:");
	Serial.println(offset);
	set_timezone_offset(offset);
	config.last_tz_lookup = doomsday_clock.gmt();
	saveSettings();
      }
      else{
	Serial.println("Using previously selected timezone");
      }
    }
    else{
      Serial.println("No timezone found");
      Serial.println("Payload:");
      Serial.println(payload);
    }
  }
}

void setPixel(byte row, byte col, const struct CRGB & color){
  int pos = XY(col, row);
  leds[pos] = color;
}
void setPixelMask(bool* mask, uint8_t row, uint8_t col, bool b){
  if(row >= MatrixHeight){
  }
  else if(col >= MatrixWidth){
  }
  else{
    uint16_t pos = XY(col, row);
    if(pos < NUM_LEDS){
      mask[pos] = b;
    }
  }
}

void wipe_around(bool val){
  float dtheta = 31.4 / 180;
  float theta = -3.14 - dtheta;
  int row, col;
  bool tmp[NUM_LEDS];

  int cx = random(0, MatrixWidth-1);
  int cy = random(0, MatrixHeight-1);
  cx = 8;
  cy = 4;
  
  fillMask(wipe, !val); // 
  while (theta < 3.14 + dtheta){
    for(row=0; row < MatrixHeight; row++){
      for(col=0; col < MatrixWidth; col++){
	if(atan2(row - cy, col - cx) < theta){
	  setPixelMask(wipe, row, col, val); // set wedge to val
	}
      }
    }
    if(val){
      logical_or(NUM_LEDS, wipe, mask, tmp);
    }
    else{
      logical_and(NUM_LEDS, wipe, mask, tmp);
    }
    apply_mask(tmp);
    my_show();
    theta += dtheta;
    delay(10);
  }
  
}

typedef struct{
  Init       init;           // called when display changes
  DisplayTime display_time;  // called in main loop to update time display (if needed)
  String     name;
  int        id;
} Display;

void Plain_init(){
  return;
  while(1){
    fill_blue();
    fillMask(mask, ON);
    wipe_around(ON);
    Serial.println("HERE");
    delay(500);

    wipe_around(OFF);
    my_show();
    delay(500);
  }
}
void Plain_display_time(uint32_t last_tm, uint32_t tm){
  fillMask(mask, OFF);
  Faceplates[config.faceplate_idx].maskTime(tm, mask);
  rainbow_fast();
  apply_mask(mask);
}

Display *CurrentDisplay_p;
Display PlainDisplay = {Plain_init, Plain_display_time, String("Plain"), 0};
Display TheMatrixDisplay = {TheMatrix_init, TheMatrix_display_time, String("The Matrix"), 1};
Display WordDropDisplay = {WordDrop_init, WordDrop_display_time, String("Word Drop"), 2};
Display SolidColorDisplay = {SolidColor_init, SolidColor_display_time, String("Solid Color"), 3};
Display CloudDisplay = {Cloud_init, Cloud_display_time, String("Cloud"), 4};
Display FireDisplay = {Fire_init, Fire_display_time, String("Fire"), 5};
Display GlitchDisplay = {Glitch_init, Glitch_display_time, String("Glitch"), 6};

const uint8_t N_DISPLAY = 7;
Display Displays[N_DISPLAY] = {PlainDisplay, TheMatrixDisplay, WordDropDisplay, SolidColorDisplay,
			       CloudDisplay, FireDisplay, GlitchDisplay};

/*
Display WipeAroundDisplay = {blend_to_rainbow, rainbow, wipe_around_transition, String("Wipe Around"), 3};
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
    my_show();
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
    my_show();
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
void fill_white(){
  fill_solid(leds, NUM_LEDS, CRGB::White);
}
void fill_black(){
  fill_solid(leds, NUM_LEDS, CRGB::Black);
}
void noop(){
}

void setWordMask(bool *mask, uint8_t* word, bool b){
  // word = [row, col, len]
  for(int i=0; i < word[2]; i++){
    setPixelMask(mask, word[1], word[0] + i, b);
  }
}


void WordDrop_init(){
  uint32_t current_time = Now();
  last_time = current_time;
  fillMask(mask, OFF);
  Faceplates[config.faceplate_idx % N_FACEPLATE].maskTime(current_time, mask);
  //blend_to_rainbow();// resets ESP32??
}

void word_drop_in(uint16_t time_inc){
  uint8_t bits;     // holds the on off state for 8 words at a time
  uint8_t word[3];  // start columm, start row, length of the current word
  bool tmp_mask[NUM_LEDS];
  uint8_t tmp_word[3];
  uint8_t n_byte_per_display = Faceplates[config.faceplate_idx % N_FACEPLATE].displays[0];
  
  fillMask(mask, false);
  fillMask(wipe, false);
  fillMask(tmp_mask, false);
  
  for(uint8_t j = 0; j < n_byte_per_display; j++){ // j is a byte index 
    // read the state for the next set of 8 words
    bits = pgm_read_byte(Faceplates[config.faceplate_idx % N_FACEPLATE].displays + 1 + (time_inc * n_byte_per_display) + j);
    for(uint8_t k = 0; k < 8; k++){                     // k is a bit index
      if((bits >> k) & 1){                              // check to see if word is on or off
	Faceplates[config.faceplate_idx % N_FACEPLATE].getword(j * 8 + k, word);                       // if on, read location and length
	tmp_word[0] = word[0];
	tmp_word[1] = word[1];
	tmp_word[2] = word[2];
	for(int rr = 0; rr <= word[1]; rr++){
	  tmp_word[1] = rr;
	  setWordMask(wipe, tmp_word, true);
	  logical_or(NUM_LEDS, mask, wipe, tmp_mask);
	  rainbow_slow();
	  apply_mask(tmp_mask);
	  my_show();
	  delay(25);
	}
	setWordMask(mask, word, true);
	for(int rr = 0; rr < word[1]; rr++){
	  tmp_word[1] = rr;
	  setWordMask(wipe, tmp_word, false);
	  logical_or(NUM_LEDS, mask, wipe, tmp_mask);
	  
	  rainbow_slow();
	  apply_mask(tmp_mask);
	  my_show();
	  delay(25);
	}
      }
    }
  }
}
void word_drop_out(uint16_t time_inc){
  uint8_t bits;     // holds the on off state for 8 words at a time
  uint8_t word[3];  // start columm, start row, length of the current word
  bool tmp_mask[NUM_LEDS];
  uint8_t tmp_word[3];
  uint8_t n_byte_per_display = Faceplates[config.faceplate_idx % N_FACEPLATE].displays[0];
  
  //fillMask(mask, false);
  //fillMask(wipe, false);
  //fillMask(tmp_mask, false);
  logical_copy(NUM_LEDS, mask, wipe);
  logical_copy(NUM_LEDS, mask, tmp_mask);
  
  for(uint8_t j = 0; j < n_byte_per_display; j++){ // j is a byte index 
    // read the state for the next set of 8 words
    bits = pgm_read_byte(Faceplates[config.faceplate_idx % N_FACEPLATE].displays + 1 + (time_inc * n_byte_per_display) + j);
    for(uint8_t k = 0; k < 8; k++){                     // k is a bit index
      if((bits >> k) & 1){                              // check to see if word is on or off
	Faceplates[config.faceplate_idx % N_FACEPLATE].getword(j * 8 + k, word);                       // if on, read location and length
	tmp_word[0] = word[0];
	tmp_word[1] = word[1];
	tmp_word[2] = word[2];
	for(int rr = word[1]; rr <= 8; rr++){
	  tmp_word[1] = rr;
	  setWordMask(wipe, tmp_word, true);
	  logical_or(NUM_LEDS, mask, wipe, tmp_mask);
	  rainbow_slow();
	  apply_mask(tmp_mask);
	  my_show();
	  delay(25);
	}
	setWordMask(mask, word, false);
	for(int rr = word[1]; rr <= 8; rr++){
	  tmp_word[1] = rr;
	  setWordMask(wipe, tmp_word, false);
	  logical_or(NUM_LEDS, mask, wipe, tmp_mask);
	  
	  rainbow_slow();
	  apply_mask(tmp_mask);
	  my_show();
	  delay(25);
	}
      }
    }
  }
}

void word_drop(uint16_t last_time_inc, uint16_t time_inc){
  bool tmp_d[NUM_LEDS];

  rainbow_slow();

  // swipe rainbow from the left
  //wipe_around(ON);
  //delay(1000);
  if(last_time_inc != 289){
    word_drop_out(last_time_inc);
  }
  
  // clear the new display
  fillMask(tmp_d, false);
  
  // read display for next time incement
  Faceplates[config.faceplate_idx % N_FACEPLATE].maskTime(time_inc * 300, mask);
  
  // clear rainbow to reveal the time
  //wipe_off_left();
  //wipe_around(OFF);
  word_drop_in(time_inc);
}

void WordDrop_display_time(uint32_t last_tm, uint32_t next_tm){
  int last_tm_inc = (last_tm / 300) % 288;
  int      tm_inc = (next_tm / 300) % 288;
  if(last_tm_inc == tm_inc - 1 || (last_tm_inc == 287 && tm_inc == 0)){
    word_drop(last_tm_inc, tm_inc);
  }
  rainbow_slow();
  fillMask(mask, false);
  Faceplates[config.faceplate_idx % N_FACEPLATE].maskTime(next_tm, mask);  
  apply_mask(mask);
}

void TheMatrix_drop(uint32_t last_tm_inc, uint32_t current_tm_inc){
  int n_drop = 0;
  int n_need = 8;
  
  const struct CRGB color = CRGB::Green;
  uint8_t cols[NUM_LEDS];
  uint8_t rows[NUM_LEDS];
  uint8_t pause[NUM_LEDS];
  bool have[NUM_LEDS];
  int col;
  int i, j;

  Serial.print("TheMatrix: Change Time\n");
  // clear all masks
  fillMask(mask, false);
  fillMask(wipe, false);
  fillMask(have, false);

  // set masks to appropriate times

  Faceplates[config.faceplate_idx % N_FACEPLATE].maskTime(last_tm_inc * 300 + 4 * 60, mask);
  apply_mask(mask);
  my_show();
  blend_to_green();
  Faceplates[config.faceplate_idx % N_FACEPLATE].maskTime(current_tm_inc * 300, wipe);
  //fill_green();
    
  for(i=0; i < MatrixWidth; i++){
    for(j=0; j < MatrixHeight; j++){
      if(leds[XY(i, j)].red > 0 ||
	 leds[XY(i, j)].green > 0 ||
	 leds[XY(i, j)].blue > 0){
	rows[n_drop] = j;
	cols[n_drop] = i;
	n_drop++;
      }
      if(wipe[XY(i, j)]){
	n_need++;
      }
    }
  }
  
  delay(10);
  for(j = 0; j < 255 * 3; j++){
    for(i=0; i < NUM_LEDS; i++){
      leds[i].red   = blend8(leds[i].red, 0, 1);
      leds[i].green = blend8(leds[i].green, 255, 1);
      leds[i].blue  = blend8(leds[i].blue, 0, 1);
    }
    apply_mask(mask);
    my_show();
    delay(5);
  }

  for(i = n_drop; i < n_need; i++){/// add enough drops to complete
    cols[i] = random(0, MatrixWidth);
    rows[i] = -random(0, MatrixHeight);
    n_drop++;
  }

  int end = millis() + 5000; // go for 5 seconds
  // while new display is not filled out
  while(!logical_equal(NUM_LEDS, wipe, have)){
    //  while(millis() < end){
    fadeToBlackBy(leds, NUM_LEDS, 75);
    for(i = 0; i < n_drop; i++){
      if(millis() > end && wipe[XY(cols[i], rows[i])]){
	if(random(0, 3) == 0){
	  have[XY(cols[i], rows[i])] = true;
	}
      }
      
      if(random(0, 16) == 0){ // pause at random times
	pause[i] = random(6, 9); // for random duration
      }
      if(pause[i] == 0){
	rows[i]++;
      }
      else{
	pause[i]--; 
      }
      if(rows[i] > MatrixHeight - 1){
	if(n_drop > n_need){
	  for(j = i; j < n_drop; j++){ // slide drops down by one
	    rows[j] = rows[j + 1];
	    cols[j] = cols[j + 1];
	  }
	  n_drop--;
	  Serial.print("n_drop:");
	  Serial.println(n_drop);
	}
	else{
	  rows[i] = -random(0, MatrixHeight);
	  cols[i] = random(0, MatrixWidth);
	}
      }
      if(0 <= rows[i] && rows[i] <  MatrixHeight){
	leds[XY(cols[i], rows[i])] = color;
      }
    }

    for(int ii = 0; ii < NUM_LEDS; ii++){
      if(have[ii]){
	//leds[ii] = CRGB::Blue;
	leds[ii] = CRGB(config.solid_color_rgb[0],
			config.solid_color_rgb[1],
			config.solid_color_rgb[2]);
      }
    }
    my_show();
    delay(75);
  }
  for(int ii=0; ii< MatrixHeight * 10; ii++){
    //  while(millis() < end){
    fadeToBlackBy(leds, NUM_LEDS, 75);
    for(i = 0; i < n_drop; i++){
      rows[i]++;
      if(0 <= rows[i] && rows[i] <  MatrixHeight){
	leds[XY(cols[i], rows[i])] = color;
      }
    }
    for(int ii = 0; ii < NUM_LEDS; ii++){
      if(have[ii]){
	//leds[ii] = CRGB::Blue;
	leds[ii] = CRGB(config.solid_color_rgb[0],
			config.solid_color_rgb[1],
			config.solid_color_rgb[2]);
      }
    }
    my_show();
    delay(75);
  }
}

void TheMatrix_init(){
  uint32_t current_time = Now();
  last_time = current_time;
  fill_solid(leds, NUM_LEDS, CRGB(config.solid_color_rgb[0],
				  config.solid_color_rgb[1],
				  config.solid_color_rgb[2]));
  fillMask(mask, false);
  Faceplates[config.faceplate_idx % N_FACEPLATE].maskTime(current_time, mask);  
  apply_mask(mask);
}

void TheMatrix_display_time(uint32_t last_tm, uint32_t tm){

  int last_tm_inc = (last_tm / 300) % 288;
  int      tm_inc = (     tm / 300) % 288;

  if(last_tm_inc == tm_inc - 1 || (last_tm_inc == 287 && tm_inc == 0)){
    TheMatrix_drop(last_tm_inc, tm_inc);
  }
  else{
    /* only update when needed? what about color updates?*/
    fill_solid(leds, NUM_LEDS, CRGB(config.solid_color_rgb[0],
				    config.solid_color_rgb[1],
				    config.solid_color_rgb[2]));
    fillMask(mask, false);
    Faceplates[config.faceplate_idx % N_FACEPLATE].maskTime(tm, mask);  
    apply_mask(mask);
  }
  /* // just do it every cycle?
  fill_solid(leds, NUM_LEDS, CRGB(config.solid_color_rgb[0],
				  config.solid_color_rgb[1],
				  config.solid_color_rgb[2]));
  fillMask(mask, false);
  Faceplates[config.faceplate_idx % N_FACEPLATE].maskTime(tm, mask);  
  apply_mask(mask);
  */
}

void rainbow_cycle(int count){
  int i, dx, dy;
  CHSV hsv;
  float dist;
  
  hsv.hue = 0;
  hsv.val = 255;
  hsv.sat = 240;

  for( int row = 0; row < MatrixHeight; row++) {
    for( int col = 0; col < MatrixWidth; col++) {
      // dx, dy, dist used for radial pattern, not used here
      dy = (row - 4) * 2;
      dx = col - 8;
      dist = sqrt(dx * dx + dy * dy);
      i = XY(col, row);
      //hsv.hue =  ((int)(dist * 16) - count) % 256;
      hsv.hue =  (count + (MatrixWidth * row + col) * 2) % 256;
      leds[i] = hsv;
    }
  }
}

void rainbow_fast() {
  uint32_t current_time = Now();
  int count = millis() / 100;
  
  rainbow_cycle(millis()/25);
  // Show the leds (only one of which is set to white, from above)
  //delay(100);
}

void rainbow_slow() {
  uint32_t current_time = Now();
  int count = ((current_time % 300) * 255) / 300;
  rainbow_cycle(count);
}

void SolidColor_init(){
}
void SolidColor_display_time(uint32_t last_tm, uint32_t tm){
  if(last_tm != tm || force_update){
    force_update = false;
    fill_solid(leds, NUM_LEDS, CRGB(config.solid_color_rgb[0],
				    config.solid_color_rgb[1],
				  config.solid_color_rgb[2]));
    fillMask(mask, false);
    Faceplates[config.faceplate_idx % N_FACEPLATE].maskTime(last_tm, mask);  
    apply_mask(mask);
  }
}

void Cloud_init(){
}
void cloudNoise(void);
void Cloud_transition(uint32_t last_tm, uint32_t tm){
  // bring fire up!
  fillMask(mask, false);
  Faceplates[config.faceplate_idx % N_FACEPLATE].maskTime(last_tm, mask);
  for(int mask_to=0; mask_to<16; mask_to++){
    for(int row=0; row < 8; row++){
      setPixelMask(mask, row, mask_to, true);
    }
    for(int ii=0; ii<1; ii++){
      cloudNoise();
      apply_mask(mask);
      my_show();
      delay(40);
    }
  }
  
  fillMask(mask, true);
  // bring fire out!
  for(int fill_to=0; fill_to<16; fill_to++){
    for(int row=0; row < 8; row++){
      setPixelMask(mask, row, fill_to, false);
    }
    Faceplates[config.faceplate_idx % N_FACEPLATE].maskTime(tm, mask);
    for(int ii=0; ii<1; ii++){
      cloudNoise();
      apply_mask(mask);
      my_show();
      delay(40);
    }
  }
}
void Cloud_display_time(uint32_t last_tm, uint32_t tm){
  /*
  fill_white();
  fillMask(mask, false);
  Faceplates[config.faceplate_idx % N_FACEPLATE].maskTime(last_tm, mask);  
  apply_mask(mask, CRGB::Blue)
  */
  if(last_tm / 300 != tm / 300){
    Cloud_transition(last_tm, tm);
  }
  EVERY_N_MILLISECONDS(40){// prevent clouds from moving too fast!
    cloudNoise();
    fillMask(mask, false);
    Faceplates[config.faceplate_idx % N_FACEPLATE].maskTime(tm, mask);  
    apply_mask(mask);
  }
  /*
  */
}
void Fire_init(){
}
void fireNoise(void);
void fireNoise2(void);
void Fire_transition(uint32_t last_tm, uint32_t tm){
  // bring fire up!
  fillMask(mask, false);
  Faceplates[config.faceplate_idx % N_FACEPLATE].maskTime(last_tm, mask);
  for(int mask_to=7; mask_to>=0; mask_to--){
    for(int col=0; col < 16; col++){
      setPixelMask(mask, mask_to, col, true);
    }
    for(int ii=0; ii<1; ii++){
      fireNoise2();
      apply_mask(mask);
      my_show();
      delay(40);
    }
  }
  
  fillMask(mask, true);
  // bring fire out!
  for(int fill_to=7; fill_to>=0; fill_to--){
    for(int col=0; col < 16; col++){
      setPixelMask(mask, fill_to, col, false);
    }
    Faceplates[config.faceplate_idx % N_FACEPLATE].maskTime(tm, mask);
    for(int ii=0; ii<1; ii++){
      fireNoise2();
      apply_mask(mask);
      my_show();
      delay(40);
    }
  }
}
void Fire_display_time(uint32_t last_tm, uint32_t tm){
  if(last_tm/300 != tm/300){
    Fire_transition(last_tm, tm);
    Serial.print(last_tm);
    Serial.print(" ");
    Serial.println(tm);
  }
  EVERY_N_MILLISECONDS(40){// prevent moving too fast!
    fireNoise2();
    fillMask(mask, false);
    Faceplates[config.faceplate_idx % N_FACEPLATE].maskTime(tm, mask);  
    apply_mask(mask);
  }
}

void Glitch_init(){
}
uint8_t gHue = 0; // rotating "base color" used by many of the patterns
void rainbow(){
  // FastLED's built-in rainbow generator
  fill_rainbow( leds, NUM_LEDS, gHue++, 255 / NUM_LEDS);
}
void addGlitch( uint8_t chanceOfGlitch){
  if ( random8() < chanceOfGlitch) {
    leds[ random16(NUM_LEDS) ] += CRGB::White;
  }
}
void Glitch_display_time(uint32_t last_tm, uint32_t tm){
  EVERY_N_MILLISECONDS(40){// prevent moving too fast!
    rainbow();
    addGlitch(80);
    fillMask(mask, false);
    Faceplates[config.faceplate_idx % N_FACEPLATE].maskTime(tm, mask);  
    apply_mask(mask);
  }
}

// end Displays
//********************************************************************************

uint8_t logo_rgb[] = {
  0x11,0x00,0x29,0x00,0x25,0x00,0x23,0x00,0x25,0x00,0x29,0x00,0x31,0x00,0xe0,0x01,
  0x00,0x03,0x80,0x04,0x80,0x04,0x80,0x04,0x80,0x04,0x00,0x03,0x00,0x00,0x00,0x00,
  0x11,0x00,0x09,0x88,0x05,0x48,0x03,0x28,0x05,0x18,0x09,0x28,0x11,0x48,0x00,0x88
};

void ChangeDisplay(Display* display_p);
void ChangeDisplay(Display* display_p){
  CurrentDisplay_p = display_p;
  CurrentDisplay_p->init();
}

// Common Interface for buttons and MQTT
void set_brightness(uint8_t brightness){
  if(brightness < 256){
    config.brightness = brightness;
    FastLED.setBrightness(config.brightness);
    Serial.print("Adjust brightness to ");
    Serial.println(config.brightness);
    saveSettings();
  }
}

void adjust_brightness(int delta){
  int new_val = delta + config.brightness;
  if(delta != 0){
    if(0 < new_val && new_val < 256){
      set_brightness(new_val);
    }
  }
}

void dimmer(){
  byte b;
  b = config.brightness;
  if(b == 255){
    b = 128;
  }
  else if(b > 3){
    b /= 2;
  }
  else if (b == 3){
    b = 2;
  }
  else{
    b = 2;
  }
  set_brightness(b);
}
void brighter(){
  byte b;
  b = config.brightness;
  if(b >= 128){
    b = 255;
  }
  else if(b < 128){
    b *= 2;
  }
  else{
    b = 128;
  }
  set_brightness(b);
}

void set_faceplate(uint8_t faceplate_idx){
  config.faceplate_idx = faceplate_idx % N_FACEPLATE;
  Serial.print("Changing Faceplate to ");
  Serial.println(Faceplates[config.faceplate_idx].name);
  //ChangeFaceplate(&Faceplates[config.faceplate_idx % N_FACEPLATE]);// no action required
  saveSettings();
}
void set_display(uint8_t display_idx){
  config.display_idx = display_idx % N_DISPLAY;
  Serial.print("config.display_idx % N_DISPLAY:");
  Serial.println(config.display_idx % N_DISPLAY);
  ChangeDisplay(&Displays[config.display_idx % N_DISPLAY]);
  saveSettings();
}
void next_display(){
  config.display_idx = (config.display_idx + 1) % N_DISPLAY;
  ChangeDisplay(&Displays[config.display_idx % N_DISPLAY]);
  saveSettings();
  Serial.print("Display id: "); Serial.println(CurrentDisplay_p->name);
  Serial.print("Display #"); Serial.println(config.display_idx);
}

void add_to_timezone(int32_t offset){ 
  config.timezone += offset;
  config.use_ip_timezone = false; // time zone manually changed... ignore internate timezone
  saveSettings();
  if(config.use_wifi){
    ntp_clock.setOffset(config.timezone);
  }
}

void set_timezone_offset(int32_t offset){
  config.timezone = offset % 86400;
  saveSettings();
  if(config.use_wifi){
    ntp_clock.setOffset(config.timezone);
  }
}

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
void apply_mask(bool* mask, CRGB color){
  uint16_t b, k;
  for(uint16_t i=0; i < NUM_LEDS; i++){
    if(!mask[i]){
      leds[i] = color;
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
  EEPROM_readAnything(0, config);
}

void saveSettings(){
  EEPROM_writeAnything(0, config);
  EEPROM.commit();
}

uint16_t XY( uint8_t x, uint8_t y){
  uint16_t out = 0;
  if(config.flip_display){
    x = MatrixWidth - x - 1;
    y = MatrixHeight - y - 1;
  }
  if(x < MatrixWidth && y < MatrixHeight){
    out = MatrixMap[y][x];
  }
  return out;
}

uint8_t hex2dig(char h){
  uint8_t d = 0;
  if('0' <= h && h <= '9'){
    d = (uint8_t)(h - '0');
  }
  else if('a' <= h && h <= 'f'){
    d = (uint8_t)(10 + h - 'a');
  }
  else if('A' <= h && h <= 'F'){
    d = (uint8_t)(10 + h - 'A');
  }
  return d;
}

uint8_t hh2dd(char *hh){
  return hex2dig(hh[0]) * 16 + hex2dig(hh[1]);
}

bool force_timezone_from_ip = false;

void handle_msg(char* topic, byte* payload, unsigned int length) {
  bool handled = false;
  char str_payload[length + 1];
  char *subtopic = topic + 9;

  // copy bytes to normal char array
  for(int i = 0; i < length; i++){
    str_payload[i] = payload[i];
  }
  str_payload[length] = 0;
  
  Serial.print("msg\n  subtopic:");
  Serial.println(subtopic);
  Serial.print("  payload:");
  Serial.println(str_payload);
  
  if(strcmp(subtopic, "timezone_offset") == 0){
    Serial.println("Change timezone.");
    if(strcmp(str_payload, "IP") == 0){
      config.use_ip_timezone = true;
      force_timezone_from_ip = true;
      saveSettings();
      print_config();
    }
    else{
      String s = String(str_payload);
      set_timezone_offset(s.toInt());
      config.use_ip_timezone = false;
      saveSettings();
      print_config();
    }
  }
  else if(strcmp(subtopic, "add_to_timezone") == 0){
    Serial.println("Add to timezone!");
    add_to_timezone(String(str_payload).toInt());
  }
  else if(strcmp(subtopic, "display_idx") == 0){
    Serial.println("Change display_idx.");
    set_display(String(str_payload).toInt());
  }
  else if(strcmp(subtopic, "faceplate_idx") == 0){
    Serial.println("Change faceplate_idx.");
    set_faceplate(String(str_payload).toInt());
  }
  else if(strcmp(subtopic, "next_display") == 0){
    Serial.println("Increment display.");
    next_display();
  }
  else if(strcmp(subtopic, "brighter") == 0){
    Serial.println("Increment brigtness.");
    brighter();
  }
  else if(strcmp(subtopic, "dimmer") == 0){
    Serial.println("Decrement brigtness.");
    dimmer();
  }
  else if(strcmp(subtopic, "flip_display") == 0){
    if(config.flip_display){
      config.flip_display = false;
    }
    else{
      config.flip_display = true;
    }      
    Serial.print("Flip Display:");
    Serial.println(config.flip_display);
    force_update = true;
    saveSettings();
  }
  else if(strcmp(subtopic, "mqtt_ip") == 0){
    Serial.println("Update mqtt_ip address.");
    byte tmp_ip[4];
    if(ip_from_str(str_payload, tmp_ip)){
      for(int i=0; i<4; i++){
	config.mqtt_ip[i] = tmp_ip[i];
      }
      saveSettings();
      mqtt_setup();
    }
  }
  else if(strcmp(subtopic, "set_rgb") == 0 && length == 6){
    // payload: rrggbb lowercase html color code example "ff0000" is RED
    config.solid_color_rgb[0] = hh2dd((char*)payload);
    config.solid_color_rgb[1] = hh2dd((char*)payload + 2);
    config.solid_color_rgb[2] = hh2dd((char*)payload + 4);
    force_update = true;
    saveSettings();
  }
  else if(strcmp(subtopic, "set_time") == 0){
    // set DS3231, turn off internet time
    uint32_t tm = String(str_payload).toInt();
    ds3231_clock.set(tm);
    config.use_ip_timezone = false;
    config.use_ntp_time = false;
    config.use_wifi = false;
    saveSettings();
  }
  else if(strcmp(subtopic, "use_ntp") == 0){
    if(!config.use_ntp_time){
      Serial.println("Use NTP time service.");
      // turn ON internet time
      config.use_ip_timezone = true;
      config.use_wifi = true;
      config.use_ntp_time = true;

      wifi_setup();
      ntp_clock.setup(&timeClient);
      ntp_clock.setOffset(config.timezone);
      doomsday_clock.setup(&ntp_clock, &ds3231_clock);
      saveSettings();
    }
    else{
      Serial.println("Already using NTP time service.");
    }
  }
  else if(strcmp(subtopic, "notify") == 0){
    // payload: ascii notification
  }
  else if(strcmp(subtopic, "get_faceplates") == 0){
    Serial.println(get_faceplate_json());
  }
}

void handle_mqtt_msg(char* topic, byte* payload, unsigned int length){
  handle_msg(topic, payload, length);
}

bool ip_from_str(char* str, byte* ip){
  byte my_ip[4];
  int end_poss[4];
  int i = 0, j = 0;
  int dots_found = 0;
  bool out = false;
  byte num;
  String strstr = String(str);
  
  Serial.println("ip_from_str");
  Serial.println(str);
  while(i < strlen(str) && dots_found < 3){
    if(str[i] == '.'){
      end_poss[dots_found] = i;
      dots_found++;
    }
    i++;
  }
  if(dots_found == 3){
    out = true;
    end_poss[3] = strlen(str);
    for(i = 0; i < 4; i++){
      num = String(strstr.substring(j, end_poss[i])).toInt();
      if(num == 0){
	out = false;
      }
      ip[i] = num;
      j = end_poss[i] + 1;
    }
  }
  if(out){
    Serial.print("IP: ");
    for(i=0; i<4; i++){
      Serial.print(ip[i]);
      if(i < 3){
	Serial.print(".");
      }
    }
    Serial.println();
  }
  return out;
}

void mqtt_subscribe(){
  mqtt_client.subscribe("clockiot/#");
}

uint32_t next_mqtt_attempt = 0;

bool mqtt_connect(){
  String str;
  String unique_id = String("ClockIOT") + String(WiFi.macAddress());

  if(!mqtt_client.connected() && next_mqtt_attempt < millis()){
    if(mqtt_client.connect(unique_id.c_str())){
      Serial.println("mqtt connected");
      Serial.println(unique_id);
      // Once connected, publish an announcement...
      // ... and resubscribe
      mqtt_subscribe();
    }
  }
  uint32_t n = millis();
  
  next_mqtt_attempt = n + 5000;
  
  return mqtt_client.connected();
}

void mqtt_setup(){
  //uint8_t server[4] = {192, 168, 1, 159};
  //uint8_t server[4] = {10, 10, 10, 2};
  mqtt_client.setServer(config.mqtt_ip, 1883);
  mqtt_client.setCallback(handle_mqtt_msg);
  mqtt_connect();
  Serial.println("USE MQTT");
}

void led_setup(){
  FastLED.addLeds<LED_TYPE, DATA_PIN, CLK_PIN, COLOR_ORDER>(leds, NUM_LEDS);
  FastLED.setDither(true);
  FastLED.setCorrection(TypicalLEDStrip);
  FastLED.setMaxPowerInVoltsAndMilliamps(5, MILLI_AMPS);
  
  fill_solid(leds, NUM_LEDS, CRGB::Black);
  my_show();
}

void wifi_setup(){
  if(config.wifi_reset){
    config.wifi_reset = false;
    saveSettings();
    wifiManager.startConfigPortal("KLOK");
  }
  else{
    wifiManager.autoConnect("KLOK");
  }
  Serial.println("Yay connected!");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}


/*********************************************************************************/
// Web Socket Server stuff
WebSocketsServer webSocket = WebSocketsServer(81);

void hexdump(const void *mem, uint32_t len, uint8_t cols) {
	const uint8_t* src = (const uint8_t*) mem;
	Serial.printf("\n[HEXDUMP] Address: 0x%08X len: 0x%X (%d)", (ptrdiff_t)src, len, len);
	for(uint32_t i = 0; i < len; i++) {
		if(i % cols == 0) {
			Serial.printf("\n[0x%08X] 0x%08X: ", (ptrdiff_t)src, i);
		}
		Serial.printf("%02X ", *src);
		src++;
	}
	Serial.printf("\n");
}
String get_faceplate_json(){
  String faceplate_names = String("{\"faceplates\":[");
  for(int ii=0; ii < N_FACEPLATE; ii++){
    faceplate_names = faceplate_names + String("\"") + String(Faceplates[ii].name)  + String("\"");
    if(ii < N_FACEPLATE - 1){
      faceplate_names = faceplate_names + String(",");
    }
    //Serial.println(faceplate_names);
  }
  faceplate_names = faceplate_names + String("],\"faceplate_idx\":\"") + String(config.faceplate_idx) + String("\"}");

  return faceplate_names;
}
void webSocketEvent(uint8_t num, WStype_t type, uint8_t * ws_payload, size_t length) {
  char topic_payload[length + 1];
  String str_topic_payload;
  int i;
  int start, stop;
  
  switch(type) {
  case WStype_DISCONNECTED:
    Serial.printf("[%u] Disconnected!\n", num);
    break;
  case WStype_CONNECTED:
    {
      IPAddress ip = webSocket.remoteIP(num);
      Serial.printf("[%u] Connected from %d.%d.%d.%d url: %s\n", num, ip[0], ip[1], ip[2], ip[3], ws_payload);
      
      // send message to client
      webSocket.sendTXT(num, "Connected");
    }
    break;
  case WStype_TEXT:
    Serial.printf("[%u] get Text: %s\n", num, ws_payload);

    for(i=0; i < length; i++){
      topic_payload[i] = (char)ws_payload[i];
    }
    topic_payload[length] = 0;
    str_topic_payload = String(topic_payload);

    start = str_topic_payload.indexOf("//");
    stop = start + 2;
    if(start < 0){
      start = length;
      stop = length;
    }
    
    char topic[100];
    byte payload[100];
    for(i = 0; i < start; i++){
      topic[i] = topic_payload[i];
    }
    topic[start] = 0;

    for(i = 0; i < length - stop; i++){
      payload[i] = (byte)topic_payload[stop + i];
    }
    payload[length - stop] = 0;
    
    handle_msg(topic, payload, length - stop);

    if (strcmp(topic, "clockiot/get_displays") == 0) {
	// send display names to client
      String display_names = String("{\"displays\":[");
      for(int ii=0; ii < N_DISPLAY; ii++){
	display_names = display_names + String("\"") + String(Displays[ii].name)  + String("\"");
	if(ii < N_DISPLAY - 1){
	  display_names = display_names + String(",");
	}
      }
      display_names = display_names + String("],\"display_idx\":\"") + String(config.display_idx) + String("\"}");
      
      webSocket.sendTXT(num, display_names.c_str());
      Serial.println("Display names requested");
      }
    
    // send data to all connected clients
    // webSocket.broadcastTXT("message here");
    if (strcmp(topic, "clockiot/get_faceplates") == 0) {
	// send faceplate names to client
      String faceplate_names = get_faceplate_json();
      
      webSocket.sendTXT(num, faceplate_names.c_str());
      Serial.println("Faceplate names requested");
      }
    
    // send data to all connected clients
    // webSocket.broadcastTXT("message here");
    break;
  case WStype_BIN:
    Serial.printf("[%u] get binary length: %u\n", num, length);
    hexdump(ws_payload, length, 16);
    
    // send message to client
    // webSocket.sendBIN(num, ws_payload, length);
    break;
  case WStype_ERROR:			
  case WStype_FRAGMENT_TEXT_START:
  case WStype_FRAGMENT_BIN_START:
  case WStype_FRAGMENT:
  case WStype_FRAGMENT_FIN:
    break;
  }
}

void websocket_setup(){
  webSocket.begin();
  webSocket.onEvent(webSocketEvent);  
}

// Web Socket Server stuff
/*********************************************************************************/

/*********************************************************************************/
// Button stuff
byte read_buttons(bool *enter_p, bool *inc_p, bool *decr_p, bool *mode_p){
  byte state;
  *enter_p = digitalRead(ENTER);
  *inc_p = digitalRead(INC);
  *decr_p = digitalRead(DECR);
  *mode_p = digitalRead(MODE);
  
  state |= (*enter_p) << 3;
  state |= (*inc_p)  << 2;
  state |= (*decr_p) << 1;
  state |= (*mode_p) << 0;

  return state;
}

void shuffle(int *array, size_t n){
    if (n > 1) 
    {
        size_t i;
        for (i = 0; i < n - 1; i++) 
        {
          size_t j = i + rand() / (RAND_MAX / (n - i) + 1);
          int t = array[j];
          array[j] = array[i];
          array[i] = t;
        }
    }
}

void test_leds(){
  int i;

  int order[NUM_LEDS];
  for(i=0; i< NUM_LEDS; i++){
    order[i] = i;
  }
  shuffle(order, NUM_LEDS);
  
  for(i=0; i < NUM_LEDS; i++){
    leds[order[i]] = CRGB::Red;
  }
  FastLED.show();
  delay(200);
  shuffle(order, NUM_LEDS);
  for(i=0; i < NUM_LEDS; i++){
    leds[order[i]] = CRGB::Green;
    FastLED.show();
    delay(10);
  }
  delay(200);
  shuffle(order, NUM_LEDS);
  for(i=0; i < NUM_LEDS; i++){
    leds[order[i]] = CRGB::Blue;
    FastLED.show();
    delay(10);
  }
  delay(200);
  shuffle(order, NUM_LEDS);
  for(i=0; i < NUM_LEDS; i++){
    leds[order[i]] = CRGB::White;
    FastLED.show();
    delay(10);
  }
  delay(1000);
  fill_black();
  FastLED.show();
}

void bigX(){
  for(int i=0;i < 8; i++){
    setPixel(i, 2 * i - 1, CRGB::Red);
    setPixel(i, 2 * i, CRGB::Red);
    setPixel(i, 2 * i + 1, CRGB::Red);
    setPixel(7 - i, 2 * i, CRGB::Red);
  }
  my_show();
  Serial.println("Big X");
  while(1) delay(100);
}
void test_ds3231(){
  int first = ds3231_clock.now();
  delay(1500);
  int second = ds3231_clock.now();
  if(second <= first){
    Serial.println("RTC Failed");
    bigX();
  }
  else{
    Serial.println("RTC Passed");
  }
}

void factory_reset(){
  Serial.println("Factory RESET");
  config.timezone = 255; //?
  config.brightness = 8; 
  config.brightness = 255;  // todo: delete me!
  config.display_idx = 255;
  config.use_wifi = 255;
  config.use_ip_timezone = 255;
  config.mqtt_ip[0] = 255;
  config.mqtt_ip[1] = 255;
  config.mqtt_ip[2] = 255;
  config.mqtt_ip[3] = 255;
  config.solid_color_rgb[0] = 0;
  config.solid_color_rgb[1] = 0;
  config.solid_color_rgb[2] = 255;
  config.flip_display = 255;
  config.last_tz_lookup = 0;
  config.use_ntp_time = true;
  config.wifi_reset = true;
  config.faceplate_idx = DEFAULT_FACEPLATE_IDX;
  config.factory_reset = false;
  saveSettings();
  print_config();
  test_leds();
  test_ds3231();
  delay(1000);

  Serial.println("Factory reset complete");
  ESP.restart();
}

void button_set_time(){
  Serial.println("Set Time w/ INC/DEC buttons");
  
  uint32_t current_time = ds3231_clock.now();
  current_time -= current_time % 60;
  bool enter, inc, decr, mode;
  byte button_state;
  byte last_button_state = 0b000;
  while(1){

    button_state = read_buttons(&enter, &inc, &decr, &mode);

    if(button_state != last_button_state){
      if(button_state == 0b0100){
	Serial.println("inc");
	current_time += 60;
      }
      else if(button_state == 0b0010){
	Serial.println("decr");
	current_time -= 60;
      }
      else if(button_state == 0b1000){
	Serial.println("enter");
	current_time += 60 * 60;
      }
      else if(button_state == 0b0001){
	Serial.println("mode");
	current_time -= 60 * 60;
      }
      if(button_state){
	ds3231_clock.set(current_time);
      }
    }
    last_button_state = button_state;
    if((millis() % 1000) < 900){
      fill_blue();
    }
    else{
      fill_green();
    }
    fillMask(mask, OFF);
    Faceplates[config.faceplate_idx % N_FACEPLATE].maskTime(current_time, mask);
    apply_mask(mask);
    for(int min = 0; min < (current_time % 300)/60; min++){
      setPixel(min, 15, CRGB::Green);
    }
    my_show();
  }
}

void button_setup(){
  bool enter, inc, decr, mode;
  pinMode(ENTER, INPUT);
  pinMode(INC, INPUT);
  pinMode(DECR, INPUT);
  pinMode(MODE, INPUT);

  byte button_state = read_buttons(&enter, &inc, &decr, &mode);

  Serial.print("Button state: ");
  Serial.println(button_state);

  if(button_state == (1 << 3 | 1)){ // MODE + ENTER does a factory reset
    factory_reset();
  }
  else if(mode){ // mode configures stand alone
    if(config.use_ntp_time){ // toggle using internet time
      config.use_ntp_time = false;
      config.use_wifi = false;
      config.use_ip_timezone = false;
      saveSettings();
      button_set_time();
    }
    else{
      config.use_ntp_time = true;
      config.use_wifi = true;
      config.use_ip_timezone = true;
      saveSettings();
    }
  }
  else if(enter){
    set_brightness(4);
    Serial.println("Low power mode");
  }
}

uint32_t last_pressed = 0;
byte last_button_state = 0b0000;
void button_loop(){
  bool enter, inc, decr, mode;
  byte button_state = read_buttons(&enter, &inc, &decr, &mode);
  
  if(button_state != last_button_state){
    if(mode){
      next_display();
    }
    if(inc){
      brighter();
    }
    if(decr){
      dimmer();
    }
  }
  last_button_state = button_state;
}
// Button stuff
/*********************************************************************************/

bool use_mqtt(){
  bool out = false;
  for(int i=0; i<4; i++){
    if(config.mqtt_ip[i] != 255){
      out = true;
      break;
    }
  }
  return out;
}

void setup(){
  last_time = 0;
  
  CurrentDisplay_p = &PlainDisplay;
  //CurrentDisplay_p = &TheMatrixDisplay;
  //CurrentDisplay_p = &WordDropDisplay;
  Wire.begin();
  Serial.begin(115200);
  
  delay(200);
  Serial.println("setup() starting");

  EEPROM.begin(1024);
  //for(int i = 0; i < 1024; i++){
  //  EEPROM.write(i, 255);
  //}
  //EEPROM.commit();
  loadSettings();
  print_config();
  
  led_setup(); // set up leds first so buttons can affect display if needed
  if(config.factory_reset){// do factory reset on first on
    factory_reset();
  }
  if(config.display_idx == 255){
    config.display_idx = 0;
    saveSettings();
  }
  
  //config.use_wifi = false; // Debug
  
  CurrentDisplay_p = &Displays[config.display_idx % N_DISPLAY];
  Serial.println("{\"faceplates\":");
  for(int ii = 0; ii < N_FACEPLATE; ii++){
    Serial.print("    ");
    Serial.print(ii);
    Serial.print(" : \"");
    Serial.print(Faceplates[ii].name);
    Serial.print("\"");
    if(ii < N_FACEPLATE - 1){
      Serial.print(",");
    }
    Serial.println();
    Faceplates[ii].setup(MatrixWidth, MatrixHeight, XY);
  }
  Serial.println("}");
  if (config.faceplate_idx >= N_FACEPLATE){
    set_faceplate(DEFAULT_FACEPLATE_IDX);
  }
  Serial.println("faceplates setup()!");

  // logo
  if( config.brightness == 0 || config.brightness == 255){
    config.brightness == 8;
  }
  FastLED.setBrightness(config.brightness);
  ds3231_clock.setup();
  button_setup();

  test_leds(); // resume normal start up
  wipe_around(ON);
  display_bitmap_rgb(logo_rgb);
  my_show();  
  wipe_around(OFF);
  display_bitmap_rgb(logo_rgb);
  my_show();
  if(config.use_wifi){
    wifi_setup();
  }
  if(use_mqtt()){
    mqtt_setup();
  }
  
  wipe_around(ON);
  fillMask(mask, false);
  wipe_around(OFF);

  //CurrentDisplay_p->init();
  //while(1)delay(100);
  if(config.use_wifi){
    if(config.use_ntp_time){
      ntp_clock.setup(&timeClient);
      ntp_clock.setOffset(config.timezone);
      ds3231_clock.set(ntp_clock.now());
      doomsday_clock.setup(&ntp_clock, &ds3231_clock);
    }
    set_timezone_from_ip();
    websocket_setup();
  }
  Serial.print("config.timezone: ");
  Serial.println(config.timezone);
  Serial.print("config.use_ip_timezone: ");
  Serial.println((bool)config.use_ip_timezone);
  set_display(config.display_idx % N_DISPLAY);
  Serial.println("setup() complete");
  setup_complete = true;
}

uint32_t Now(){
  uint32_t out;

  if(config.use_wifi){
    if(config.use_ntp_time){
      if(setup_complete){
	out = doomsday_clock.now();
      }
      else{
	out = 0;
      }
      if(weekday(out) == 1){ // refresh utc offset sunday between 3:02 and 3:59 AM
	if(hour(out) == 3){
	  if(minute(out) > 1){ 
	    if(doomsday_clock.gmt() - config.last_tz_lookup > 86400 -2 * 60){// at most one update per day
	      Serial.print(" Check DST!! ");
	      Serial.print(hour(out));Serial.print(":");
	      Serial.print(minute(out));Serial.print("\n");
	      set_timezone_from_ip();
	    }
	  }
	}
      }
    }
    else{
      out = ds3231_clock.now();
    }
  }
  else{
    out = ds3231_clock.now();
  }
  return out;
}

void tobytes(const char *frm, byte *_to, int len){
  for(int ii = 0; ii < len; ii++){
    _to[ii] = frm[ii];
  }
}

void tochars(const char *frm, char *_to, int len){
  for(int ii = 0; ii < len; ii++){
    _to[ii] = frm[ii];
  }
  _to[len] = 0;
}

#define SERMAXLEN 100
char ser_msg[SERMAXLEN + 1];
uint8_t ser_msg_len = 0;
void serial_loop(){/// allow same msgs as mqtt
  String ser_str, topic, payload;
  int start, stop;
  char topic_c_str[101];
  byte payload_bytes[100];
  
  //  msg format: topic//payload.  Example: "clockiot/timezone_offset//-14400"
  
  while(Serial.available() && ser_msg_len < SERMAXLEN){
    ser_msg[ser_msg_len++] = Serial.read();
  }
  if(ser_msg_len > 0){
    ser_str = String(ser_msg);
    start = ser_str.indexOf("clockiot");
    if(start >= 0){
      stop = ser_str.indexOf("//", start);
      if(stop < 0){
	stop = ser_str.length();
      }
      else{
      }
      topic = ser_str.substring(start, stop);
      if(stop == ser_str.length()){
      }
      else{
	stop += 2; // skip slashes "//"
      }
      payload = ser_str.substring(stop, ser_str.length());
      tochars(topic.c_str(), topic_c_str, topic.length());
      tobytes(payload.c_str(), payload_bytes, payload.length());
      handle_msg(topic_c_str, payload_bytes, payload.length());      
    }
  }
  // clear msg
  for(int ii=0; ii < ser_msg_len + 1; ii++){
    ser_msg[ii] = 0;
  }
  ser_msg_len = 0;
}

void  interact_loop(){
  if(force_timezone_from_ip){
    force_timezone_from_ip = false;
    set_timezone_from_ip();
  }
  if (use_mqtt()){
    mqtt_client.loop();
  }
  if(config.use_wifi){
    webSocket.loop();
  }
  button_loop();
  serial_loop();
}

void loop(){
  uint8_t word[3];
  uint32_t current_time = Now();

  interact_loop();
  
  CurrentDisplay_p->display_time(last_time, current_time);
  my_show();

  /*
  Serial.print("NTP Time:");
  Serial.print(timeClient.getHours());
  Serial.print(":");
  Serial.print(timeClient.getMinutes());
  Serial.print(":");
  Serial.print(timeClient.getSeconds());
  Serial.println("");
  
  Serial.print("ds3231_clock.Time:");
  Serial.print(ds3231_clock.year());
  Serial.print("/");
  Serial.print(ds3231_clock.month());
  Serial.print("/");
  Serial.print(ds3231_clock.day());
  Serial.print(" ");
  Serial.print(ds3231_clock.hours());
  Serial.print(":");
  Serial.print(ds3231_clock.minutes());
  Serial.print(":");
  Serial.println(ds3231_clock.seconds());
  Serial.println(ds3231_clock.now());
  Serial.println(ds3231_clock.rtc.now().unixtime());
  Serial.println();
  delay(1000);
  */
  if(config.use_wifi){
    if(config.use_ntp_time){
      if(doomsday_clock.seconds() == 0 and millis() < 1){
	Serial.print("Doomsday Time:");
	Serial.print(doomsday_clock.year());
	Serial.print("/");
	Serial.print(doomsday_clock.month());
	Serial.print("/");
	Serial.print(doomsday_clock.day());
	Serial.print(" ");
	if(doomsday_clock.hours() < 10)Serial.print('0');
	Serial.print(doomsday_clock.hours());
	Serial.print(":");
	if(doomsday_clock.minutes() < 10)Serial.print('0');
	Serial.print(doomsday_clock.minutes());
	Serial.print(":");
	if(doomsday_clock.seconds() < 10)Serial.print('0');
	Serial.println(doomsday_clock.seconds());
      }
    }
  }
  last_time = current_time;
}
#include "Noise.h"
