#define F_CPU 16000000UL
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
int flag=0;
ISR (INT1_vect){
flag=1;
}

void on(){
    sei();
    flag=0;
    PORTB=0x1F;
    PORTC=flag;
    int i;
    for(i=0; i<10; i++){
    _delay_ms(35);
    if (flag==1){on(); return;}
    }


}

int main()
{
    EICRA=(1 << ISC10)|(1 << ISC11); //interrupt on every INT1
    EIMSK = (1 << INT1);
    sei();
    DDRC = 0xFF;
    DDRB= 0xFF;  //PORTB as output
    PORTB=0x00;  //start with LEDs off

    int i;
  //  DDRD= 0x00;  //PD3 as input
    while(1){
        PORTC=flag;
        if (flag==0)PORTB=0x00;
        if (flag == 1){
            _delay_ms(10);
            flag=0;
            _delay_ms(10);
            PORTB=0x01;
            PORTC = flag;
            for(i=0; i<50; i++){
                //PORTB=0x0F;
                _delay_ms(60);
                if (flag==1) {PORTC = flag; on(); flag=1; break;}
            }
        }
    }
}
