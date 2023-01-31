; function: delay_0_5sec
;    Delay process for 0.5 secs
; Parameter: - 
; Returns: - 
; Registers: Unchanged 
    XDEF wait_10ms
    XDEF wait_025s

.init:  SECTION

INNER_SPEED:  EQU 2000
OUTER_SPEED:  EQU 40
OUTER_SPEED_25ms EQU 1000



wait_10ms: LDX #OUTER_SPEED                  
   delay1: LDY #INNER_SPEED
   delay2: DBNE Y, delay2      ; Decrement Y and branch to delay2 if not equal to 0
           DBNE X, delay1      ; Decrement X and branch to delay1 if not equal to 0                         
        
           RTS
           

wait_025s: LDX #OUTER_SPEED_25ms                  
delay1_25: LDY #INNER_SPEED
delay2_25: DBNE Y, delay2_25      ; Decrement Y and branch to delay2 if not equal to 0
           DBNE X, delay1_25      ; Decrement X and branch to delay1 if not equal to 0                         
        
           RTS