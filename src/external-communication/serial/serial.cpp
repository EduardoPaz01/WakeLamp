#include "serial.hpp"

serial::serial(int FREQUENCY_SERIAL) {
  FREQUENCY = FREQUENCY_SERIAL;
  Serial.begin(FREQUENCY);
}

serial::~serial() {
  Serial.end();
}

uint32_t serial::processSerial() {
  static String buf = "";
  uint32_t return_value = UNKNOWN_CMD; 

  while (Serial.available()) {
    char c = (char)Serial.read();
    if (c == '\r') continue;  
    if (c == '\n') {
      CACHE_LINE = buf;
      buf = "";

      CACHE_LINE.trim();
      if (CACHE_LINE.length() == 0) continue;

      if (CACHE_LINE.startsWith("$")) {
        CACHE_LINE = CACHE_LINE.substring(1); // remove $
        if (CACHE_LINE.startsWith("GET.TIME")) {
          return_value = GET_TIME;
        } 
        else if (CACHE_LINE.startsWith("UPDATE.TIME")) {
          return_value = UPDATE_TIME;
        } 
        else if (CACHE_LINE.startsWith("SET.WAKE.TIME.")) {
          CACHE_LINE = CACHE_LINE.substring(14); // remove SET.WAKE.TIME.
          BUFFER = CACHE_LINE;
          return_value = SET_WAKE_TIME;
        } 
        else if (CACHE_LINE.startsWith("GET.WAKE.TIME")) {
          return_value = GET_WAKE_TIME;
        } 
        else if (CACHE_LINE.startsWith("SET.LAMP.INTERVAL.")) {
          CACHE_LINE = CACHE_LINE.substring(18); // remove SET.LAMP.INTERVAL.
          BUFFER = CACHE_LINE;
          return_value = SET_LAMP_INTERVAL;
        } 
        else if (CACHE_LINE.startsWith("GET.LAMP.INTERVAL")) {
          return_value = GET_LAMP_INTERVAL;
        } 
        else if (CACHE_LINE.startsWith("GET.SSIDS")) {
          return_value = GET_SSIDS;
        } 
        else if (CACHE_LINE.startsWith("GET.USERS.SSID")) {
          return_value = GET_USERS_SSID;
        } 
        else if (CACHE_LINE.startsWith("TRY.CONNECT.")) {
          CACHE_LINE = CACHE_LINE.substring(12); // remove TRY.CONNECT.
          BUFFER = CACHE_LINE;
          return_value = TRY_CONNECT;
        } else {
          return_value = UNKNOWN_CMD;
        }
      } else {
        return_value = UNKNOWN_CMD;
      }

      CACHE_LINE = "";
      return return_value; 
    } else {
      buf += c;
      if (buf.length() > 512) {
        buf = buf.substring(0, 512); 
      }
    }
  }

  // se não teve nada, retornar UNKNOWN_CMD
  return UNKNOWN_CMD;
}

void serial::print(String MESSAGE) {
  Serial.println(MESSAGE);
}

String serial::getBuffer() {
  return BUFFER;
}
