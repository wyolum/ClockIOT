#include <Wire.h>
#include <FastLED.h>
#include <RTClib.h>

RTC_DS3231 rtc;
#define NUM_LEDS 8*16
#define DATA_PIN     4
#define CLK_PIN      16
#define COLOR_ORDER BGR
#define LED_TYPE APA102
#define MILLI_AMPS 1000  // IMPORTANT: set the max milli-Amps of your power supply (4A = 4000mA)
#define BRIGHTNESS 50

// Buttons
#define SW_ENTER 17
#define SW_INC    5
#define SW_DEC   18
#define SW_MODE  19

CRGBArray<NUM_LEDS> leds;

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
void incremental_fill_color(CRGB color){
  for(int i=0;i < NUM_LEDS;i++){
      leds[i] = color;
       FastLED.show();
       delay(10);
  }
  for (int i = NUM_LEDS-1; i >=0; i--){
    leds[i] = CRGB::Black;
    FastLED.show();
    delay(10);
  }
}

