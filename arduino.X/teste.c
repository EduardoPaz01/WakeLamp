/*
 * AVR I2C LCD (PCF8574) - Blocking Implementation
 * F_CPU = 16 MHz, no TWI interrupts for LCD
 */

#define F_CPU 16000000UL
#include <util/delay.h>
#include <avr/io.h>
#include <avr/interrupt.h>

// ===== I2C LCD PCF8574 =====
#define LCD_ADDR       0x27    // experimente 0x3F se não funcionar
#define LCD_BACKLIGHT  0x08
#define ENABLE         0x04
#define RS             0x01

// ===== Clock =====
volatile uint8_t hours = 0, minutes = 0, seconds = 0;
volatile int update_display = 0;

// ===== Prototypes =====
void timer1_init(void);
void twi_init(void);
void lcd_send_blocking(uint8_t byte, uint8_t mode);
void lcd_init_blocking(void);
void lcd_set_pos_blocking(uint8_t col, uint8_t row);
void lcd_print_blocking(const char *str);
void lcd_show_time_blocking(uint8_t h, uint8_t m, uint8_t s);

// ===== Blocking send of one byte (4-bit mode) =====
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

// ===== LCD Initialization (blocking) =====
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

// ===== Helper routines =====
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

// ===== ISRs =====
ISR(TIMER1_COMPA_vect) {
    if (++seconds >= 60) {
        seconds = 0;
        if (++minutes >= 60) {
            minutes = 0;
            hours = (hours + 1) % 24;
        }
    }
    update_display = 1;
}

// ===== Inits =====
void timer1_init(void) {
    // CTC, prescaler 1024
    TCCR1B = (1<<WGM12)|(1<<CS12)|(1<<CS10);
    OCR1A = 15624;
    TIMSK1 = (1<<OCIE1A);
}

void twi_init(void) {
    TWSR = 0;
    TWBR = 32;           // ~100 kHz
    // clear TWINT & enable TWI (no interrupts)
    TWCR = (1<<TWINT)|(1<<TWEN);
}

int main(void) {
    timer1_init();
    twi_init();

    // blocking LCD init
    lcd_init_blocking();

    // habilita interrupções
    sei();

    // imprime cabeçalho
    lcd_set_pos_blocking(4, 0);
    lcd_print_blocking("Horario:");

    while (1) {
        if (update_display) {
            lcd_show_time_blocking(hours, minutes, seconds);
            update_display = 0;
        }
    }
    return 0;
}
