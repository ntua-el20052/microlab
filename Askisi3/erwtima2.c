#define F_CPU 16000000UL
#include "avr/io.h"
#include "util/delay.h"


int main(){
    
    TCCR1A =(1 << WGM10) | (1 << COM1A1);
    TCCR1B =(1 << WGM12) | (1 << CS11);
    ADMUX |=0b01100001;
    ADCSRA |=0b10000111;
    unsigned char duty=128;
    DDRC |=0b00111111; //C output
    DDRB |=0b00111111; //B output
    DDRD |=0b00000000; //D inpou
    while(1){
        
        unsigned char inpout,temp,adcvalue;
        inpout = PIND;
        _delay_ms(100);
        if(inpout == 0b11111110){if (duty <228)duty=227;} // != exw arnitiki logiki
        if(inpout == 0b11111101){if (duty > 0)duty=1;}  // != exw arnitiki logiki
        
        OCR1AL=duty;
        
        
        ADCSRA|=(1 << ADSC);
        while(1){
            temp=ADCSRA;
            temp=temp&0b01000000;
            if(temp==0b00000000){
                break;
            }
        }
        adcvalue=ADCL;
        if(adcvalue>0b10000000){PORTC=0b00011111;}
        else if(adcvalue<0b10000000){PORTC=0b00101111;}
        }
        
        
    
    }



