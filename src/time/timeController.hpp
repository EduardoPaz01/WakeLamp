#ifndef TIMECONTROLLER_HPP
#define TIMECONTROLLER_HPP

#include <Arduino.h>
#include <time.h>
#include "timeClass.hpp"

/**
 * @brief Class for time controller
 */
class timeController {

private:
  struct tm TIME;
  unsigned long lastUpdateMillis = 0;

  timeClass WAKE_TIME = timeClass(5, 30, 0); 
  timeClass LAMP_INTERVAL = timeClass(1, 0, 0);

public:
  /**
   * @brief Construct a new time object
   */
  timeController(void);

  /**
   *
   */
  void updateTime(struct tm current_time);

  /**
   *
   */
  struct tm getTime(void); 

  /**
   *
   */
  class timeClass getWakeTime(void);

  /**
   *
   */
  class timeClass getLampInterval(void);

  /**
   * Increment the time locally based on elapsed milliseconds
   */
  void tick(void);
};

#endif // TIMECONTROLLER_HPP