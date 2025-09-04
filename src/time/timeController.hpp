#ifndef TIMECONTROLLER_HPP
#define TIMECONTROLLER_HPP

#include <Arduino.h>
#include <time.h>

/**
 * @brief Class for time controller
 */
class timeController {

private:
  struct tm TIME;
  unsigned long lastUpdateMillis = 0;

public:
  /**
   * @brief Construct a new time object
   */
  timeController(void);

  /**
   * @brief Delay the program for a specified number of milliseconds
   * @param MILLISECONDS The number of milliseconds to delay
   */
};

#endif // TIMECONTROLLER_HPP