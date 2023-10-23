.include "m328PBdef.inc"
 
.equ FOSC_MHZ=16	;MHz
.equ DEL_mS=50	;mS
.equ DEL_NU= FOSC_MHZ*DEL_mS 
.def dc_value=r24


TABLE:
    .DW 0x0005,0x001A,0x002E,0x0042,0x0057,0x006B,0x007F,0x0094,0x00A8,0x00BD
    .DW 0x00D1,0x00E6,0x00FA

reset:
    ldi r24,low(RAMEND)
    out SPL,r24
    ldi r24,high(RAMEND)
    out SPH,r24  

    ldi r26,0b00111111
    out DDRB, r26	    ;PORTB=out
    clr r26
    out DDRD,r26	    ;PortD == in
    
    ;set TMR1Ain fast PWM 8-bit mode with non-inverted output and prescale=8
    ldi r16,(1<<OCIE1A)
    sts TIMSK1,r16
    ldi r16,(1 << WGM10) | (1 << COM1A1) 
    sts TCCR1A, r16
    ldi r16,(1 << WGM12) | (1 << CS11)
    sts TCCR1A, r16
    ;the table with the dc_values is set on Z and the first dc_value is 50%
    ;so its the 7th value on the TABLE
    ldi ZH,HIGH(TABLE*2)
    ldi ZL,LOW(TABLE*2)
    adiw ZL,12
  
    
   
    
    
main:
    lpm
    mov dc_value,r0
start:
    sts OCR1AL,dc_value
    ldi r24, low(DEL_NU-1)
    ldi r25, high(DEL_NU-1)
    rcall delay_mS
    sbic PINB,1
    rjmp syn
    sbic PINB,2
    rjmp meion
    rjmp start
syn:
    cpi dc_value,0xFA
    breq start
    adiw ZL,2
    rjmp main
meion:
    cpi dc_value,0x05
    breq start
    sbiw ZL,2
    rjmp main
    
delay_mS:
    ldi r23, 249
loop_inn:
    dec r23		    ; 1 cycle
    nop			    ; 1 cycle
    brne loop_inn	    ; 1 or 2 cycles
     
    sbiw r24 ,1		    ; 2 cycles
    brne delay_mS	    ; 1 or 2 cycles
 
    ret			    ;4 cycl
    


