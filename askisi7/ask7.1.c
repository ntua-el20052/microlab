#define F_CPU 16000000UL
#include<avr/io.h>
#include<avr/interrupt.h>
#include<util/delay.h>
#include "twi.h"

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
      temp &=0x01;
      temp=temp<<4;
      PORTD=temp;
      _delay_ms(0.58);
      DDRD &=0b11101111;
      PORTD &=0b11101111;
      _delay_ms(0.01); 
  }
  uint16_t one_wire_receive_byte(){
      uint16_t temp,result;
      result=0x00;
      for(int i=0;i<8;i++){
          temp=one_wire_receive_bit();
          result=result>>1;
          if(temp==1){
              result|=0x80;
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
          temp=temp>>1;
      }
  }
  int main(void){
      twi_init();
      PCA9555_0_write(REG_CONFIGURATION_0, 0x00); //Set EXT_PORT0 as output
      uint16_t init1,init2,result;
      init1=one_wire_reset();
      while(init1==0x0001){
         one_wire_transmit_byte(0xCC);
         _delay_ms(100);
         one_wire_transmit_byte(0x44);
         if(one_wire_receive_bit()==1){
             break;
      }
      }
      init2=one_wire_reset();
      while(init2==0x0001){
         one_wire_transmit_byte(0xCC);
         _delay_ms(100);
         one_wire_transmit_byte(0x34);
         result=one_wire_receive_byte();
      }
      }
      
      
  
