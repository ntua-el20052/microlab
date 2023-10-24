.include "m328PBdef.inc"

.equ FOSC_MHZ=16	;MHz
.equ DEL_mS1=1500	;mS
.equ DEL_mS2=2000	;mS
.equ F1=FOSC_MHZ*DEL_mS1
.equ F2=FOSC_MHZ*DEL_mS2
	
 reset:
    ldi r24,low(RAMEND)
    out SPL,r24
    ldi r24,high(RAMEND)
    out SPH,r24  
    
    ser r24
    out DDRD,r24
    out PORTD,r24
    ldi r26,0x80
    out PORTD,r26
   
    ldi r24, low(F2-1)
    ldi r25, high(F2-1)
    rcall delay_mS
start:
    seT
    ldi r26,0x80
    ldi r27,0x08
    rjmp loop4
loop1:
    ldi r24, low(F1-1)
    ldi r25, high(F1-1)
    rcall delay_mS
loop4: 
    out PORTD,r26
    lsr r26
    dec r27
    brne loop1
    ldi r24, low(F2-1)
    ldi r25, high(F2-1)
    rcall delay_mS
    ldi r27,0x08
    ldi r26,0x01
    clT
    rjmp loop3
loop2:
    ldi r24, low(F1-1)
    ldi r25, high(F1-1)
    rcall delay_mS
loop3: 
    out PORTD,r26
    lsl r26
    dec r27
    brne loop2
    ldi r24, low(F2-1)
    ldi r25, high(F2-1)
    rcall delay_mS
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



