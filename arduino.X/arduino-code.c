#define F_CPU 16000000UL
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

// MACROS
#define SBI(y,bit)(y|=(1<<bit));
#define CBI(y,bit)(y&=~(1<<bit));
#define RELAY_PORT      PORTD
#define RELAY_DDR       DDRD
#define RELAY_PIN_1     PD2
#define RELAY_PIN_2     PD3
#define LED_PORT        PORTB
#define LED_DDR         DDRB
#define LED_PIN         PB5
#define CMD_BUFFER_SIZE 16
#define LCD_ADDR        0x27   
#define LCD_BACKLIGHT   0x08
#define ENABLE          0x04
#define RS              0x01
#define ERR_MSG "$ERR\n"

// TIME
volatile uint8_t seconds = 0, minutes = 0, hours = 0;
volatile int update_display = 0;

// BUFFER UART
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
void twi_init(void);
void lcd_send_blocking(uint8_t byte, uint8_t mode);
void lcd_init_blocking(void);
void lcd_set_pos_blocking(uint8_t col, uint8_t row);
void lcd_print_blocking(const char *str);
void lcd_show_time_blocking(uint8_t h, uint8_t m, uint8_t s);

int main(void) {
    RELAY_DDR |= (1 << RELAY_PIN_1);
    //RELAY_DDR |= (1 << RELAY_PIN_2);
    LED_DDR |= (1 << LED_PIN);
    relay_off();

    // START HOUR
    hours = 0;
    minutes = 0;
    seconds = 0;

    // TIMER + UART + I2C
    timer1_init();
    uart_init(103);  // 9600 BPS => UBRR = 103
    twi_init();
    
    // BLOCKING LCD INIT
    lcd_init_blocking();
    sei();
    
    // HEADER LCD
    lcd_set_pos_blocking(4, 0);
    lcd_print_blocking("Horario:");

    // LOOPING
    while (1) {
        if (update_display) {
            lcd_show_time_blocking(hours, minutes, seconds);
            update_display = 0;
        }
    }
    return 1;
}

// RELAY CONFIG
void relay_on() {
    CBI(RELAY_PORT, RELAY_PIN_1);
    //CBI(RELAY_PORT, RELAY_PIN_2);
    SBI(LED_PORT, LED_PIN);  
}
void relay_off() {
    SBI(RELAY_PORT, RELAY_PIN_1);
    //SBI(RELAY_PORT, RELAY_PIN_2);
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

    if (hours == 5 && minutes == 30 && seconds == 0) {
        relay_on();
    }
    if (hours == 7 && minutes == 30 && seconds == 0) {
        relay_off();
    }
    
    update_display = 1;
}
#define ERR_MSG "$ERR\n"

