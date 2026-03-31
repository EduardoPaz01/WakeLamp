#include "lamp.hpp"

lamp::lamp() {
  lampCurrentState = false;
}

void lamp::lampOn()  { 
  if(!lampCurrentState) toggleLamp(); 
}

void lamp::lampOff() {
  if(lampCurrentState) toggleLamp(); 
}

void lamp::toggleLamp(){ 
  lampDesiredState = true; 
}

void lamp::updatedLamp(){
  if(lampDesiredState){
    if (millis() - lastToggleTime < TOGGLE_DELAY) return;

    lastToggleTime = millis();

    digitalWrite(OUTPUT_PIN, HIGH);
    digitalWrite(LED_PIN, HIGH);

    delay(10); // 10 ms

    digitalWrite(OUTPUT_PIN, LOW);
    digitalWrite(LED_PIN, LOW);

    lampDesiredState = false;
    lampCurrentState = !lampCurrentState;
  }
}
