.include "m328PBdef.inc"
 
.equ FOSC_MHZ=16	;MHz
.equ DEL_mS1=30	;mS
.equ DEL_mS2=50=	;mS
.equ DEL_NU1= FOSC_MHZ*DEL_mS1 
.equ DEL_NU2= FOSC_MHZ*DEL_mS2
.equ DEL_NU5= FOSC_MHZ*5
.def led = r16
.DEF ledall= r17
.DEF counter=r18
.def flag= r19
.cseg
    .org 0x00		;Reset vector
    rjmp reset		;Jump to reset handler
    
    .org 0x4		;Start of interrupt vectors
    rjmp ISR1		;Jump to INT1 interrupt handler 

reset:
    ldi r24,low(RAMEND)
    out SPL,r24
    ldi r24,high(RAMEND)
    out SPH,r24  
    
ldi flag,0x00  

;init PORTB as output
ser r26
out DDRB, r26	    ;PORTB=out
clr r26
out DDRD,r26	    ;PortD == in

ldi r26,(1<<ISC11)|(0<<ISC10)  ;Initialize external interrupt 1 (INT1)
sts EICRA,r26		    ;Falling edge generates interrupt
ldi r26,(1<<INT1)		    ;Configure INT1
out EIMSK,r26		    ;Enable INT1 interrupt

sei		;Enable global interrupts
  

ldi ledall,0xff 
ldi led,0x00

    
main_loop:
    cpi flag,0
    brne open
    out PORTB,led
    rjmp main_loop
open:
    ldi led,0x01
    out PORTB,led
    ldi r24, low(DEL_NU1-1)
    ldi r25, high(DEL_NU1-1)
    rcall delay_mS
    dec counter
    brne open
    ldi flag,0x00
    ldi led,0x00
    out PORTB, led
    rjmp main_loop


ISR1:
    push r25
    push r24
    in r20, SREG  ;???????
    push r20
    
    
    cpi flag,0
    brne all
    ldi flag,0x01
    ldi counter,0x64
    pop r20
    out SREG, r20   
    pop r24
    pop r25
    reti
all:
    out PORTB,ledall
    ldi r24, low(DEL_NU2-1)
    ldi r25, high(DEL_NU2-1)
    rcall delay_mS
    ldi counter,0x64
    pop r20
    out SREG, r20 
    pop r24
    pop r25
    reti

delay_mS:
    ldi r23, 249
loop_inn:
    dec r23		    ; 1 cycle
    nop			    ; 1 cycle
    brne loop_inn	    ; 1 or 2 cycles
     
    sbiw r24 ,1		    ; 2 cycles
    brne delay_mS	    ; 1 or 2 cycles
 
    ret			    ;4 cycle
	
    
    
    
    


