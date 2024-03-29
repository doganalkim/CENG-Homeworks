PROCESSOR 18F8722

#include <xc.inc>

; CONFIGURATION (DO NOT EDIT)
; CONFIG1H
CONFIG OSC = HSPLL      ; Oscillator Selection bits (HS oscillator, PLL enabled (Clock Frequency = 4 x FOSC1))
CONFIG FCMEN = OFF      ; Fail-Safe Clock Monitor Enable bit (Fail-Safe Clock Monitor disabled)
CONFIG IESO = OFF       ; Internal/External Oscillator Switchover bit (Oscillator Switchover mode disabled)
; CONFIG2L
CONFIG PWRT = OFF       ; Power-up Timer Enable bit (PWRT disabled)
CONFIG BOREN = OFF      ; Brown-out Reset Enable bits (Brown-out Reset disabled in hardware and software)
; CONFIG2H
CONFIG WDT = OFF        ; Watchdog Timer Enable bit (WDT disabled (control is placed on the SWDTEN bit))
; CONFIG3H
CONFIG LPT1OSC = OFF    ; Low-Power Timer1 Oscillator Enable bit (Timer1 configured for higher power operation)
CONFIG MCLRE = ON       ; MCLR Pin Enable bit (MCLR pin enabled; RE3 input pin disabled)
; CONFIG4L
CONFIG LVP = OFF        ; Single-Supply ICSP Enable bit (Single-Supply ICSP disabled)
CONFIG XINST = OFF      ; Extended Instruction Set Enable bit (Instruction set extension and Indexed Addressing mode disabled (Legacy mode))
CONFIG DEBUG = OFF      ; Disable In-Circuit Debugger


GLOBAL innervar
GLOBAL middlevar
GLOBAL outvar
GLOBAL blinkled
GLOBAL re_one_prev
GLOBAL re_one_click
GLOBAL portbrunning
GLOBAL portbstate
GLOBAL re_zero_prev
GLOBAL re_zero_click
GLOBAL portcrunning
GLOBAL portcstate

; Define space for the variables in RAM
PSECT udata_acs
innervar:
    DS 1 ; Allocate 1 byte for var1
middlevar:
    DS 1 
outvar:
    DS 1
blinkled:
    DS 1
re_one_prev:
    DS 1
re_one_click:
    DS 1
portbrunning:
    DS 1
portbstate:
    DS 1
re_zero_prev:
    DS 1
re_zero_click:
    DS 1
portcrunning:
    DS 1
portcstate:
    DS 1

PSECT resetVec,class=CODE,reloc=2
resetVec:
    

PSECT CODE
init:    
    
    clrf TRISB,A
    clrf TRISC,A
    clrf TRISD,A
    
    movlw 0xFF
    movwf PORTB
    movwf PORTC
    movwf PORTD
    
    setf TRISE,A
    
    
    movlw 0x10
    movwf outvar
    
    setf blinkled
    
    movlw 0x00
    movwf re_one_prev
    movwf re_zero_prev
    
    clrf re_one_click
    clrf portbstate
    clrf portbrunning
    
    clrf re_zero_click
    clrf portcstate
    clrf portcrunning

;PSECT CODE
main:
    
   call busy_wait
   
   setf blinkled
   
   call turn_off
   
   call input
    
busy_wait:
    
    out_loop:
	movlw 0x9F
	movwf middlevar
	middle_loop:
	    movlw 0x82
	    movwf innervar
	    inner_loop:
		decf innervar
		bnz inner_loop
	    decf middlevar
	    bnz middle_loop
	decf outvar
	bnz out_loop
    return

    
turn_off:
    clrf PORTB
    clrf PORTC
    clrf PORTD
    return
    
input:
    call blink_setter
    
    call waiter 
    
    call portb_caller
    
    call portc_caller
    
    goto input
    
    
blink_setter:
    incfsz blinkled
    goto blinkled_1
    blinkled_0: ;blinkled was 0xFF, it becomes 0 and comes to here.
	clrf PORTD
	return
    blinkled_1:
	movlw 0x01
	movwf PORTD
	setf blinkled
    return
    
waiter:
    movlw 0xFF
    movwf innervar
    
    movlw 0xFF
    movwf middlevar
    
    movlw 0x06
    movwf outvar
    
    out_loop_waiter:
	movlw 0x41
	movwf middlevar
	middle_loop_waiter:
	    movlw 0x31
	    movwf innervar
	    inner_loop_waiter:
		call re_one_check
		call re_zero_check
		decf innervar
		bnz inner_loop_waiter
	    decf middlevar
	    bnz middle_loop_waiter
	decf outvar
	bnz out_loop_waiter
    return
    
re_one_check:
    movlw 0x01
    tstfsz re_one_prev ; if re_one_prev was zero, then skip. THis means go to rev_One_prev_zero.
    goto re_one_prev_one
    re_one_prev_zero: ; RE1 was 0
	movlw 0x02
	cpfslt PORTE ; if PORTE is smaller than2 , then skip. It means ER1 is still 0. Else call re_one_become_one
	call re_one_become_one
	return
    re_one_prev_one: ; RE1 was 1
	movlw 0x01
	cpfsgt PORTE ; if re_one_prev is greater than 1, then skip . It means it becomes is still 1. Else call re_one_become_zero
	call re_one_become_zero
	return 
	
    return
    
re_zero_check:
    movlw 0x01
    tstfsz re_zero_prev ; if re_zero_prev was zero, then skip. THis means go to re_zero_prev_zero
    goto re_zero_prev_one
    re_zero_prev_zero: ; Here the prev state was zero
	movlw 0x01
	andwf PORTE
	bnz re_zero_become_one 
	return
    re_zero_prev_one:; HERE the prev state was one
	movlw 0x01
	andwf PORTE
	bz re_zero_become_zero
	return

