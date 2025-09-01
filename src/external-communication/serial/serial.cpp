#include "serial.hpp"

serial::serial(int FREQUENCY_SERIAL) {
  FREQUENCY = FREQUENCY_SERIAL;
  
  Serial.begin(FREQUENCY);
}