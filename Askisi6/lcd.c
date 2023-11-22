#define F_CPU 16000000UL
#include<avr/io.h>
#include<avr/interrupt.h>
#include<util/delay.h>
#include "askisi5.h"


uint8_t scan_row(uint8_t row){
    row=~row;
    PCA9555_0_write(REG_OUTPUT_1, row); //Set I0row as 1 to enable pull up
    uint8_t read=PCA9555_0_read(REG_INPUT_1);
    read=~read;
    read=read>>4;
    PCA9555_0_write(REG_OUTPUT_1, 0xFF);
    return read;
}


uint16_t scan_keypad(){
    uint16_t tmp=0x00;
    tmp=scan_row(8);
    tmp=tmp<<4;
    tmp|=scan_row(4);
    tmp=tmp<<4;
    tmp|=scan_row(2);
    tmp=tmp<<4;
    tmp|=scan_row(1);
    return tmp;
}
uint16_t scan_keypad_rising_edge(){
    uint16_t tmp=0,read=0;
    read= scan_keypad();
    while(1){
        tmp=scan_keypad();
        if(read==tmp)break;
       _delay_ms(50);
        read= scan_keypad();
    }
    
    return read;
}
uint16_t tmp=0,tmp2=0;

char keypad_to_ascii(){
    uint16_t read = scan_keypad_rising_edge();
    char result;
    
    switch (read) {
        case 0x0100:
            result= '4';
            break;
        case 0x0200:
            result = '5';
            break;
        case 0x0400:
            result = '6';
            break;
        case 0x0800:
            result = 'B';
            break;
        case 0x1000:
            result = '1';
            break;
        case 0x2000:
            result = '2';
            break;
        case 0x4000:
            result = '3';
            break;
        case 0x8000:
            result = 'A';
            break;
        case 0x0010:
            result = '7';
            break;
        case 0x0020:
            result = '8';
            break;
        case 0x0040:
            result = '9';
            break;
        case 0x0080:
            result = 'C';
            break;
        case 0x0001:
            result = '*';
            break;
        case 0x0002:
            result = '0';
            break;
        case 0x0004:
            result = '#';
            break;
        case 0x0008:
            result = 'D';
            break;
        default:
            result=' ';  // Indicate an error if the value is not 1 or 2
    }
    return result;
}


void lcd_init() {
    // Wait for 250 milliseconds
    _delay_ms(250);

    // Switch to 8-bit mode (3 times as per your assembly code)
    for (int i = 0; i < 3; ++i) {
        PORTD=0x30;
        enable_pulse();
        _delay_ms(1);
    }

    // Switch to 4-bit mode
    PORTD=0x20;
    enable_pulse();
    _delay_ms(1);

    // Set display parameters
    lcd_command(0x28); // 5x8 dots, 2 lines
    lcd_command(0x0C); // Display on, cursor off
    lcd_clear_display();
    lcd_command(0x06); // Increase address, no display shift
}

void enable_pulse() {
    PORTD |=0x08;  // Set Enable (PD3) high
    _delay_ms(1);
    PORTD &= 0xF7;  // Clear Enable (PD3) low
}

void lcd_command(uint8_t command) {
    // clear LCD_RS (PD2) for instruction
    PORTD &=0b11111011;

    // Send instruction using write_2_nibbles function
    write_2_nibbles(command);

    _delay_ms(1);
}

void write_2_nibbles(uint8_t data) {
    uint8_t temp = PIND & 0x0f;
    
    uint8_t hi=data & 0xf0;
    hi= hi + temp;
    PORTD = hi;
    
    PORTD |= 0b00001000;
    _delay_us(50);
    PORTD &= 0b11110111;
    
    uint8_t lo=data & 0x0f;
    lo= lo << 4;
    lo= lo + temp;
    PORTD = lo;
    
    PORTD |= 0b00001000;
    _delay_us(50);
    PORTD &= 0b11110111;
    
    
   
}

void lcd_clear_display() {
  // Clear display command
    lcd_command(0x01);
    // Wait 5 mSec
    _delay_ms(10);
}
void lcd_data(uint8_t data) {
    // Set LCD_RS (PD2) to 1 for data
    PORTD |= 0x04;

    // Send data using write_2_nibbles function
    write_2_nibbles(data);

    // Wait for 250 millioseconds opte 1 micro einai ok
    _delay_ms(1);
}



int main(void){
    twi_init();
    PCA9555_0_write(REG_CONFIGURATION_0, 0x00); //Set EXT_PORT0 as output
   PCA9555_0_write(REG_CONFIGURATION_1, 0xF0); //Set for EXT_PORT1 P4-P7 as input and P0 as output
    DDRD=0XFF;
    lcd_init();
    _delay_ms(200);
    while(1){
    lcd_clear_display();
    _delay_ms(100);
    char read=keypad_to_ascii();
     while( read == ' '){read=keypad_to_ascii();}
    _delay_ms(100);
    
    lcd_data(read);
    _delay_ms(1000);
    }
}

