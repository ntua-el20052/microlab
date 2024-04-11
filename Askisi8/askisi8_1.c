#define F_CPU 16000000UL
#include<avr/io.h>
#include<avr/interrupt.h>
#include<util/delay.h>
#include <stddef.h>
#include "askisi5.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

void lcd_init() {
    // Wait for 250 milliseconds
    _delay_ms(250);

    // Switch to 8-bit mode (3 times as per your assembly code)
    for (int i = 0; i < 3; ++i) {
        PCA9555_0_write(REG_OUTPUT_0, 0x30);
        enable_pulse();
        _delay_ms(1);
    }

    // Switch to 4-bit mode
    PCA9555_0_write(REG_OUTPUT_0, 0x20);
    enable_pulse();
    _delay_ms(1);

    // Set display parameters
    lcd_command(0x28); // 5x8 dots, 2 lines
    lcd_command(0x0C); // Display on, cursor off
    lcd_clear_display();
    lcd_command(0x06); // Increase address, no display shift
}

void enable_pulse() {
    uint16_t temp=PCA9555_0_read(REG_INPUT_0);
    PCA9555_0_write(REG_OUTPUT_0, (temp|0x08));  // Set Enable (PD3) high
    _delay_ms(1);
    uint16_t temp1=PCA9555_0_read(REG_INPUT_0);
    PCA9555_0_write(REG_OUTPUT_0,(temp1 & 0xF7));  // Clear Enable (PD3) low
}

void lcd_command(uint8_t command) {
    // clear LCD_RS (PD2) for instruction
    uint16_t temp=PCA9555_0_read(REG_INPUT_0);
    PCA9555_0_write(REG_OUTPUT_0, (temp & 0b11111011));
    // Send instruction using write_2_nibbles function
    write_2_nibbles(command);

    _delay_ms(1);
}

void write_2_nibbles(uint8_t data) {
    uint8_t temp = PCA9555_0_read(REG_INPUT_0) & 0x0f;
    
    uint8_t hi=data & 0xf0;
    hi= hi + temp;
    PCA9555_0_write(REG_OUTPUT_0, hi);
    uint16_t temp1=PCA9555_0_read(REG_INPUT_0);
    PCA9555_0_write(REG_OUTPUT_0,(temp1 | 0x08));
    _delay_ms(0.0050);
    uint16_t temp2=PCA9555_0_read(REG_INPUT_0);
    PCA9555_0_write(REG_OUTPUT_0, (temp2 & 0b11110111));
    
    uint8_t lo=data & 0x0f;
    lo= lo << 4;
    lo= lo + temp;
    PCA9555_0_write(REG_OUTPUT_0, lo);
    uint16_t temp3=PCA9555_0_read(REG_INPUT_0);
    PCA9555_0_write(REG_OUTPUT_0,(temp3 | 0x08));
    _delay_ms(0.005);
    uint16_t temp4=PCA9555_0_read(REG_INPUT_0);
    PCA9555_0_write(REG_OUTPUT_0, (temp4 & 0b11110111));
    
    
   
}

void lcd_clear_display() {
  // Clear display command
    lcd_command(0x01);
    // Wait 5 mSec
    _delay_ms(10);
}
void lcd_data(uint8_t data) {
    // Set LCD_RS (PD2) to 1 for data
    uint16_t temp=PCA9555_0_read(REG_INPUT_0);
    PCA9555_0_write(REG_OUTPUT_0, (temp | 0x04));

    // Send data using write_2_nibbles function
    write_2_nibbles(data);

    // Wait for 250 millioseconds opte 1 micro einai ok
    _delay_ms(0.250);
}

