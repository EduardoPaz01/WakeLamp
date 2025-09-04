#include "timeController.hpp"

timeController::timeController(void){
}

void timeController::updateTime(struct tm current_time) {
  TIME = current_time;
  TIME.tm_hour -= 3; // (-3h) America/Sao_Paulo
  if(TIME.tm_hour < 0) TIME.tm_hour += 24;
  lastUpdateMillis = millis();
}

struct tm timeController::getTime(void){
  return TIME;
}

class timeClass timeController::getWakeTime(void){
  return WAKE_TIME;
}

class  timeClass timeController::getLampInterval(void){
 return LAMP_INTERVAL;
}

void timeController::tick(void) {
  unsigned long elapsedMillis = millis() - lastUpdateMillis;
  unsigned long elapsedSeconds = elapsedMillis / 1000;

  if (elapsedSeconds > 0) {
    TIME.tm_sec += elapsedSeconds;
    lastUpdateMillis += elapsedSeconds * 1000;

    if (TIME.tm_sec >= 60) {
      TIME.tm_min += TIME.tm_sec / 60;
      TIME.tm_sec %= 60;
    }
    if (TIME.tm_min >= 60) {
      TIME.tm_hour += TIME.tm_min / 60;
      TIME.tm_min %= 60;
    }
    if (TIME.tm_hour >= 24) {
      TIME.tm_hour %= 24;
    }
  }
}

