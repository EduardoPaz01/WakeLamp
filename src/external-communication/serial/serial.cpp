#include "serial.hpp"

serial::serial(int FREQUENCY_SERIAL) {
  FREQUENCY = FREQUENCY_SERIAL;
  Serial.begin(FREQUENCY);
}

serial::~serial(void) {
  Serial.end();
}

uint32_t serial::processSerial(void) {
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

        if (CACHE_LINE == "GET.TIME"){
          return_value = GET_TIME;
        }
        else if (CACHE_LINE == "UPDATE.TIME"){
          return_value = UPDATE_TIME;
        }
        else if (CACHE_LINE.startsWith("SET.WAKE.TIME.")) {
          CACHE_LINE = CACHE_LINE.substring(14);
          BUFFER = CACHE_LINE;
          return_value = SET_WAKE_TIME;
        } 
        else if (CACHE_LINE == "GET.WAKE.TIME") {
          return_value = GET_WAKE_TIME;
        }
        else if (CACHE_LINE.startsWith("SET.LAMP.INTERVAL.")) {
          CACHE_LINE = CACHE_LINE.substring(18);
          BUFFER = CACHE_LINE;
          return_value = SET_LAMP_INTERVAL;
        }         
        else if (CACHE_LINE == "GET.LAMP.INTERVAL") {
          return_value = GET_LAMP_INTERVAL;
        }
        else if (CACHE_LINE == "GET.ALL.SSIDS") 
          return_value = GET_ALL_SSIDS;
        else if (CACHE_LINE.startsWith("CONNECT.")) {
          CACHE_LINE = CACHE_LINE.substring(8);
          BUFFER = CACHE_LINE;
          return_value = CONNECT;
        } 
        else if(CACHE_LINE == "DISCONNECT"){
          return_value = DISCONNECT;
        }
        else if(CACHE_LINE == "GET.IS.CONNECTED"){
          return_value = GET_IS_CONNECTED;
        }
        else if(CACHE_LINE == "GET.IP.ADDRESS"){
          return_value = GET_IP_ADDRESS;
        }
        else if(CACHE_LINE == "GET.SSID"){
          return_value = GET_SSID;
        }
        else if(CACHE_LINE == "GET.STATUS"){
          return_value = GET_STATUS;
        }
        else {
          return_value = UNKNOWN_CMD;
        }
      } else {
        return_value = UNKNOWN_CMD;
      }

      CACHE_LINE = "";
      return return_value; 
    } else {
      buf += c;
      if (buf.length() > 1024) {
        buf = buf.substring(0, 1024); 
      }
    }
  }

  return UNKNOWN_CMD;
}

void serial::print(String MESSAGE) {
  Serial.println(MESSAGE);
}

String serial::getBuffer(void) {
  return BUFFER;
}