void lcd_data_string(const char *data) {
    // Set LCD_RS (PD2) to 1 for data
    uint16_t temp = PCA9555_0_read(REG_INPUT_0);
    PCA9555_0_write(REG_OUTPUT_0, (temp | 0x04));

    // Loop through each character in the string
    for (size_t i = 0; data[i] != '\0'; ++i) {
        // Send data using write_2_nibbles function for each character
        write_2_nibbles(data[i]);

        // Wait for 250 milliseconds (you can adjust this delay as needed)
        _delay_ms(1);
    }
}

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
void usart_transmit_string(const char *a){
    for (int i=0; i< strlen(a); i++){
        usart_transmit(a[i]);
    }    
}
void usart_receive_string(){
    char a[50];
    for(int i=0; i<50; i++){
        a[i]=usart_receive();
        if (a[i]=='\n')break;
    }
    for(int i=0; i<50; i++){
        if(a[i]!='\n')lcd_data(a[i]);
       else break;
    }
    _delay_ms(2000);
}


uint16_t one_wire_reset(){
  DDRD |=0x10;
  PORTD &=0b11101111;
  _delay_ms(0.48);
  DDRD &=0b11101111;
  PORTD &=0b11101111;
  _delay_ms(0.1);
  uint16_t temp = PIND;
  _delay_ms(0.38);
  temp &=0b00010000;
  if(temp==0b00000000){
      return 0x0001;
  }
  else{ 
      return 0x8000;
  }
}
  
uint16_t one_wire_receive_bit(){
  DDRD |=0x10;
  PORTD &=0b11101111;
  _delay_ms(0.002);
  DDRD &=0b11101111;
  PORTD &=0b11101111;
  _delay_ms(0.01);
  uint16_t temp=PIND;
  _delay_ms(0.38);
  temp &= 0b00010000;
  if(temp==0b00010000){
      temp=1;
  }
  else{ 
      temp=0;
  }
  _delay_ms(0.049);
  return temp; 
}
  
  
  void one_wire_transmit_bit(uint16_t data){
      uint16_t temp=data;
      DDRD |=0x10;
      PORTD &=0b11101111;
      _delay_ms(0.002);
      temp &=0x0001;
      temp=temp<<4;
      PORTD=temp;
      _delay_ms(0.058);
      DDRD &=0b11101111;
      PORTD &=0b11101111;
      _delay_ms(0.01); 
  }
  
  
  uint16_t one_wire_receive_byte(){
      uint16_t temp,result;
      result=0x0000;
      for(int i=0;i<16;i++){
          temp=one_wire_receive_bit();
          result=result>>1;
          if(temp==1){
              result|=0x8000;
          }
      }
      return result;
  }
  void one_wire_transmit_byte(uint16_t data){
      uint16_t temp=data;
      uint16_t temp2=0x01;
      for(int i=0;i<8;i++){
          if((temp|0b11111110)==0b11111111){
              one_wire_transmit_bit(temp2);
          }
          else one_wire_transmit_bit(0x00);
          temp=temp>>1;
      }
  }


int presicion (int a){
    while (a%10 ==0 && a!=0)a/=10;
    return a;
}
void adc_init(){
   
    return;
}

int mode2(){                                         //auto to while me to apokatw sxolia- isws xreiastei, trexei mexri na pati8ei allagi mode wste na glitwsw calls sth sinartisi mode2 
   ADCSRA |=(1 << ADSC);
   uint8_t temp; 
   while(1){
      temp=ADCSRA;
      temp=temp & 64; //0b01000000; apomonosi ADSC
      if(temp==0)break;
   }
   uint8_t adcvalueh,adcvaluel;
   int result;
   adcvaluel=ADCL;
   adcvalueh=ADCH;
   switch (adcvalueh) {
    case 1 ... 32:
        result=20;
        break;

    case 33 ... 64:
        result=16;
        break;

    case 65 ... 96:
        result=12;
        break;

    case 97 ... 128:
        result=10;
        break;

    case 129 ... 160:
        result=8;
        break;

    case 161 ... 192:
        result=4;
        break;

    case 193 ... 224:
        result=2;
        break;

    case 225 ... 255:
        result=0;
        break;

    default:
        result=20;
        break;
}
 
 return result;  
}


