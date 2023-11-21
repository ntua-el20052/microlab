#define F_CPU 16000000UL
#include<avr/io.h>
#include<avr/interrupt.h>
#include<util/delay.h>

void lcd_init() {
    // Wait for 200 milliseconds
    _delay_ms(200);

    // Switch to 8-bit mode (3 times as per your assembly code)
    for (uint8_t i = 0; i < 3; ++i) {
        PORTD=0x30;
        enable_pulse();
        _delay_ms(10);
    }

    // Switch to 4-bit mode
    PORTD=0x20;
    enable_pulse();
    _delay_ms(10);

    // Set display parameters
    lcd_command(0x28); // 5x8 dots, 2 lines
    lcd_command(0x0C); // Display on, cursor off
    lcd_clear_display();
    lcd_command(0x06); // Increase address, no display shift
}

void enable_pulse() {
    PORTD |=0x08;  // Set Enable (PD3) high
    _delay_ms(10);
    PORTD &= 0xF7;  // Clear Enable (PD3) low
}

// Assume lcd_command, lcd_clear_display, wait_msec, and wait_usec functions are defined similarly in C.
// Replace these with your actual implementations.

void lcd_command(uint8_t command) {
    // Set LCD_RS (PD2) to 0 for instruction
    PORTD &=0xFB;

    // Send instruction using write_2_nibbles function
    write_2_nibbles(command);

    // Wait for 250 microseconds
    _delay_ms(10);
}

void write_2_nibbles(uint8_t data) {
    uint8_t temp = 0;

    // Save the original LCD_Data
    uint8_t lcd_data = data;

    // Read PIND and isolate the lower 4 bits
    uint8_t lower_bits = PIND & 0x0F;

    // Prepare the high nibble
    data &= 0xF0;
    temp = (PORTD & 0xF0) | (data >> 4);

    // Write the high nibble to PORTD
    PORTD = temp;

    // Enable Pulse
    PORTD |= 0x08;
    _delay_ms(10);   
    PORTD &= 0xF7;

    // Recover r24(LCD_Data)
    data = lcd_data;

    // Prepare the low nibble
    temp = (PORTD & 0xF0) | (data & 0x0F);

    // Write the low nibble to PORTD
    PORTD = temp;

    // Enable Pulse
    PORTD |= 0x08;
    _delay_ms(10);   
    PORTD &= 0xF7;
}

void lcd_clear_display() {
  // Clear display command
    lcd_command(0x01);

    // Wait 5 mSec
    _delay_ms(5);
   
}
void lcd_data(uint8_t data) {
    // Set LCD_RS (PD2) to 1 for data
    PORTD |= 0x02;

    // Send data using write_2_nibbles function
    write_2_nibbles(data);

    // Wait for 250 microseconds
    _delay_ms(250);
}
int main(void){
    DDRD=0XFF;
    lcd_init();
    _delay_ms(1000);
    while(1){
    lcd_clear_display();
    _delay_ms(1000);
    lcd_data(1);
    _delay_ms(1000);
    
    }


}
