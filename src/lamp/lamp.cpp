#include "lamp.hpp"

lamp::lamp(uint8_t LAMP1_PIN, uint8_t LAMP2_PIN, uint8_t BUTTON_PIN){
	LAMP1 = LAMP1_PIN;
	LAMP2 = LAMP2_PIN;
	BUTTON = BUTTON_PIN;

	pinMode(LAMP1, OUTPUT);
	pinMode(LAMP2, OUTPUT);
	pinMode(BUTTON, INPUT_PULLUP);
}

void lamp::lamp1On(void){
	digitalWrite(LAMP1, HIGH);
}

void lamp::lamp1Off(void){
	digitalWrite(LAMP1, LOW);
}

void lamp::lamp2On(void){
	digitalWrite(LAMP2, HIGH);
}

void lamp::lamp2Off(void){
	digitalWrite(LAMP2, LOW);
}