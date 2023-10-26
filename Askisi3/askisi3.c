#define F_CPU 16000000UL
#include "avr/io.h"
#include "util/delay.h"

uint8_t duty=134;
//GLOBAL


void adc_init(){
   
    return;
}

uint8_t table [9]={0, 32, 64, 96, 128, 160, 192, 224,255};

// i idea einai sto if na valoume ena switch me to table alla variemai twra, isws to valw chat gpt na to vgalei


void mode1(){
   uint8_t input;
   input = PIND;
   _delay_ms(100);            //delay kuriws gia debouncing
   input = input & 6;    //apomonosi button pd1-pd2 as 0b00000110 = 6
   if(input==4)        {if(duty<245){duty+=20;} }// != exw arnitiki logiki 
   else if(input==2)   {if(duty>25 ){duty-=20;} } // != exw arnitiki logiki
   OCR1AL=duty;
}

void mode2(){
//while(1){                                           //auto to while me to apokatw sxolia- isws xreiastei, trexei mexri na pati8ei allagi mode wste na glitwsw calls sth sinartisi mode2 
   ADCSRA |=(1 << ADSC);
   uint8_t temp; 
   while(1){
      temp=ADCSRA;
      temp=temp & 64; //0b01000000; apomonosi ADSC
      if(temp==0)break;
   }
   uint8_t adcvalueh,adcvaluel, duty2;
   adcvaluel=ADCL;
   adcvalueh=ADCH;
   if(adcvalueh>0 && adcvalueh<=32){duty=250;}
   if(adcvalueh>32 && adcvalueh<=64){duty=200;}
   if(adcvalueh>64 && adcvalueh<=96){duty=170;}
   if(adcvalueh>96 && adcvalueh<=128){duty=140;}
   if(adcvalueh>128 && adcvalueh<=160){duty=110;;}
   if(adcvalueh>160 && adcvalueh<=192){duty=80;}
   if(adcvalueh>192 && adcvalueh<=224){duty=50;}
   if(adcvalueh>224 && adcvalueh<=255){duty=20;}
   OCR1AL=duty;     

   return;
}









int main(){
    
    TCCR1A =(1 << WGM10) | (1 << COM1A1);
    TCCR1B =(1 << WGM12) | (1 << CS11);
    ADMUX =0b01100000; // upo8etw mas endiaferei o adc0 pou einai to pontesiometro
    ADCSRA =0b10000111;
   
    DDRD |=0b00000000; //D input
    DDRB |=0b11111111; //B output
    PORTB=0b00000000;
    int mode=0;
   while(1){
        uint8_t input;
        input = PIND;
        _delay_ms(100);            //delay kuriws gia debouncing
        input = input & 192;    //apomonosi button p6-p7 as 0b11000000 = 192
        if(input==128)        {mode=1;}//  PRESSED P1
        else if(input==64)   {mode=2;} //  PRESSED P2

       if (mode==1){
          mode1();
       }
       if(mode==2) {
          mode2();
       }
    }
}
