.include "m328PBdef.inc"
 
.equ FOSC_MHZ=16	;MHz
.equ DEL_mS=1000	;mS
.equ DEL_NU= FOSC_MHZ*DEL_mS  

 
.cseg
    .org 0x00		;Reset vector
    rjmp reset		;Jump to reset handler
    
   .org 0x2		;Start of interrupt vectors
    rjmp ISR0		;Jump to INT1 interrupt handler    
    
    
    
reset:
    ldi r24,low(RAMEND)
    out SPL,r24
    ldi r24,high(RAMEND)
    out SPH,r24  

;init PORTB as output
ser r26
out DDRC, r26	    ;PORTC=out
clr r26
out DDRB,r26	    ;PortB == in
    
ldi r26,(1<<ISC01)|(0<<ISC00)  ;Initialize external interrupt 1 (INT1)
sts EICRA,r26		    ;Falling edge generates interrupt
ldi r26,(1<<INT0)		    ;Configure INT1
out EIMSK,r26		    ;Enable INT1 interrupt
    
sei		;Enable global interrupts
    
    
    
    
loop1:
    clr r26
loop2:
    out PORTC, r26
    ldi r26, low(DEL_NU-1)
    ldi r25, high(DEL_NU-1)
    rcall delay_mS
    
    inc r26
    
    cpi r26, 31
    breq loop1
    rjmp loop2
    
delay_mS:
    nop
    nop
    ldi	r22, 248	    ; (1 cycle)	
loop_out:
    dec r22		    ; 1 cycle
    nop			    ; 1 cycle
    brne loop_out	    ; 1 or 2 cycles
     
    ;total group delay 996 cycles
delay_inner:		    
    ldi	r23, 249	    ; (1 cycle)	
loop_inn:
    dec r23		    ; 1 cycle
    nop			    ; 1 cycle
    brne loop_inn	    ; 1 or 2 cycles
     
    sbiw r24 ,1		    ; 2 cycles
    brne delay_inner	    ; 1 or 2 cycles
 
    ret			    ;4 cycles

ISR0:
    in r17,PINB
    ldi r18,0x05
    ldi r19,0x00
loop3:
    ror r17
    brcs meion
    rjmp syn
    
syn:
    inc r19
meion:
    dec r18
    brne loop3
    
    ldi r20,0x00
    cpi r19,0
    breq exit
loop4:
    seC
    rol r20
    dec r19
    brne loop4
exit:
    out PORTC,r20
    reti
    


