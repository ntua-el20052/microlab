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
    PORTB=0xFF;
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
    DDRB= 0xFF;  //PORTB as output
    PORTB=0x00;  //start with LEDs off

    int i;
    while(1){
        if (flag==0)PORTB=0x00;
        if (flag == 1){
            _delay_ms(10);
            flag=0;
            _delay_ms(10);
            PORTB=0x01;
            for(i=0; i<50; i++){
                _delay_ms(60);
                if (flag==1) {on(); flag=1; break;}
            }
        }
    }
}