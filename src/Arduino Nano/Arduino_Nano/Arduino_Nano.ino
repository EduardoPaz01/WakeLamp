#include <Arduino.h>
#include <avr/sleep.h>
#include <avr/interrupt.h>

// Pinos
const int PIN_INT0 = 2;   // INT0 no Arduino Nano
const int PIN_OUT_1  = A1;  // Saída digital (relé)
const int PIN_OUT_2  = A2;  // Saída digital (relé)
const int LED_OUT = 13;    // LED para indicar atividade

// Controle de debounce
volatile bool intFlag = false;
bool currentState = LOW;

// ISR da interrupção externa INT0
void ISR_int0() {
  intFlag = true;
}

void setup() {
  Serial.begin(115200);
  pinMode(PIN_OUT_1, OUTPUT);
  pinMode(PIN_OUT_2, OUTPUT);
  pinMode(LED_OUT, OUTPUT);
  digitalWrite(PIN_OUT_1, LOW);
  digitalWrite(PIN_OUT_2, LOW);
  digitalWrite(LED_OUT, LOW);
  pinMode(PIN_INT0, INPUT); // Entrada para receber sinal do ESP32

  // Configura interrupção externa em qualquer mudança de nível
  attachInterrupt(digitalPinToInterrupt(PIN_INT0), ISR_int0, RISING);

  // Configura modo de sono
  set_sleep_mode(SLEEP_MODE_IDLE);
}

void loop() {
  if (intFlag) {
    intFlag = false;

    // XOR / toggle
    currentState = !currentState;

    digitalWrite(PIN_OUT_1, currentState);
    digitalWrite(PIN_OUT_2, currentState);
    digitalWrite(LED_OUT, currentState); // Indica atividade no LED
  }

  // ---- Economia de energia ----
  sleep_enable();
  sleep_mode();   // Dorme até próxima interrupção (INT0 ou Timer1)
  sleep_disable();
}
