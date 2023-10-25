#define F_CPU 16000000UL
#include "avr/io.h"
#include "util/delay.h"

void adc_init(){
   
    return;
}

int calc(){
    ADCSRA |=(1 << ADSC);
    unsigned char temp; 
        while(1){
            temp=ADCSRA;
            temp=temp&0b01000000;
            if(temp==0b00000000){
                
                break;
            }
       }

}


int open(unsigned char duty){
        
        uint8_t adcvalueh,adcvaluel;
        adcvaluel=ADCL;
        adcvalueh=ADCH;
        //ORTD=adcvalueh;
       //PORTD = adcvalueh ;
        //PORTD =  ADCH;
        if(adcvalueh>0x00 && adcvalueh<=0x20){PORTD=0b00000001;}
        if(adcvalueh>0x20 && adcvalueh<=0x40){PORTD=0b00000010;}
        if(adcvalueh>0x40 && adcvalueh<=0x60){PORTD=0b00000100;}
        if(adcvalueh>0x60 && adcvalueh<=0x80){PORTD=0b00001000;}
        if(adcvalueh>0x80 && adcvalueh<=0xA0){PORTD=0b00010000;}
        if(adcvalueh>0xA0 && adcvalueh<=0xC0){PORTD=0b00100000;}
        if(adcvalueh>0xC0 && adcvalueh<=0xE0){PORTD=0b01000000;}
        if(adcvalueh>0xE0 && adcvalueh<=0x100){PORTD=0b10000000;}
        

        return 2;

}


int main(){
    
    TCCR1A =(1 << WGM10) | (1 << COM1A1);
    TCCR1B =(1 << WGM12) | (1 << CS11);
    ADMUX =0b01100001;
    ADCSRA =0b10000111;
    unsigned char duty=134;
    //OCR1AL=duty;
    DDRB |=0b00000010; //B output
    DDRD |=0b11111111; //B output
    //DDRD |=0b11111111; //D inpou
   while(1){
        unsigned char inpout;
       

        
        
         inpout = PINB;
        _delay_ms(100);
        inpout = inpout&0b00110000;
        if(inpout==0b00100000){if(duty<245){duty+=20;}}// != exw arnitiki logiki
        else if(inpout==0b00010000){if(duty>25){duty-=20;} } // != exw arnitiki logiki
        OCR1AL=duty;
         
        calc();
       open(duty);
         
    }
}



