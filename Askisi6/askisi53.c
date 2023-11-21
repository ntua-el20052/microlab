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
            result = '4';
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
            result=0x00;  // Indicate an error if the value is not 1 or 2
    }
    return result;
}

void open(){
    for(int i=0; i<10; i++){
        PORTB=0xFF;
        _delay_ms(250);
        PORTB=0x00;
        _delay_ms(250);
    }
}






int main(void) {
 twi_init();
 PCA9555_0_write(REG_CONFIGURATION_0, 0x00); //Set EXT_PORT0 as output
 PCA9555_0_write(REG_CONFIGURATION_1, 0xF0); //Set for EXT_PORT1 P4-P7 as input and P0 as output
 DDRB=0xFF;
 while(1)
 {
    PORTB=0x00;
    char read = keypad_to_ascii();
    while( read == 0){read=keypad_to_ascii();}
    if (read=='5'){
        _delay_ms(500);
        read = keypad_to_ascii();
        while( read == 0){read=keypad_to_ascii();}
        
        if(read=='5'){PORTB=0xFF;_delay_ms(4000); continue;}
        else {open(); continue;}
    }
    else{
        _delay_ms(500);
        read = keypad_to_ascii();
        while( read == 0){read=keypad_to_ascii();};
        open();
    }
    
    //read=read>>4;  
    
    //PCA9555_0_write(REG_OUTPUT_0, read);


 }
}