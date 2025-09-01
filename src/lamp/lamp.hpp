/**
 * Class representing a Lamp controller
 */
class lamp {

private:
  /**
    * Pin number for lamp 1
    */
  int LAMP1 = 0;

  /**
    * Pin number for lamp 2
    */
  int LAMP2 = 0;

  /**
    * Pin number for button
    */
  int BUTTON = 0;

public:

  /**
    * Constructor of the lamp class
    */
  lamp(uint8_t LAMP1_PIN = 12, uint8_t LAMP2_PIN = 13, uint8_t BUTTON_PIN = 14);

  /**
    * Turn on lamp 1
    */
  void lamp1On();

  /**
    * Turn off lamp 1
    */
  void lamp1Off();

  /**
    * Turn on lamp 2
    */
  void lamp2On();

  /**
    * Turn off lamp 2
    */
  void lamp2Off();
};