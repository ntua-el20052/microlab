.include "m328PBdef.inc" ;ATmega328P microcontroller definition
.def temp=r16
.def counter=r17
.def low_reg=r26
.def high_reg=r27
.def extra_high_reg=r19
.def extra_temp=r20
.org 0x0
rjmp init_progr
.org 0x2A
rjmp my_handler
init_progr:
    ldi temp, high(RAMEND)
    out SPH,temp
    ldi temp, low(RAMEND)
    out SPL,temp
    ser r24
    out DDRD, r24 ; set PORTD as output
    out DDRB,r24
    rcall lcd_init
    ldi r24, low(100)
    ldi r25, high(100) ; delay 100 mS
    rcall wait_msec
    ; REFSn[1:0]=01 => select Vref=5V, MUXn[4:0]=0010 => select ADC2(pin PC2)
    ; ADLAR=0 => right adjust the ADC result
    ldi temp, 0b01000010 ;
    sts ADMUX, temp
    ldi temp, 0b10000111 ; ADEN=1 => ADC Enable, ADCS=0 => No Conversion,
    ; ADIE=0 => disable adc interrupt, ADPS[2:0]=111 => fADC=16MHz/128=125KHz
    sts ADCSRA, temp ; we enable the ADC but do not allow a conversion to happen yet
Start_conv:
    ldi r24,low(1000)
    ldi r25,high(1000)
    rcall wait_msec ; wait 1 sec and then enable the ADC
    rcall lcd_clear_display
    sei
    lds temp, ADCSRA ;
    ori temp, (1<<ADSC) | (1<<ADIE) ; Set ADSC flag of ADCSRA and enable the interrupt
    sts ADCSRA, temp ;
    rjmp Start_conv
