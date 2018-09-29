#include <Wire.h>
#include <FastLED.h>

#define LED_TYPE   APA102
#define MILLI_AMPS   1000  // IMPORTANT: set the max milli-Amps of your power supply (4A = 4000mA)
#define BRIGHTNESS     20
#define NUM_LEDS      128
#define DATA_PIN        4
#define CLK_PIN        16
#define COLOR_ORDER   BGR


CRGBArray<NUM_LEDS> leds; 

/*
#define ENTER 19
#define INC 18
#define DECR 5
#define MODE 17
*/
#define ENTER 17
#define INC 5
#define DECR 18
#define MODE 19

void incremental_fill_color(CRGB color){
  for(int i=0;i < NUM_LEDS;i++){
    leds[i] = color;
    FastLED.show();
  }
  for (int i = NUM_LEDS-1; i >=0; i--){
    leds[i] = CRGB::Black;
    FastLED.show();
  }
}

void setup() {
  
  Serial.begin(115200);
  delay(200);
  Serial.println("HWtest.ino");
  Wire.begin();

  // clear /EOSC bit
  // Sometimes necessary to ensure that the clock
  // keeps running on just battery power. Once set,
  // it shouldn't need to be reset but it's a good
  // idea to make sure.
  Wire.beginTransmission(0xD0); // address DS3231
  Wire.write(0x0E);             // select register
  Wire.write(0b00011100);       // write register bitmap, bit 7 is /EOSC
  Wire.endTransmission();

  FastLED.addLeds<LED_TYPE,DATA_PIN,CLK_PIN, COLOR_ORDER>(leds,NUM_LEDS);
  FastLED.setBrightness(BRIGHTNESS);
  
}

byte bcd(byte in){
  byte out;
  //out = in & 0b1111;
  out = ((in >> 4) & 0b1111) * 10 + (in & 0b1111);
  return out;
}
void loop() {
  // send request to receive data starting at register 0

  // test RCT with LEDs on
  Wire.beginTransmission(0x68); // 0x68 is DS3231 device address
  Wire.write((byte)0);          // start at register 0
  Wire.endTransmission();
  Wire.requestFrom(0x68, 3); // request three bytes (seconds, minutes, hours)
  
  while(Wire.available()){
    int seconds = bcd(Wire.read()); // get seconds
    int minutes = bcd(Wire.read()); // get minutes
    int hours = bcd(Wire.read());   // get hours
    Serial.print(hours);
    Serial.print(":");
    Serial.print(minutes);
    Serial.print(":");
    Serial.println(seconds);
  }


  if (Serial.available()){
    Serial.print("received: ");
    Serial.println(Serial.read());
  }

  if (digitalRead(ENTER)){
    Serial.println("ENTER");
    delay(100);
  }
  if (digitalRead(INC)){
    Serial.println("INC");
    delay(100);
  }
  if (digitalRead(DECR)){
    Serial.println("DECR");
    delay(100);
  }
  if (digitalRead(MODE)){
    Serial.println("MODE");
    delay(100);
  }

  //Serial.println("Testing LEDS");
  incremental_fill_color(CRGB(random(0, 255),random(0, 255), random(0, 255)));
  
}