ISR(USART_RX_vect) {
    // RECEIVE
    char received = UDR0;

    if (rx_index < CMD_BUFFER_SIZE - 1 && received != '\n' && received != '\r') {
        rx_buffer[rx_index++] = received;
        rx_buffer[rx_index] = '\0';
    }

    if (received == '\n' || received == '\r' || rx_index >= CMD_BUFFER_SIZE - 1) {

        // $GET 
        if (rx_index == 4 &&
            rx_buffer[0] == '$' &&
            rx_buffer[1] == 'G' &&
            rx_buffer[2] == 'E' &&
            rx_buffer[3] == 'T') 
        {
            cli();
            uint8_t h = hours;
            uint8_t m = minutes;
            uint8_t s = seconds;
            sei();

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
        }

        // $SET.HH.MM.SS
        else if (rx_index == 13 &&
                 rx_buffer[0] == '$' &&
                 rx_buffer[1] == 'S' &&
                 rx_buffer[2] == 'E' &&
                 rx_buffer[3] == 'T' &&
                 rx_buffer[4] == '.' &&
                 rx_buffer[7] == '.' &&
                 rx_buffer[10] == '.') 
        {
            uint8_t h = (rx_buffer[5] - '0') * 10 + (rx_buffer[6] - '0');
            uint8_t m = (rx_buffer[8] - '0') * 10 + (rx_buffer[9] - '0');
            uint8_t s = (rx_buffer[11] - '0') * 10 + (rx_buffer[12] - '0');

            if (h < 24 && m < 60 && s < 60) {
                cli();
                hours = h;
                minutes = m;
                seconds = s;
                sei();

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
            } else {
                uart_send_str(ERR_MSG);
            }
        }

        // $SRY.n
        else if (rx_index == 5 &&
                 rx_buffer[0] == '$' &&
                 rx_buffer[1] == 'S' &&
                 rx_buffer[2] == 'R' &&
                 rx_buffer[3] == '.') 
        {
            uart_send_str("$SR.1\n");
            relay_on();
        }

        // $CRY.n
        else if (rx_index == 5 &&
                 rx_buffer[0] == '$' &&
                 rx_buffer[1] == 'C' &&
                 rx_buffer[2] == 'R' &&
                 rx_buffer[3] == '.') 
        {
            uart_send_str("$CR.1\n");
            relay_off();
        }

        else {
            uart_send_str(ERR_MSG);
        }

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

// I2C COMMUNICATION
void twi_init(void) {
    TWSR = 0;
    TWBR = 32;                   // ~100 kHz
    TWCR = (1<<TWINT)|(1<<TWEN); 
}
void lcd_send_blocking(uint8_t byte, uint8_t mode) {
    uint8_t data;
    // --- START condition ---
    TWCR = (1<<TWINT)|(1<<TWSTA)|(1<<TWEN);
    while (!(TWCR & (1<<TWINT)));
    // --- SLA+W ---
    TWDR = (LCD_ADDR << 1);
    TWCR = (1<<TWINT)|(1<<TWEN);
    while (!(TWCR & (1<<TWINT)));

    // High nibble
    data = ((byte >> 4) & 0x0F) << 4;
    data |= LCD_BACKLIGHT;
    if (mode) data |= RS;
    // ENABLE pulse
    TWDR = data | ENABLE;
    TWCR = (1<<TWINT)|(1<<TWEN);
    while (!(TWCR & (1<<TWINT)));
    TWDR = data;
    TWCR = (1<<TWINT)|(1<<TWEN);
    while (!(TWCR & (1<<TWINT)));

    // Low nibble
    data = (byte & 0x0F) << 4;
    data |= LCD_BACKLIGHT;
    if (mode) data |= RS;
    // ENABLE pulse
    TWDR = data | ENABLE;
    TWCR = (1<<TWINT)|(1<<TWEN);
    while (!(TWCR & (1<<TWINT)));
    TWDR = data;
    TWCR = (1<<TWINT)|(1<<TWEN);
    while (!(TWCR & (1<<TWINT)));

    // STOP condition
    TWCR = (1<<TWINT)|(1<<TWEN)|(1<<TWSTO);
    _delay_us(50);  // small delay to settle
}
void lcd_init_blocking(void) {
    _delay_ms(50); // power-up delay
    // function set 8-bit x3
    for (uint8_t i = 0; i < 3; i++) {
        lcd_send_blocking(0x03, 0);
        _delay_ms(5);
    }
    // set 4-bit mode
    lcd_send_blocking(0x02, 0);
    _delay_ms(5);
    // function set, display on, entry, clear
    lcd_send_blocking(0x28, 0);
    lcd_send_blocking(0x0C, 0);
    lcd_send_blocking(0x06, 0);
    lcd_send_blocking(0x01, 0);
    _delay_ms(2);
}
void lcd_set_pos_blocking(uint8_t col, uint8_t row) {
    uint8_t addr = (row == 0) ? 0x00 : 0x40;
    lcd_send_blocking(0x80 | (addr + col), 0);
}
void lcd_print_blocking(const char *str) {
    while (*str) {
        lcd_send_blocking(*str++, 1);
    }
}
void lcd_show_time_blocking(uint8_t h, uint8_t m, uint8_t s) {
    char buf[9] = {
        '0' + h/10, '0' + h%10, ':',
        '0' + m/10, '0' + m%10, ':',
        '0' + s/10, '0' + s%10,
        '\0'
    };
    uint8_t start_col = (16 - 8) / 2;
    lcd_set_pos_blocking(start_col, 1);
    lcd_print_blocking(buf);
}