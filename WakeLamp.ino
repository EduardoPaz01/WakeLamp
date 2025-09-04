#include "src/external-communication/serial/serial.hpp"
#include "src/external-communication/wifi/wifiController.hpp"
#include "src/lamp/lamp.hpp"
#include "src/time/timeController.hpp"
#include "src/utils/string/string-editions.hpp"
#include "src/time/timeClass.hpp"
#include "Arduino.h"
#include "time.h"

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
  myTime.tick();

  uint32_t command = mySerial.processSerial();
  if (command != UNKNOWN_CMD) {
    switch (command) {
      case GET_TIME: {
        mySerial.print("HOUR:");
        mySerial.print(String(myTime.getTime().tm_hour));
        mySerial.print(" MINUTE:");
        mySerial.print(String(myTime.getTime().tm_min));
        mySerial.print(" SECOND:");
        mySerial.print(String(myTime.getTime().tm_sec));
        break;
      }
      case UPDATE_TIME: {
        myTime.updateTime(myWifi.getTimeFromNTP());
        break;
      }
      case SET_WAKE_TIME: {
        break;
      }
      case GET_WAKE_TIME: {
        mySerial.print("HOUR:");
        mySerial.print(String(myTime.getWakeTime().HOUR));
        mySerial.print(" MINUTE:");
        mySerial.print(String(myTime.getWakeTime().MINUTE));
        mySerial.print(" SECOND:");
        mySerial.print(String(myTime.getWakeTime().SECOND));
        break;
      }
      case SET_LAMP_INTERVAL: {
        break;
      }
      case GET_LAMP_INTERVAL: {
        mySerial.print("HOUR:");
        mySerial.print(String(myTime.getLampInterval().HOUR));
        mySerial.print(" MINUTE:");
        mySerial.print(String(myTime.getLampInterval().MINUTE));
        mySerial.print(" SECOND:");
        mySerial.print(String(myTime.getLampInterval().SECOND));
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
        myWifi.getStatus();
        break;
      }

      default: {
        mySerial.print("Unknown command.");
        break;
      }
    }
  }
}
