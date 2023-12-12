void usart_init(unsigned int ubrr){
UCSR0A=0;
UCSR0B=(1<<RXEN0)|(1<<TXEN0);
UBRR0H=(unsigned char)(ubrr>>8);
UBRR0L=(unsigned char)ubrr;
UCSR0C=(3 << UCSZ00);
return;
}

void usart_transmit(uint8_t data){
while(!(UCSR0A&(1<<UDRE0)));
UDR0=data;
}

uint8_t usart_receive(){
while(!(UCSR0A&(1<<RXC0)));
return UDR0;
}


int main(void){
    twi_init();
    PCA9555_0_write(REG_CONFIGURATION_0, 0x00);
    lcd_init();
    _delay_ms(200);
    usart_init(103);
    usart_transmit('ESP:connect');
    while(1){
        //lcd_clear_display();
   lcd_data(usart_receive());
    }
   
    
}
