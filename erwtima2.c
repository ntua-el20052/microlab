#define F_CPU 16000000UL
#include "avr/io.h"
#include "util/delay.h"


int main(){
    
    TCCR1A =(1 << WGM10) | (1 << COM1A1);
    TCCR1B =(1 << WGM12) | (1 << CS11);
    unsigned char duty=128;
    DDRC |=0b00111111; //C output
    DDRB |=0b00111111; //B output
    DDRD |=0b00000000; //D inpout
    while(1){
        unsigned char inpout;
        inpout = PIND;
        if(inpout == 0b11111101){if (duty <228)duty+=20;} // != exw arnitiki logiki
        if(inpout == 0b11111011){if (duty > 28)duty-=20;}  // != exw arnitiki logiki
        PINC = PINB;
        OCR1AL=duty;
    
    
    }


}
