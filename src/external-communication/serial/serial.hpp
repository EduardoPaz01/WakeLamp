#include <Arduino.h>
#include "serial-codes.hpp"

/** 
  * @brief Class for serial communication 
  */
class serial {

private:
  /**
  * @brief Serial communication frequency
  */
  int FREQUENCY;

  /**
   * @brief Cache line for incoming serial data
   */
  String CACHE_LINE;

  /**
  * @brief Buffer for outgoing serial data
  */
  String BUFFER;

public:

  /**
   * @brief Constructor for the serial class
   *
   * @param FREQUENCY_SERIAL Serial communication frequency
   */
  serial(int FREQUENCY_SERIAL = 115200);

  /**
   * @brief Destructor for the serial class
   */
  ~serial();

  /**
   * @brief Process incoming serial data
   */
  uint32_t processSerial();

  /**
   * @brief Print a message to the serial port
   *
   * @param MESSAGE The message to print
   */
  void print(String MESSAGE);

  /**
   * @brief Get the buffer for outgoing serial data
   *
   * @return The buffer as a String
   */
  String getBuffer();

};