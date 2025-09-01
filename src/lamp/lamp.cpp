#include "lamp.hpp"

lamp::lamp(uint8_t lamp_1_pin = 12, uint8_t lamp_2_pin = 13, uint8_t button_pin = 14){
	lamp_1 = lamp_1_pin;
	lamp_2 = lamp_2_pin;
	button = button_pin;

	pinMode(lamp_1, OUTPUT);
	pinMode(lamp_2, OUTPUT);
	pinMode(button, INPUT_PULLUP);
}

void lamp::lamp_1_on(void){
	digitalWrite(lamp_1, HIGH);
}

void lamp::lamp_1_off(void){
	digitalWrite(lamp_1, LOW);
}

void lamp::lamp_2_on(void){
	digitalWrite(lamp_2, HIGH);
}

void lamp::lamp_2_off(void){
	digitalWrite(lamp_2, LOW);
}