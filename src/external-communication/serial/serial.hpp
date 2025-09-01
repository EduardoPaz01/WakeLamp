#include <Arduino.h>

/** 
  * @brief Class for serial communication 
  */
class serial {

private:
  /**
  * @brief Serial communication frequency
  */
  int FREQUENCY;

public:
  /**
   * @brief Constructor for the serial class
   *
   * @param FREQUENCY_SERIAL Serial communication frequency
   */
  serial(int FREQUENCY_SERIAL = 115200);

};