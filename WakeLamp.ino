#include "src/external-communication/serial/serial.hpp"
#include "src/lamp/lamp.hpp"
#include "src/time/timeController.hpp"

int relay_1 = 12;
int relay_2 = 13;
int button = 14;

lamp myLamp(relay_1, relay_2, button);
serial mySerial(115200);
timeController myTime();

void setup() {
  
}

void loop() {

}