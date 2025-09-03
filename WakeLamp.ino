#include "src/external-communication/serial/serial.hpp"
#include "src/external-communication/wifi/wifiController.hpp"
#include "src/lamp/lamp.hpp"
#include "src/time/timeController.hpp"

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
      case GET_TIME:
        break;
      case UPDATE_TIME:
        break;
      case SET_WAKE_TIME:
        break;
      case GET_WAKE_TIME:
        break;
      case SET_LAMP_INTERVAL:
        break;
      case GET_LAMP_INTERVAL:
        break;
      case GET_ALL_SSIDS:
        myWifi.getAllSSIDS();  
        break;
      case CONNECT:
        break;
      case DISCONNECT:
        break;
      case GET_IS_CONNECTED:
        break;
      case GET_IP_ADDRESS:
        break;
      case GET_SSID:
        break;
      case GET_STATUS:
        break;

      default:
        mySerial.print("Unknown command.");
        break;
    }
  }
}