re_one_become_one:
    movlw 0x01
    movwf re_one_prev
    return
    
re_zero_become_one:
    movlw 0x01
    movwf re_zero_prev
    return
    
re_one_become_zero:
    movlw 0x01
    movwf re_one_click
    movlw 0x00
    movwf re_one_prev
    return
    
re_zero_become_zero:
    movlw 0x01
    movwf re_zero_click
    movlw 0x00
    movwf re_zero_prev
    return
    
portb_caller:
    movlw 0x00
    cpfsgt re_one_click ; if clicked, then skip the next command. If not clicked, then go on with the current mode
    goto teststate ; if we chose this, this means we should go on with the current mode
    goto testportb
    
    teststate: 
	cpfseq portbrunning ; if port b is not running, then skip
	goto portb_setter ; port b is already running, keep this port running
        return ; do not make any change
	
    testportb:
	movlw 0x00
	movwf re_one_click
	cpfseq portbrunning ; portb is not running rn. if not running, then skip, else do not skip
	goto portb_stop
	goto portb_setter
    
    portb_stop:
	movlw 0x00
	movwf PORTB
	movwf portbrunning
	movwf portbstate
	return   
    
    portb_setter:
	movlw 0x01
	movwf portbrunning
	
	movlw 0x00
	cpfsgt portbstate
	goto bstatezero
	
	movlw 0x01
	cpfsgt portbstate
	goto bstateone
	
	movlw 0x02
	cpfsgt portbstate
	goto bstatetwo
	
	movlw 0x03
	cpfsgt portbstate
	goto bstatethree
	
	movlw 0x04
	cpfsgt portbstate
	goto bstatefour
	
	movlw 0x05
	cpfsgt portbstate
	goto bstatefive
	
	movlw 0x06
	cpfsgt portbstate
	goto bstatesix
	
	movlw 0x07
	cpfsgt portbstate
	goto bstateseven
	
	movlw 0x08
	cpfsgt portbstate
	goto bstateeight
    
    bstatezero:
	movlw 0x01
	movwf portbstate
	movwf PORTB
	return
	
    bstateone:
	movlw 0x02
	movwf portbstate
	movlw 0x03
	movwf PORTB
	return

    bstatetwo:
	movlw 0x03
	movwf portbstate
	movlw 0x07
	movwf PORTB
	return
     
    bstatethree:
	movlw 0x04
	movwf portbstate
	movlw 0x0F
	movwf PORTB
	return

    bstatefour:
	movlw 0x05
	movwf portbstate
	movlw 0x1F
	movwf PORTB
        return
   
    bstatefive:
	movlw 0x06
	movwf portbstate
	movlw 0x3F
	movwf PORTB
	return

    bstatesix:
	movlw 0x07
	movwf portbstate
	movlw 0x7F
	movwf PORTB
        return

    bstateseven:
	movlw 0x08
	movwf portbstate
	movlw 0xFF
	movwf PORTB
	return
	
    bstateeight:
        movlw 0x00
	movwf portbstate
	movwf PORTB
	return
	
portc_caller:
    movlw 0x00
    cpfsgt re_zero_click ; if clicked, then skip the next command. If not clicked, then go on with the current mode
    goto teststatec ; if we chose this, this means we should go on with the current mode
    goto testportc
    
    teststatec: 
	cpfseq portcrunning ; if port b is not running, then skip
	goto portc_setter ; port b is already running, keep this port running
        return ; do not make any change
	
    testportc:
	movlw 0x00
	movwf re_zero_click
	cpfseq portcrunning ; portb is not running rn. if not running, then skip, else do not skip
	goto portc_stop
	goto portc_setter
    
    portc_stop:
	movlw 0x00
	movwf PORTC
	movwf portcrunning
	movwf portcstate
	return   
    
    portc_setter:
	movlw 0x01
	movwf portcrunning
	
	movlw 0x00
	cpfsgt portcstate
	goto cstatezero
	
	movlw 0x01
	cpfsgt portcstate
	goto cstateone
	
	movlw 0x02
	cpfsgt portcstate
	goto cstatetwo
	
	movlw 0x03
	cpfsgt portcstate
	goto cstatethree
	
	movlw 0x04
	cpfsgt portcstate
	goto cstatefour
	
	movlw 0x05
	cpfsgt portcstate
	goto cstatefive
	
	movlw 0x06
	cpfsgt portcstate
	goto cstatesix
	
	movlw 0x07
	cpfsgt portcstate
	goto cstateseven
	
	movlw 0x08
	cpfsgt portcstate
	goto cstateeight

    cstatezero:
	movlw 0x01
	movwf portcstate
	movlw 0x80
	movwf PORTC
	return
	
    cstateone:
	movlw 0x02
	movwf portcstate
	movlw 0xC0    
	movwf PORTC
	return

    cstatetwo:
	movlw 0x03
	movwf portcstate
	movlw 0xE0
	movwf PORTC
	return
     
    cstatethree:
	movlw 0x04
	movwf portcstate
	movlw 0xF0
	movwf PORTC
	return

    cstatefour:
	movlw 0x05
	movwf portcstate
	movlw 0xF8
	movwf PORTC
        return
   
    cstatefive:
	movlw 0x06
	movwf portcstate
	movlw 0xFC
	movwf PORTC
	return

    cstatesix:
	movlw 0x07
	movwf portcstate
	movlw 0xFE
	movwf PORTC
        return

    cstateseven:
	movlw 0x08
	movwf portcstate
	movlw 0xFF
	movwf PORTC
	return
	
    cstateeight:
        movlw 0x00
	movwf portcstate
	movwf PORTC
	return	

    return

end resetVec