int main(void){
    twi_init();
    PCA9555_0_write(REG_CONFIGURATION_0, 0x00);
    PCA9555_0_write(REG_CONFIGURATION_1, 0xF0);
    lcd_init();
    _delay_ms(200);
    usart_init(103);
    DDRB=0xFF;
    char data[20];
    char answer[50];
    lcd_clear_display();
    usart_transmit_string("ESP:connect\n");
    char a=usart_receive();
    a=usart_receive();
    if(a=='S'){lcd_data_string("1.Success");}
    else{lcd_data_string("1.Fail");}
  
    _delay_ms(2000);
    lcd_clear_display();
    while (a!= '\n'){a = usart_receive();}
    usart_transmit_string("ESP:url:\"http://192.168.1.250:5000/data\"\n");
    a = usart_receive();
    a = usart_receive();
    if(a=='S'){lcd_data_string("2.Success");}
    else{lcd_data_string("2.Fail");}
    _delay_ms(2000);
    while (a!= '\n'){a = usart_receive();}
    while(1){
  
    
    uint16_t init1,init2,result,temp;
     init1=one_wire_reset();
     lcd_clear_display();
      
      if(init1==0x0001){
         one_wire_transmit_byte(0xCC);
         one_wire_transmit_byte(0x44);
         while(one_wire_receive_bit()==0);
      }
     
      init2=one_wire_reset();
      if(init2==0x0001){
         one_wire_transmit_byte(0xCC);
         one_wire_transmit_byte(0xBE);
         result=one_wire_receive_byte();
         temp=result;
          
        float deci;
        char string1[5],string2[5];
        if( result > 0x0800){
                result =~ result;
                result +=1;
        }
        
        deci=(float)(result & 0x0F)/16.0;
        int value2 = presicion((int)(deci*1000));
        result = result>>4;
        result=result+;
        sprintf(string1, "%d", result);
        sprintf(string2, "%d", value2);
        
        strcpy(data, string1);
        strcat(data, ".");
        strcat(data, string2);
       
        _delay_ms(2000);
      }
  
      ADMUX =0b01100000; 
      ADCSRA =0b10000111;
      int piesi;
      
      lcd_clear_display();
      
      char piesi_str[50];
      piesi=mode2(); 
      sprintf(piesi_str,"%d", piesi);  
      
      char status[100];
      sprintf(status,"OK");
      
     //sprintf(piesi_str,"%d", piesi);////;;;;;;;;;;
      for (int i = 0; i < 100; i++){
     char read1 = keypad_to_ascii();
     if(read1=='0'){
        sprintf(status,"NURSECALL");
     }
      }
      for (int i = 0; i < 100; i++){
     char read2=keypad_to_ascii();
     if(read2=='#'){
            sprintf(status,"OK");
                 
     }
      }
     
     if(piesi>12||piesi<4){
         sprintf(status,"CHECKPRESSURE");
     }
     if(result>37 || result<34){
        sprintf(status,"CHECKTEMP");
     }
    
    char payload[600];
   
    

   sprintf(payload, "ESP:payload:[{\"name\": \"temperature\",\"value\": \"");
   strcat(payload, data);
   strcat(payload,"\"},");
   strcat(payload, "{\"name\": \"pressure\",\"value\": \"");
   strcat(payload, piesi_str);
   strcat(payload, "\"},");
   strcat(payload, "{\"name\": \"team\",\"value\": \"50\"},");
   strcat(payload, "{\"name\": \"status\",\"value\": \"");
   strcat(payload, status);
   strcat(payload, "\"}]\n");
   
   lcd_data_string("T:");
   lcd_data_string(data);
   lcd_data_string(" P:");
   lcd_data_string(piesi_str);
   lcd_command(0xC0);
   lcd_data_string(status);
   _delay_ms(2000);
   
    
   
    lcd_clear_display(); 
    
    
    
    
    usart_transmit_string(payload);
   
    a = usart_receive();
    a = usart_receive();
    if(a=='S'){lcd_data_string("3.Success");}
    else{lcd_data_string("3.Fail");}
    _delay_ms(2000);
    lcd_clear_display(); 
    while (a!= '\n'){a = usart_receive();}
    usart_transmit_string("ESP:transmit\n");
       

    usart_receive_string();
    _delay_ms(2000);
    

}
}


    