my_handler:
    ; here we try to compute and ouput the result
    ldi temp,5 ; 5 is the Vref
    lds extra_temp,ADCL
    mul extra_temp,temp ; we dont include ADCL(we already shifted the result of the conversion
    mov low_reg,r0 ; keep the result of ADCL and Vref
    mov high_reg,r1
    lds extra_temp,ADCH
    andi extra_temp,0b00000011 ; keep only the last three bbits
    mul extra_temp,temp
    add high_reg,r0 ; add the result of ADCH*Vref to the result of the previous multiplication
    ldi temp,100 ; multiply ADC with 100 to make a number with two decimal points an integer
    mul low_reg,temp ; we first multiply the previous low_register with 100
    mov low_reg,r0
    mov extra_temp,r1
    mul high_reg,temp ; then we multiply the previous high_register with 100
    mov high_reg,r0
    add high_reg,extra_temp ; we have to add the high result of low_reg*100 with
    ; the low result of high_reg *100
    mov extra_high_reg,r1 ; now we have the result of all the multiplications in three registers
    ldi counter,2
    mov low_reg,high_reg
    mov high_reg,extra_high_reg
rotate_2_times: ; now we will rotate the registers left 2 times because we devide with 2^10
; we rotate the registers just two times because we dont need the low_register
; if we rotate 10 times we would keep the low register but its contents would
; come from the other registers-> It's unnecessary to rotate 10 times
    ror high_reg
    ror low_reg
    dec counter
    brne rotate_2_times
    andi high_reg,1
    ; now we will make the binary number to a decimal
    sbrc high_reg,0
    rjmp count_hundreds
    cpi low_reg,99
    brlo zero_hundreds
    ldi counter,0
count_hundreds: ; we perform consecutive subtractions of 100 from the current number
; in order to count the number of hundreds
    inc counter
    sbiw low_reg,63 ; in sbiw the immediate is at most 63
    sbiw low_reg,37 ; so we make two subtractions 63+37=100
    sbrc high_reg,0 ; check if the curent number is lower than 100
    rjmp count_hundreds
    cpi low_reg,99
    brlo output_counter1
    cpi low_reg,99
    breq output_counter1
    rjmp count_hundreds
output_counter1:
    mov r24,counter
    ori r24,0b00110000 ; code to output a number
    rcall lcd_data
continue:
    ldi r24,0b00101110 ; output a dot
    rcall lcd_data
    ldi counter,0
    cpi low_reg,10
    brlo zero_decades
    ldi counter,0
count_decades:; we perform consecutive subtractions of 10 from the current number
; in order to count the number of decades
    inc counter
    subi low_reg,10
    cpi low_reg,10
    brge count_decades
output_counter2:
    mov r24,counter
    ori r24,0b00110000
    rcall lcd_data
continue_2:
    mov r24,low_reg
    ori r24,0b00110000
    rcall lcd_data
    reti
zero_hundreds:
    clr r24
    ori r24,0b00110000 ; code to output a number
    rcall lcd_data
    rjmp continue
zero_decades:
    clr r24
    ori r24,0b00110000 ; code to output a number
    rcall lcd_data
    rjmp continue_2
write_2_nibbles:
    push r24 ; save r24(LCD_Data)
    in r25 ,PIND ; read PIND
    andi r25 ,0x0f ;
    andi r24 ,0xf0 ; r24[3:0] Holds previus PORTD[3:0]
    add r24 ,r25 ; r24[7:4] <-- LCD_Data_High_Byte
    out PORTD ,r24 ;
    sbi PORTD ,3 ; Enable Pulse
    nop
    nop
    cbi PORTD ,3
    pop r24 ; Recover r24(LCD_Data)
    swap r24 ;
    andi r24 ,0xf0 ; r24[3:0] Holds previus PORTD[3:0]
    add r24 ,r25 ; r24[7:4] <-- LCD_Data_Low_Byte
    out PORTD ,r24
    sbi PORTD ,3;
    nop
    nop
    cbi PORTD ,3
    ret
lcd_data:
    sbi PORTD ,2 ; LCD_RS=1(PD2=1), Data
    rcall write_2_nibbles ; send data
    ldi r24 ,250 ;
    ldi r25 ,0 ; Wait 250uSec
    rcall wait_usec
    ret
lcd_command:
    cbi PORTD ,2 ; LCD_RS=0(PD2=0), Instruction
    rcall write_2_nibbles ; send Instruction
    ldi r24 ,250 ;
    ldi r25 ,0 ; Wait 250uSec
    rcall wait_usec
    ret
lcd_clear_display:
    ldi r24 ,0x01 ; clear display command
    rcall lcd_command
    ldi r24 ,low(5) ;
    ldi r25 ,high(5) ; Wait 5 mSec
    rcall wait_msec ;
    ret
lcd_init:
    ldi r24 ,low(200) ;
    ldi r25 ,high(200) ; Wait 200 mSec
    rcall wait_msec ;
    ldi r24 ,0x30 ; command to switch to 8 bit mode
    out PORTD ,r24 ;
    sbi PORTD ,3 ; Enable Pulse
    nop
    nop
    cbi PORTD ,3
    ldi r24 ,250 ;
    ldi r25 ,0 ; Wait 250uSec
    rcall wait_usec ;
    ldi r24 ,0x30 ; command to switch to 8 bit mode
    out PORTD ,r24 ;
    sbi PORTD ,3 ; Enable Pulse
    nop
    nop
    cbi PORTD ,3
    ldi r24 ,250 ;
    ldi r25 ,0 ; Wait 250uSec
    rcall wait_usec ;
    ldi r24 ,0x30 ; command to switch to 8 bit mode
    out PORTD ,r24 ;
    sbi PORTD ,3 ; Enable Pulse
    nop
    nop
    cbi PORTD ,3
    ldi r24 ,250 ;
    ldi r25 ,0 ; Wait 250uSec
    rcall wait_usec
    ldi r24 ,0x20 ; command to switch to 4 bit mode
    out PORTD ,r24
    sbi PORTD ,3 ; Enable Pulse
    nop
    nop
    cbi PORTD ,3
    ldi r24 ,250 ;
    ldi r25 ,0 ; Wait 250uSec
    rcall wait_usec
    ldi r24 ,0x28 ; 5x8 dots, 2 lines
    rcall lcd_command
    ldi r24 ,0x0c ; dislay on, cursor off
    rcall lcd_command
    rcall lcd_clear_display
    ldi r24 ,0x06 ; Increase address, no display shift
    rcall lcd_command ;
    ret
wait_msec:
    push r24 ; 2 cycles
    push r25 ; 2 cycles
    ldi r24 , low(999) ; 1 cycle
    ldi r25 , high(999) ; 1 cycle
    rcall wait_usec ; 998.375 usec
    pop r25 ; 2 cycles
    pop r24 ; 2 cycles
    nop ; 1 cycle
    nop ; 1 cycle
    sbiw r24 , 1 ; 2 cycles
    brne wait_msec ; 1 or 2 cycles
    ret ; 4 cycles
wait_usec:
    sbiw r24 ,1 ; 2 cycles (2/16 usec)
    call delay_8cycles ; 4+8=12 cycles
    brne wait_usec ; 1 or 2 cycles
    ret
delay_8cycles:
    nop
    nop
    nop
    ret
