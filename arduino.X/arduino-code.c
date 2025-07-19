#define F_CPU 16000000UL
#include <avr/io.h>
#include <avr/interrupt.h>

#define SBI(y,bit)(y|=(1<<bit));
#define CBI(y,bit)(y&=~(1<<bit));

#define RELAY_PORT PORTD
#define RELAY_DDR DDRD
#define RELAY_PIN PD2

volatile uint8_t seconds = 0;
volatile uint8_t minutes = 0;
volatile uint8_t hours = 0;

void relay_on() {
    SBI(RELAY_PORT, RELAY_PIN);
}

void relay_off() {
    CBI(RELAY_PORT, RELAY_PIN);
}

void timer1_init() {
    TCCR1B |= (1 << WGM12);              // CTC
    TCCR1B |= (1 << CS12) | (1 << CS10); // PRESCALER 1024
    OCR1A = 15624;                       // MATCH 1s (16MHz / 1024 = 15625)
    TIMSK1 |= (1 << OCIE1A);             // INTERRUPR COMPARE MATCH A
}

ISR(TIMER1_COMPA_vect) {
    seconds++;
    if (seconds >= 60) {
        seconds = 0;
        minutes++;
        if (minutes >= 60) {
            minutes = 0;
            hours++;
            if (hours >= 24) {
                hours = 0;
            }
        }
    }

    // ON 5;30
    if (hours == 5 && minutes == 30 && seconds == 0) {
        relay_on();
    }
    // OF 7:30
    if (hours == 7 && minutes == 30 && seconds == 0) {
        relay_off();
    }
}

int main(void) {
    RELAY_DDR |= (1 << RELAY_PIN);
    relay_off();

    // START HOUR
    hours = 5;
    minutes = 28;
    seconds = 0;

    timer1_init();
    sei();

    while (1) {}
}
