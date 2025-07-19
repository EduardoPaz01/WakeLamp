#define F_CPU 16000000UL
#include <avr/io.h>
#include <avr/interrupt.h>

#define SBI(y,bit)(y|=(1<<bit));
#define CBI(y,bit)(y&=~(1<<bit));

#define RELAY_PORT PORTD
#define RELAY_DDR DDRD
#define RELAY_PIN PD2
#define LED_PORT PORTB
#define LED_DDR DDRB
#define LED_PIN PB5

volatile uint8_t seconds = 0;
volatile uint8_t minutes = 0;
volatile uint8_t hours = 0;

//? BUFFER
#define CMD_BUFFER_SIZE 15
volatile char rx_buffer[CMD_BUFFER_SIZE];
volatile uint8_t rx_index = 0;

// PROTOTYPES
void relay_on();
void relay_off();
void timer1_init();
ISR(TIMER1_COMPA_vect);
ISR(USART_RX_vect);
void uart_init(uint16_t ubrr);
void uart_send_char(char c);
void uart_send_str(const char* str);


int main(void) {
    RELAY_DDR |= (1 << RELAY_PIN);
    LED_DDR |= (1 << LED_PIN);
    relay_off();

    // START HOUR
    hours = 0;
    minutes = 0;
    seconds = 0;

    timer1_init();
    uart_init(103);  // 9600 bps com 16MHz ? UBRR = 103
    sei();

    while (1) {}
    return 1;
}

// RELAY CONFIG
void relay_on() {
    SBI(RELAY_PORT, RELAY_PIN);
    SBI(LED_PORT, LED_PIN);  
}
void relay_off() {
    CBI(RELAY_PORT, RELAY_PIN);
    CBI(LED_PORT, LED_PIN); 
}

// TIMER START
void timer1_init() {
    TCCR1B |= (1 << WGM12);              // CTC
    TCCR1B |= (1 << CS12) | (1 << CS10); // PRESCALER 1024
    OCR1A = 15624;                       // MATCH 1s (16MHz / 1024 = 15625)
    TIMSK1 |= (1 << OCIE1A);             // INTERRUPR COMPARE MATCH A
}

// INTERRUPT'S
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

    // ON 5:30
    if (hours == 5 && minutes == 30 && seconds == 0) {
        relay_on();
    }
    // OF 7:30
    if (hours == 7 && minutes == 30 && seconds == 0) {
        relay_off();
    }
}
ISR(USART_RX_vect) {
    // RECEIVE
    char received = UDR0;
    if (rx_index < CMD_BUFFER_SIZE - 1) {
        rx_buffer[rx_index++] = received;
        rx_buffer[rx_index] = '\0'; 
    }
    // $GET
    if (rx_index == 4 &&
        rx_buffer[0] == '$' &&
        rx_buffer[1] == 'G' &&
        rx_buffer[2] == 'E' &&
        rx_buffer[3] == 'T') 
    {
        // GET TIME
        cli();
        uint8_t h = hours;
        uint8_t m = minutes;
        uint8_t s = seconds;
        sei();
        
        // RETURN
        uart_send_str("$SMH.");
        uart_send_char('0' + h / 10);
        uart_send_char('0' + h % 10);
        uart_send_char('.');
        uart_send_char('0' + m / 10);
        uart_send_char('0' + m % 10);
        uart_send_char('.');
        uart_send_char('0' + s / 10);
        uart_send_char('0' + s % 10);
        uart_send_char('\n');

        // FINISH COMMUNICATION
        rx_index = 0;  
    }
    // $SET.HH.MM.SS
    if (rx_index == 14 &&
        rx_buffer[0] == '$' &&
        rx_buffer[1] == 'S' &&
        rx_buffer[2] == 'E' &&
        rx_buffer[3] == 'T' &&
        rx_buffer[4] == '.' &&
        rx_buffer[7] == '.' &&
        rx_buffer[10] == '.') {
        // ASCII TO INT 
        uint8_t h = (rx_buffer[5] - '0') * 10 + (rx_buffer[6] - '0');
        uint8_t m = (rx_buffer[8] - '0') * 10 + (rx_buffer[9] - '0');
        uint8_t s = (rx_buffer[11] - '0') * 10 + (rx_buffer[12] - '0');
        
        // SET 
        if (h < 24 && m < 60 && s < 60) {
            // SET TIME
            cli(); 
            hours = h;
            minutes = m;
            seconds = s;
            sei();
            
            // SEND CONFIRMATION
            uart_send_str("$NHMS.");
            uart_send_char('0' + h / 10);
            uart_send_char('0' + h % 10);
            uart_send_char('.');
            uart_send_char('0' + m / 10);
            uart_send_char('0' + m % 10);
            uart_send_char('.');
            uart_send_char('0' + s / 10);
            uart_send_char('0' + s % 10);
            uart_send_char('\n');
        }
        
        // FINISH COMMUNICATION
        rx_index = 0;  
    }
}

// UART COMMUNICATION
void uart_init(uint16_t ubrr) {
    // SET BAUD RATE
    UBRR0H = (uint8_t)(ubrr >> 8);
    UBRR0L = (uint8_t)ubrr;

    // ENABLE RX, TX AND INTERRUPT
    UCSR0B = (1 << RXEN0) | (1 << RXCIE0) | (1 << TXEN0);

    // 8 DATA BITS, 1 STOP BIT, NO PARITY
    UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);
}
void uart_send_char(char c) {
    while (!(UCSR0A & (1 << UDRE0)));  // Espera buffer vazio
    UDR0 = c;
}
void uart_send_str(const char* str) {
    while (*str) {
        uart_send_char(*str++);
    }
}