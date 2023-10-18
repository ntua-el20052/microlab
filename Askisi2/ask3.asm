.include "m328PBdef.inc"
 
.equ FOSC_MHZ=16	;MHz
.equ DEL_mS1=30		;mS
.equ DEL_mS2=500	;mS
.equ DEL_NU1= FOSC_MHZ*DEL_mS1 
.equ DEL_NU2= FOSC_MHZ*DEL_mS2
    
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

LDI r20,0X00
out PORTB,r20

ldi r21,0x01
ldi r22,0x64
ldi r23,0xFF

ISR1:
    
start:	in r17,PIND
	sbrs r17, 3
	rjmp skip
	out PORTB,r21
	ldi r26, low(DEL_NU1-1)
	ldi r25, high(DEL_NU1-1)
	rcall delay_mS
	dec r22
	brne start
	ret
skip:	out PORTB,r23
	ldi r26, low(DEL_NU2-1)
	ldi r25, high(DEL_NU2-1)
	rcall delay_mS
	ldi r22,0x64
	rjmp start
    
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



