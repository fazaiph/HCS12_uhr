        XDEF decToASCII

; Defines

; RAM: Variable data section
.data:  SECTION
teiler:   DS.W 1    

; ROM: Constant data
.const: SECTION

; ROM: Code section
.init: SECTION

;***********************************************************************
; Public interface function: decToASCII
; Write convert a decimal value into ASCII
; Parameter:
;   X string pointer. Points to Adress in RAM we want to save conversion
;   D passes value to be converted
; Returns: a decimal value converted into ASCII into Adress X points to
; Registers: Unchanged

decToASCII:

        PSHY                  ;push to stackpos:1     save registers before changing values in them
        PSHX                  ;push to stackpos:2
        PSHD                  ;push to stackpos:3
        
        EXG X,Y
        MOVW #10000, teiler   ;initialize teiler with 10000
        TSTA                  ;test if our value is negativ. If so we write a '-'
        BMI negative          ;otherwise we write ' '(blank)
        MOVB #' ',Y
        PSHD                  ;push to stackpos:4
        BRA convers

negative:
        MOVB #'-',Y
        COMA
        COMB
        ADDD #1
        PSHD                  ;push to stackpos:4
        
convers:
        INY
        PULD                  ;Pull from Stackpos:4
        LDX teiler
        IDIV                  ;D/X-> result in X rest in D
        PSHD                  ;push to stackpos 4
        EXG X,D               ;result(Q) in D        
        ADDD #'0'             ;string= Q+'0' 
        STAB Y
        LDD teiler            ;divide teiler with 10 as long as teiler is not 0!
        LDX #10
        IDIV                  ;teiler/10--> new teiler in X
        STX teiler
        CPX #0      
        BNE convers           ;if teiler is 0 then the conversion is complete
        
        INY
        MOVB #$00,Y           ;add the zero terminating zero
        
        PULD                  ;restore registers
        PULD
        PULX
        PULY
        RTS
        
        
        
        