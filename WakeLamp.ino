#include "src/external-communication/serial/serial.hpp"
#include "src/external-communication/wifi/wifiController.hpp"
#include "src/lamp/lamp.hpp"
#include "src/time/timeController.hpp"
#include "src/utils/string/string-editions.hpp"
#include "Arduino.h"

int RELAY1 = 12;
int RELAY2 = 13;
int BUTTON = 14;

lamp myLamp(RELAY1, RELAY2, BUTTON);
serial mySerial(115200);
wifiController myWifi; 
timeController myTime;

void setup() {

}

void loop() {
  uint32_t command = mySerial.processSerial();
  if (command != UNKNOWN_CMD) {
    switch (command) {
      case GET_TIME: {
        break;
      }
      case UPDATE_TIME: {
        break;
      }
      case SET_WAKE_TIME: {
        break;
      }
      case GET_WAKE_TIME: {
        break;
      }
      case SET_LAMP_INTERVAL: {
        break;
      }
      case GET_LAMP_INTERVAL: {
        break;
      }
      case GET_ALL_SSIDS: {
        myWifi.getAllSSIDS();  
        break;
      }
      case CONNECT:{
        String current_buffer = mySerial.getBuffer();
        if (current_buffer.startsWith(String(CONNECT))) {
          current_buffer = removePrefix(current_buffer);
          int dotIndex = current_buffer.indexOf('.');
          String SSID = current_buffer.substring(0, dotIndex);
          String PASS = current_buffer.substring(dotIndex + 1);
          myWifi.connect(SSID, PASS);
        }
        break;
      }
      case DISCONNECT: {
        myWifi.disconnect();
        break;
      }
      case GET_IS_CONNECTED: {
        myWifi.isConnected();
        break;
      }
      case GET_IP_ADDRESS: {
        myWifi.getIPAddress();
        break;
      }
      case GET_SSID: {
        myWifi.getSSID();
        break;
      }
      case GET_STATUS: {
        break;
      }

      default: {
        mySerial.print("Unknown command.");
        break;
      }
    }
  }
}
