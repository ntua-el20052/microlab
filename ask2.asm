.include "m328PBdef.inc"
    
ldi r16,0x45
ldi r17,0x23
ldi r18,0x21
ldi r19,0x01
ldi r24,0x01
ldi r25,0x02
ldi r26,0x04
ldi r27,0x05
ldi r28,0x05
 
loop:
    mov r20,r16
    com r20
    mov r21,r17
    com r21
    and r20,r21
    mov r21,r18
    com r21
    and r20,r21
    or r20,r19
    com r20
    mov r21,r16
    com r21
    or r21,r18
    mov r22,r17
    com r22
    mov r23,r19
    com r23
    or r22,r23
    and r21,r22
    add r16,r24
    add r17,r25
    add r18,r26
    add r19,r27
    dec r28
    brne loop
    