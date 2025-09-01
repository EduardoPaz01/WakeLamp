#include "timeController.hpp"

timeController::timeController(void){
  DAY = 0;
  HOUR = 0;
  MINUTE = 0;
  SECOND = 0;
}

void timeController::delay_ms(int MILLISECONDS) {
  delay(MILLISECONDS);
}