#include <Wire.h>
#include <FastLED.h>
#include <RTClib.h>
#include <EEPROM.h>
#include <EEPROMAnything.h>

RTC_DS3231 rtc;
#define NUM_LEDS 8*16
#define DATA_PIN     4
#define CLK_PIN      16
//#define DATA_PIN     MOSI
//#define CLK_PIN      SCK
#define COLOR_ORDER BGR
#define LED_TYPE APA102
#define MILLI_AMPS 1000  // IMPORTANT: set the max milli-Amps of your power supply (4A = 4000mA)
#define BRIGHTNESS 20

// Buttons
#define SW_ENTER 17
#define SW_INC    5
#define SW_DEC   18
#define SW_MODE  19

CRGBArray<NUM_LEDS> leds; 

// Button stuff
#define ENTER 17
#define INC 5
#define DECR 18
#define MODE 19

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
void setup() {
  Serial.begin(115200);
  FastLED.addLeds<LED_TYPE,DATA_PIN,CLK_PIN, COLOR_ORDER>(leds,NUM_LEDS);
  // set master brightness control
  FastLED.setBrightness(BRIGHTNESS);
  
  if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
    //while (1);
  } else{
    if (rtc.lostPower()) {
      Serial.println("RTC lost power, lets set the time!");
      // following line sets the RTC to the date & time this sketch was compiled
      rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
      // This line sets the RTC with an explicit date & time, for example to set
      // January 21, 2014 at 3am you would call:
      // rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));
    }
    DateTime now = rtc.now();
    
    Serial.print(now.year(), DEC);
    Serial.print('/');
    Serial.print(now.month(), DEC);
    Serial.print('/');
    Serial.print(now.day(), DEC);
    Serial.print(" ");
    Serial.print(now.hour(), DEC);
    Serial.print(':');
    Serial.print(now.minute(), DEC);
    Serial.print(':');
    Serial.print(now.second(), DEC);
    Serial.println();

  }

  EEPROM.begin(1024);
  loadSettings();
  print_config();

}

void loop() {
  Serial.println("Testing LEDS");
  incremental_fill_color(CRGB(255,0,0));
  delay(1000);
  incremental_fill_color(CRGB(0,255,0));
  delay(1000);
  incremental_fill_color(CRGB(0,0,255));
  delay(1000);
}

void loadSettings(){
  EEPROM_readAnything(0, config);
}
void saveSettings(){
  EEPROM_writeAnything(0, config);
  EEPROM.commit();
}

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

void incremental_fill_color(CRGB color){
  bool enter, inc, decr, mode;
  byte button_state;
  byte last_button_state = 0b000;
  int val;
  
  for(int i=0;i < NUM_LEDS;i++){
    leds[i] = color;
    FastLED.show();
    delay(10);

    button_state = read_buttons(&enter, &inc, &decr, &mode);
    val = enter + 2 * inc + 4 * decr + 8 * mode;
    if(val > 0){
      Serial.println(val);
    }
  }
  for (int i = NUM_LEDS-1; i >=0; i--){
    leds[i] = CRGB::Black;
    FastLED.show();
    delay(10);
    button_state = read_buttons(&enter, &inc, &decr, &mode);
    val = enter + 2 * inc + 4 * decr + 8 * mode;
    if(val > 0){
      Serial.println(val);
    }
  }
}

