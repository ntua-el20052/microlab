.include "m328PBdef.inc"

.equ FOSC_MHZ=16	;MHz
.equ DEL_mS=10	;mS
.equ F1=FOSC_MHZ*DEL_mS
	
 reset:
    ldi r24,low(RAMEND)
    out SPL,r24
    ldi r24,high(RAMEND)
    out SPH,r24  
    
    ser r24
    out DDRD,r24
    out PORTD,r24
    
    clr r26
    
loop1:
    ldi r24, low(F1-1)
    ldi r25, high(F1-1)
    rcall delay_mS
    com r26
    out PORTD,r26
    rjmp loop1
    
    
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


