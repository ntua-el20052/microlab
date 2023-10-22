.include "m328PBdef.inc"
 
.equ FOSC_MHZ=16	;MHz
.equ DEL_mS=10	;mS
.equ DEL_NU= FOSC_MHZ*DEL_mS 


TABLE:
    .DW 0x0005,0x001A,0x002E,0x0042,0x0057,0x006B,0x007F,0x0094,0x00A8,0x00BD
    .DW 0x00D1,0x00E6,0x00FA

reset:
    ldi r24,low(RAMEND)
    out SPL,r24
    ldi r24,high(RAMEND)
    out SPH,r24  

    ser r26
    out DDRB, r26	    ;PORTB=out
    clr r26
    out DDRD,r26	    ;PortD == in
    
    ldi r16,(1 << WGM10) | (1 << COM1A1)
    sts TCCR1A, r16
    
    ldi r16,(1 << WGM12) | (1 << CS11)
    sts TCCR1A, r16
    
    ldi ZH,HIGH(TABLE*2)
    ldi ZL,LOW(TABLE*2)
    adiw ZL,14
    ld r17,Z
    
    
    
main:
    sts OCR1AL,r17
    ldi r24, low(DEL_NU-1)
    ldi r25, high(DEL_NU-1)
    rcall delay_mS
    in r18,PIND
    mov r19,r18
    andi r19,0x02
    breq syn
    mov r19,r18
    andi r19,0x04
    breq meion
    rjmp main
syn:
    cpi r17,0xFA
    breq main
    adiw ZL,2
    rjmp main
meion:
    cpi r17,0x05
    breq main
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
    


