#include "TimeLib.h"
#include "get_time.h"

bool Clock::isCurrent(){
  return true;
}
bool Clock::set(uint32_t t){
  return false;
}
int Clock::year(){
  return ::year(now());
}
int Clock::month(){
  return ::month(now());
}
int Clock::day(){
  return ::day(now());
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

bool NTPClock::update(){
  return this->timeClient->update();
}
bool NTPClock::isCurrent(){
  return millis() - this->timeClient->_lastUpdate < this->timeClient->_updateInterval;
}

uint32_t NTPClock::now(){
  this->update();
  return this->timeClient->getEpochTime();
}

DS3231Clock::DS3231Clock(){
}
void DS3231Clock::setup(){
}
uint32_t DS3231Clock::now(){
  return rtc.now().unixtime();
}

bool DS3231Clock::set(uint32_t t){
  DateTime dt(t);
  rtc.adjust(dt);
  return true;
}

DoomsdayClock::DoomsdayClock(){
}
void DoomsdayClock::setup(Clock *_master, Clock *_backup){
  this->master = _master;
  this->backup = _backup;
}

uint32_t abs_diff(uint32_t left, uint32_t right){
  uint32_t out;
  
  if (left > right){
    out = left - right;
  }
  else{
    out = right - left;
  }
  return out;
}

uint32_t DoomsdayClock::now(){
  uint32_t out;
  uint32_t m, b;
  const uint32_t tol_sec = 5;
  
  m = master->now();
  b = backup->now();
  
  if(master->isCurrent()){
    out = master->now();
    if(abs_diff(m, b) > tol_sec){
      backup->set(master->now());
    }
  }
  else{
    out = backup->now();
  }
  return out;
}
