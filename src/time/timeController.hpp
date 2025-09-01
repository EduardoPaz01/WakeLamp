#include <Arduino.h>
#include <time.h>

/**
 * @brief Class for time controller
 */
class timeController {

private:
  /**
   * @brief Current day
   */
  uint32_t DAY    = 0;

  /**
   * @brief Current hour
   */
  uint32_t HOUR   = 0;

  /**
   * @brief Current minute
   */
  uint32_t MINUTE = 0;

  /**
   * @brief Current second
   */
  uint32_t SECOND = 0;

public:
  /**
   * @brief Construct a new time object
   */
  timeController(void);

  /**
   * @brief Delay the program for a specified number of milliseconds
   * @param MILLISECONDS The number of milliseconds to delay
   */
  void delay_ms(int MILLISECONDS);
  
};