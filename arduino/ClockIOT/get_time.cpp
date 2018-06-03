#include "TimeLib.h"
#include "get_time.h"

int Clock::years(){
  return ::year(now());
}
int Clock::month(){
  return ::month(now());
}
int Clock::day(){
  return (now() % 86400) / 3600;
}
int Clock::hours(){
  return (now() % 86400) / 3600;
}
int Clock::minutes(){
  return (now() % 3600) / 60;
}
int Clock::seconds(){
  return (now() % 60);
}

uint32_t current_time = 0;
Clock::Clock(){
}
DummyClock::DummyClock(){
}
uint32_t DummyClock::now(){
  return current_time++;
}

NTPClock::NTPClock(){
}
void NTPClock::setup(NTPClient *_timeClient){
  this->timeClient = _timeClient;
  this->timeClient->setTimeOffset(-240 * 60);
  this->timeClient->begin();
}
uint32_t NTPClock::now(){
  this->timeClient->update();
  return this->timeClient->getEpochTime();
}

DS3231Clock::DS3231Clock(){
}
void DS3231Clock::setup(){
}
uint32_t DS3231Clock::now(){
  return rtc.now().unixtime();
}

