;Export symbols
        XDEF checkButtonPressed
        XDEF checkSetButtons

; Import symbols
        XREF SETMODE
        XREF wait_10ms
        XREF wait_025s
        XREF pressedButton
        XREF stayInSetMode

; Include derivative specific macros
        INCLUDE 'mc9s12dp256.inc'

; Defines

; RAM: Variable data section
.data:  SECTION


; ROM: Constant data
.const: SECTION

.intVect: SECTION

; ROM: Code section
.init:  SECTION

initButtons:
            MOVB #$FF, DDRB                 ;initialize Port H for buttons
            MOVB #$00, DDRH
            RTS

;-----------------------SetModeButtons------------------------------------------------------ 

checkButtonPressed:
           BRSET PTH, #$08, SW2pressed        ;check if SW2 (Port H3) is pressed
           RTS
           
SW2pressed:                                   ;debounce by checking if button is still pressed after 10ms
           JSR wait_10ms                      ;wait 10ms 
           BRSET PTH, #$08, pressedSW2        ;check if button is still pressed
           BRA done                           ;if not it was just a bounced signal
           
pressedSW2:                                   ;button was really pressed
           PSHB                               ;store B
           LDAB SETMODE                       ;load setmode in B register
           jsr wait_025s
           CMPB  #0                           ;check if B in this case setmode was active before
           BEQ  activate                      ;if not activate setmode
;deactivate
           LDAB #0                             ;else deactivate setmode by substracting 1 from B register
           STAB SETMODE
           LDAB #$00
           STAB stayInSetMode
           LDAB #$80
           STAB TSCR1                         ;activate timer
           PULB                               ;restore B
           BRA done
           
activate:  
           ADDB #1                            ;set Setmode by incrementing B by 1
           STAB SETMODE
           LDAB #$00
           STAB TSCR1                         ;Stop timer
           PULB 
done:      
           RTS        
           
           
;-----------------------SetButtons----------------------------------------------------------                
           
checkSetButtons:
           
           BRSET PTH, #$04, SW3pressed        ;check if SW3 is pressed
           BRSET PTH, #$02, SW4pressed        ;check if SW4 is pressed
           BRSET PTH, #$01, SW5pressed        ;check if SW5 is pressed
           RTS
           
SW3pressed:                                   ;debounce by checking if button is still pressed after 10ms
           JSR wait_10ms                      ;wait 10ms 
           BRSET PTH, #$04, pressedSW3        ;check if button is still pressed
           BRA SetModeDone                           ;if not it was just a bounced signal
           
pressedSW3:                                   ;button was really pressed
           PSHB
           LDAB #4
           STAB pressedButton
           PULB
           jsr wait_025s
           BRA SetModeDone


SW4pressed:                                   ;debounce by checking if button is still pressed after 10ms
           JSR wait_10ms                      ;wait 10ms 
           BRSET PTH, #$02, pressedSW4        ;check if button is still pressed
           BRA SetModeDone                           ;if not it was just a bounced signal
           
pressedSW4:                                   ;button was really pressed
           PSHB
           LDAB #2
           STAB pressedButton
           PULB
           jsr wait_025s
           BRA SetModeDone 
           
           


SW5pressed:                                   ;debounce by checking if button is still pressed after 10ms
           JSR wait_10ms                      ;wait 10ms 
           BRSET PTH, #$01, pressedSW5        ;check if button is still pressed
           BRA SetModeDone                           ;if not it was just a bounced signal
           
pressedSW5:                                   ;button was really pressed
           PSHB
           LDAB #1
           STAB pressedButton
           PULB
           jsr wait_025s
           BRA SetModeDone     
           
           

SetModeDone:
           RTS
           

