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
