#define F_CPU 16000000UL
#include "avr/io.h"
#include "util/delay.h"

void adc_init(){
   
    return;
}

uint8_t table [9]={0, 32, 64, 96. 128, 160, 192, 224,255};

// i idea einai sto if na valoume ena switch me to table alla variemai twra, isws to valw chat gpt na to vgalei

void open(){
        ADCSRA |=(1 << ADSC);
        uint8_t temp; 
        while(1){
            temp=ADCSRA;
            temp=temp & 64; //0b01000000; apomonosi ADSC
            if(temp==0)break;
       }
        uint8_t adcvalueh,adcvaluel;
        adcvaluel=ADCL;
        adcvalueh=ADCH;
        //ORTD=adcvalueh;
       //PORTD = adcvalueh ;
        //PORTD =  ADCH;
        if(adcvalueh>0 && adcvalueh<=32){PORTD=0b00000001;}
        if(adcvalueh>32 && adcvalueh<=64){PORTD=0b00000010;}
        if(adcvalueh>64 && adcvalueh<=96){PORTD=0b00000100;}
        if(adcvalueh>96 && adcvalueh<=128){PORTD=0b00001000;}
        if(adcvalueh>128 && adcvalueh<=160){PORTD=0b00010000;}
        if(adcvalueh>160 && adcvalueh<=192){PORTD=0b00100000;}
        if(adcvalueh>192 && adcvalueh<=224){PORTD=0b01000000;}
        if(adcvalueh>224 && adcvalueh<=255){PORTD=0b10000000;}
        

        return;

}


int main(){
    
    TCCR1A =(1 << WGM10) | (1 << COM1A1);
    TCCR1B =(1 << WGM12) | (1 << CS11);
    ADMUX =0b01100001;
    ADCSRA =0b10000111;
    uint8_t duty=134;
    //OCR1AL=duty;
    DDRD |=0b00000000; //D input
    DDRB |=0b11111111; //B output
    DDRC |=0b11111111; //C output
    int mode=1;
   while(1){
        uint8_t inpout;
        input = PIND;
        _delay_ms(100);            //delay kuriws gia debouncing
        input = input & 192;    //apomonosi button p6-p7 as 0b11000000 = 192
        if(input==64)        {mode=1;}// != exw arnitiki logiki  PRESSED P1
        else if(input==128)   {mode=2;} // != exw arnitiki logiki PRESSED P2

       if (mode==1){
          

         
         }
     else {


       
         }
    }
}



