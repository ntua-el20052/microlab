#define F_CPU 16000000UL
#include<avr/io.h>
#include<avr/interrupt.h>
#include<util/delay.h>
#include<comunication.h>


uint8_t scan_row(uint8_t row){
    row=~row;
    PCA9555_0_write(REG_OUTPUT_1, row); //Set I0row as 1 to enable pull up
    uint8_t read=PCA9555_0_read(REG_INPUT_1);
    read=read>>4;
    return read;
}


uint16_t scan_keypad(){
    uint16_t tmp=0;
    tmp=scan_row(4);
    tmp=tmp<<4;
    tmp|=scan_row(3);
    tmp=tmp<<4;
    tmp|=scan_row(2);
    tmp=tmp<<4;
    tmp|=scan_row(1);
    return tmp;
}
uint16_t tmp=0,tmp2=0;











int main(void) {
 twi_init();
 PCA9555_0_write(REG_CONFIGURATION_0, 0x00); //Set EXT_PORT0 as output
 PCA9555_0_write(REG_CONFIGURATION_1, 0xF0); //Set for EXT_PORT1 P4-P7 as input and P0 as output
 PCA9555_0_write(REG_OUTPUT_1, 0x01); //Set I01 as 1 to enable pull up

 while(1)
 {
    uint8_t read;
    read=PCA9555_0_read(REG_INPUT_1);
     /*if(read==0x81){
       PCA9555_0_write(REG_OUTPUT_0, 0x08);  
      // _delay_ms(1000);
     }
    if(read==0x41){
       PCA9555_0_write(REG_OUTPUT_0, 0x04);  
       //_delay_ms(1000);
     }
    if(read==0x21){
       PCA9555_0_write(REG_OUTPUT_0, 0x02);  
       //_delay_ms(1000);
     }
    if(read==0x11){
       PCA9555_0_write(REG_OUTPUT_0, 0x01);  
       //_delay_ms(1000);
     }*/
     
   read=read>>4;  
     
 PCA9555_0_write(REG_OUTPUT_0, read);

 }
}
