#include "timeController.hpp"

timeController::timeController(void){
  lastUpdateMillis = millis();
  TIME.tm_hour = 0;
  TIME.tm_min = 0;
  TIME.tm_sec = 0;
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

class timeClass timeController::getLampInterval(void){
 return LAMP_INTERVAL;
}

void timeController::setWakeTime(timeClass newTime){
  WAKE_TIME = newTime; 
}

void timeController::setLampInterval(timeClass newTime){
  LAMP_INTERVAL = newTime;
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

timeClass parseTime(String data) {
  int firstDot = data.indexOf('.');
  int secondDot = data.indexOf('.', firstDot + 1);
  int thirdDot = data.indexOf('.', secondDot + 1);

  int hour = data.substring(firstDot + 1, secondDot).toInt();
  int minute = data.substring(secondDot + 1, thirdDot).toInt();
  int second = data.substring(thirdDot + 1).toInt();

  return timeClass(hour, minute, second);
}