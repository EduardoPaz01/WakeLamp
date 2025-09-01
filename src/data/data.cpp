#include "data.hpp"

data::data(void) {
  if (!EEPROM.begin(EEPROM_SIZE)) {
    Serial.println("Falha ao iniciar EEPROM!");
  }
}

data::~data() {
  EEPROM.end();
}