#include "src/external-communication/serial/serial.hpp"
#include "src/lamp/lamp.hpp"
#include "src/time/timeController.hpp"

int RELAY1 = 12;
int RELAY2 = 13;
int BUTTON = 14;

lamp myLamp(RELAY1, RELAY2, BUTTON);
serial mySerial(115200);
timeController myTime();

void setup() {

}

void loop() {
  uint32_t command = mySerial.processSerial();
  if(command!= UNKNOWN_CMD) 
    mySerial.print(String(command));
}
