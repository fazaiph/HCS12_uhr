; Export symbols
        XDEF ADtoTemp,initAD

; Import symbols
        XREF __SEG_END_SSTACK   ; End of stack
        XREF temp
        XREF maxTemp
        XREF minTemp
        XREF adcResolution
        XREF tempOutput

; Include derivative specific macros
        INCLUDE 'mc9s12dp256.inc'


; RAM: Variable data section
.data:  SECTION
range: ds.w 1

; ROM: Constant data
.const: SECTION                        

; Interrupt Vector Table
.vect:  SECTION
        org $FFD2
init22: dc.w adcIsr


; ROM: Code section
.init:  SECTION 


; --------------------------------------------------------------------------------------
; initAD: Inintalisiert den AD-Wandler

initAD:
      movw #25, temp
			movb #%11000010, ATD0CTL2		; enable atd0 & enable interrupt
			movb #%00001000, ATD0CTL3		; Automatische Abwicklung einer Messung
			movb #%00000101, ATD0CTL4		; 10Bit Aufloesung
			movb #%10000111, ATD0CTL5		; starte eine Messung an Kanal 7
			RTS

; --------------------------------------------------------------------------------------
; ADtoTemp: Wandelt AD in einen Temperaturwert um und gibt ihn auf dem LCD-Dislay aus

ADtoTemp: 
          ldd maxTemp
          subd minTemp
          std range
			    ldd temp
			    ldy range				  ; Formel: (x*100)/1023 - 30 
			    emul
			    ldx adcResolution
			    ediv
			    exg y,d
		    	addd minTemp      ; -30
		    	std tempOutput
		    	RTS
			
			
			
			
			
adcIsr:   ldd ATD0DR0                ; Read the result registers and compute average of 4 measurements
          addd ATD0DR1
          addd ATD0DR2
          addd ATD0DR3
          lsrd
          lsrd
          std temp
          bset ATD0CTL2, #$01         ; Reset interrupt flag (bit 0 in ATD0CTL2)
          movb #%10000111, ATD0CTL5   ; Start next measurement on single channel 7
          rti