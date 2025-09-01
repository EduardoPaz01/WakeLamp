#include <Arduino.h>

/**
 * @brief Class representing a Lamp controller
 */
class lamp {

private:
  /**
    * @brief Pin number for lamp 1
    */
  int LAMP1 = 0;

  /**
    * @brief Pin number for lamp 2
    */
  int LAMP2 = 0;

  /**
    * @brief Pin number for button
    */
  int BUTTON = 0;

public:

  /**
    * @brief Constructor of the lamp class
    *
    * @param LAMP1_PIN Pin number for lamp 1
    * @param LAMP2_PIN Pin number for lamp 2
    * @param BUTTON_PIN Pin number for button
    */
  lamp(uint8_t LAMP1_PIN = 12, uint8_t LAMP2_PIN = 13, uint8_t BUTTON_PIN = 14);

  /**
    * @brief Turn on lamp 1
    */
  void lamp1On();

  /**
    * @brief Turn off lamp 1
    */
  void lamp1Off();

  /**
    * @brief Turn on lamp 2
    */
  void lamp2On();

  /**
    * @brief Turn off lamp 2
    */
  void lamp2Off();
};