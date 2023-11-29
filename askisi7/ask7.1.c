#define F_CPU 16000000UL
#include<avr/io.h>
#include<avr/interrupt.h>
#include<util/delay.h>
#include <stddef.h>
#include <string.h>

#include "twi.h"
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
    _delay_us(50);
    uint16_t temp2=PCA9555_0_read(REG_INPUT_0);
    PCA9555_0_write(REG_OUTPUT_0, (temp2 & 0b11110111));
    
    uint8_t lo=data & 0x0f;
    lo= lo << 4;
    lo= lo + temp;
    PCA9555_0_write(REG_OUTPUT_0, lo);
    uint16_t temp3=PCA9555_0_read(REG_INPUT_0);
    PCA9555_0_write(REG_OUTPUT_0,(temp3 | 0x08));
    _delay_us(50);
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
    _delay_ms(1);
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




  int main(void){
      twi_init();
      PCA9555_0_write(REG_CONFIGURATION_0, 0x00);
      uint16_t init1,init2,result;
      uint16_t temp1,temp2,temp3;
      int temp4=0;
      int temp5=0;
      while(1){
      init1=one_wire_reset();
      lcd_init();
    _delay_ms(200);
    lcd_clear_display();
    _delay_ms(100);
      
      if(init1==0x0001){
         one_wire_transmit_byte(0xCC);
         //_delay_ms(100);
         one_wire_transmit_byte(0x44);
         while(one_wire_receive_bit()==0);
      }
     
      init2=one_wire_reset();
      if(init2==0x0001){
         one_wire_transmit_byte(0xCC);
         //_delay_ms(1);
         one_wire_transmit_byte(0xBE);
         result=one_wire_receive_byte();

          
        float deci;
        char string1[5],string2[5];
        char *sign="+";
        if( result > 0x0800){
                result =~ value;
                result +=1;
                sign="-";
        }
        deci=float(result & 0x0F)/16.0;
        int value2 = presicion(int(deci*1000));
        result = result>>4;
        sprintf(string1, "%d", result);
        sprintf(string2, "%d", value2);
        char data[10];
        strcpy(data, sign);
        strcat(data, string1);
        strcat(data, ".");
        strcat(data, string2);
        lcd_data_string(data);
      }
         
         
      
      else if(init2==0x8000){
          lcd_data_string("No Device");
          _delay_ms(1000);          
      }
      
      }
  }
      










/*


temp1=result & 0b1111100000000000;
         if(temp1==0b0000000000000000){
             lcd_data("+");
             temp2= result & 0b0000011111111111;
             temp2=temp2>>4;
             for(int i=0;i<7;i++){
                 temp3=temp2 & 0x0001;
                 if(temp3==0x0000){
                     temp5=0;
                 }
                 else{
                     temp5=1;
                 }
                 temp4=temp4+((temp5)*(pow(2, i)));
                 temp2=temp2>>1;
             }
             lcd_data_string(temp4);
             _delay_ms(1000);
         }
 */ 
